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
PKG_FILES ?= docs/README.md docs/images/logo.png $(TARGET)

.PHONY: all clean pkg help

INSTALL_PATH?=.

# Default platform detection
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    _PLATFORM := linux
else ifeq ($(UNAME_S),Darwin)
    _PLATFORM := macos
else ifeq (,$(findstring $(UNAME_S),MINGW MSYS))
    _PLATFORM := windows
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

# Allow user to override toolchain via command line
TOOLCHAIN?=$(_TOOLCHAIN)
include $(TOOLCHAIN)

# Set build mode
config?=default
ifeq ($(config),debug)
	CCFLAGS+=-g -fsanitize=address
else ifeq ($(config),static)
	CCFLAGS += -static
	LDLIBS += -ltinfo
endif

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
	@echo Installing to $(INSTALL_PATH)/diabaig
	@mkdir -p $(INSTALL_PATH)
	@cp $(TARGET) $(INSTALL_PATH)/diabaig

#debug: CCFLAGS+=-g -fsanitize=address
#debug: all 
#
#static: CCFLAGS += -static
#static: LDLIBS += -ltinfo
#static: all

clean:
	@rm -rf build

pkg: $(TARGET)
	@mkdir -p build/diabaig.$(MOD)
	@cp $(PKG_FILES) build/diabaig.$(MOD)
	@sed 's/VERSION/$(VERSION)/g' docs/README.md > build/diabaig.$(MOD)/README.md
	@cd build && zip -r diabaig.$(MOD).zip diabaig.$(MOD)/
	@rm -r build/diabaig.$(MOD)

define DOCS
Usage: make [config=name] [options] [target]

    CONFIGURATIONS:
        debug          - build with debug flags
        static         - build with static libraries if available

    OPTIONS:
        PLATFORM=      - cross-compile to [linux,macos,windows] 
        TOOLCHAIN=     - build with custom toolchain
        INSTALL_PATH=  - set binary install path (defaults .)

    TARGET:
        all (defaults) - builds diabaig
        install        - install binary to [INSTALL_PATH]
        clean          - cleans the working directory
        pkg            - builds package (.zip file) with binary and README files

endef

help:
	@$(info $(DOCS))
