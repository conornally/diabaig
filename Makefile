VERSION=$(shell cat VERSION.txt)

CC=gcc
CCFLAGS=-Wall -DVERSION=\"$(VERSION)\"\
		-Wno-unused-command-line-argument\
		-Wextra
INCLUDE=-Iinclude -Ibuild
LDLIBS=-lm -lncurses

BUILD_DIR=build/linux
TARGET=${BUILD_DIR}/diabaig
SRC=$(wildcard src/*.c)
HEADERS=$(wildcard include/*.h)

DATA_RAW=$(wildcard res/*.txt)
DATA_HEADER=build/data_embedded.h
DATA_EMBED =src/data_embedded.c
HEADERS+=$(DATA_HEADER)

MANPAGE = docs/diabaig.6
#MANPAGE_SRC = docs/diabaig.6.in

OBJ=$(SRC:src/%.c=${BUILD_DIR}/%.o)

ARCH=$(shell uname -m)
ifeq ($(ARCH),x86_64)
	DEB_ARCH=amd64
else ifeq ($(ARCH),aarch64)
	DEB_ARCH=arm64
else ifeq ($(ARCH),armv71)
	DEB_ARCH=armhf
else
	DEB_ARCH=unkown
endif
PKG_DIR=build/diabaig.$(DEB_ARCH)

.PHONY: all clean install package static

all:${BUILD_DIR} $(DATA_HEADER) $(TARGET)
	@cp $(TARGET) .
$(TARGET):$(OBJ) 
	$(CC) $(CCFLAGS) $(INCLUDE) -o $@ $(OBJ) $(LDLIBS) 
${BUILD_DIR}/%.o:src/%.c $(HEADERS)
	$(CC) $(CCFLAGS) $(INCLUDE) -o $@ -c $< $(LDLIBS) 

$(DATA_HEADER): $(DATA_EMBED)
	@echo "Generating embedded data header"
	@echo "//Auto generated header" > $@
	@echo "#ifndef DATAHEADER_H">> $@
	@echo "#define DATAHEADER_H\n" >> $@
	grep const $< | sed 's/const/extern const/'| sed 's/\ =.*/;/' >> $@
	@echo "\n#endif//DATAHEADER_H" >> $@

$(DATA_EMBED): $(DATA_RAW)
	@echo "Generating embedded data src"
	@echo "//Auto generated embedded data" > $@
	@echo "#include \"data_embedded.h\"\n">> $@
	@for f in $^; do xxd -i $$f | sed 's/unsigned/const/'; done >> $@

${BUILD_DIR}:
	@mkdir -p ${BUILD_DIR}

clean:
	@rm -r build

#debug: CCFLAGS += -g -DDEBUG 
debug: CCFLAGS += -fsanitize=address
debug: all

install:
	cp ${TARGET} ${HOME}/.local/bin/

static: CCFLAGS += -static
static: LDLIBS += -ltinfo
static: clean all

package: static
	mkdir -p ${PKG_DIR}/usr/bin
	cp $(TARGET) $(PKG_DIR)/usr/bin
	cp -r docs/debian $(PKG_DIR)/
	sed -i 's/VERSION/${VERSION}/' ${PKG_DIR}/debian/control
	sed -i 's/VERSION/${VERSION}/' ${PKG_DIR}/debian/changelog
	sed -i 's/VERSION/${VERSION}/' ${PKG_DIR}/debian/diabaig.6
	sed -i 's/ARCH/${DEB_ARCH}/' ${PKG_DIR}/debian/control

	cd ${PKG_DIR} && dpkg-buildpackage -b -us -uc -nc
	#pandoc -V geometry:margin=1in -o build/guide.pdf ${PKG_DIR}/debian/diabaig.6
	rm -r ${PKG_DIR}
	cd build && rm *.ddeb *.buildinfo *.changes

