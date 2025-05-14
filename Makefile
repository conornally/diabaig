VERSION=$(shell cat VERSION.txt)

CC		?= gcc
CCFLAGS ?= -Wall -Wextra -DVERSION=\"$(VERSION)\"
INCLUDE ?= -Iinclude -Ibuild
LDLIBS  ?= -lncurses -lm

SRC=$(wildcard src/*.c)
HDR=$(wildcard include/*.h)
OBJ=$(SRC:src/%.c=$(BUILD)/%.o)


DATA_HEADER=build/data_embedded.h
DATA_EMBED =src/data_embedded.c
HDR+=$(DATA_HEADER)

MOD ?= 
PKG ?= build/diabaig.pkg
PKG_FILES ?= docs/README.md docs/images/logo.png

.PHONY: all clean package static debug

# Default platform detection
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    _PLATFORM := linux
else ifeq ($(UNAME_S),Darwin)
    _PLATFORM := macos
else ifeq ($(findstring MINGW,$(UNAME_S)))
    _PLATFORM := windows
else ifeq ($(findstring MSYS,$(UNAME_S)))
    _PLATFORM := windows
else
    _PLATFORM := unknown
endif

# Allow the user to override platform via command line
PLATFORM ?= $(_PLATFORM)

# Based on PLATFORM, select appropriate toolchain
ifeq ($(PLATFORM),linux)
	_TOOLCHAIN:=toolchain/linux.mk
else ifeq ($(PLATFORM),windows)
	_TOOLCHAIN:=toolchain/windows.mk
else ifeq ($(PLATFORM),macos)
	_TOOLCHAIN:=toolchain/macos.mk
else
    $(error Unsupported platform: $(PLATFORM))
endif

# Allow the user to override toolchain via command line
TOOLCHAIN?=$(_TOOLCHAIN)
include $(TOOLCHAIN)

$(DATA_HEADER): $(DATA_EMBED)
	@echo "Generating embedded data header"
	@echo "//Auto generated header" > $@
	@echo "#ifndef DATAHEADER_H">> $@
	@echo "#define DATAHEADER_H\n" >> $@
	@grep const $< | sed 's/const/extern const/'| sed 's/\ =.*/;/' >> $@
	@echo "\n#endif//DATAHEADER_H" >> $@

$(DATA_EMBED): $(wildcard res/*.txt)
	@echo "Generating embedded data src"
	@echo "//Auto generated embedded data" > $@
	@echo "#include \"data_embedded.h\"\n">> $@
	@for f in $^; do xxd -i $$f | sed 's/unsigned/const/'; done >> $@

all: $(TARGET)

$(TARGET): $(BUILD) $(OBJ) 
	$(CC) $(CCFLAGS) $(INCLUDE) -o$@ $(OBJ) $(LDLIBS)
$(BUILD)/%.o:src/%.c $(HDR)
	$(CC) $(CCFLAGS) $(INCLUDE) -o$@ -c $< $(LDLIBS)

$(BUILD):
	@mkdir -p $(BUILD)

install:
	@echo WARNING: make install not implemented
	@echo output file \'$(TARGET)\'
	@echo move this file to where ever you fancy
	cp $(TARGET) .

debug: CCFLAGS+=-g -fsanitize=address
debug: all 

static: CCFLAGS += -static
static: LDLIBS += -ltinfo
static: all

clean:
	@rm -rf build

pkg: $(TARGET)
	@mkdir -p build/diabaig.$(MOD)
	@cp $(PKG_FILES) $(TARGET) build/diabaig.$(MOD)
	@sed -i 's/VERSION/$(VERSION)/' build/diabaig.$(MOD)/README.md
	@cd build && zip -r diabaig.$(MOD).zip diabaig.$(MOD)/
	@rm -r build/diabaig.$(MOD)
