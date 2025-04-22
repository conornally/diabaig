CC=gcc
CCFLAGS += -DLINUX
BUILD=build/linux
TARGET=$(BUILD)/diabaig

all:$(TARGET)
	@cp $(TARGET) .

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
PKG=build/diabaig.$(DEB_ARCH)

package:
	mkdir -p $(PKG)/usr/bin
	cp $(TARGET) $(PKG)/usr/bin
	cp -r docs/debian $(PKG)/
	sed -i 's/VERSION/$(VERSION)/' $(PKG)/debian/control
	sed -i 's/VERSION/$(VERSION)/' $(PKG)/debian/changelog
	sed -i 's/VERSION/$(VERSION)/' $(PKG)/debian/diabaig.6
	sed -i 's/ARCH/$(DEB_ARCH)/' $(PKG)/debian/control

	cd $(PKG) && dpkg-buildpackage -b -us -uc -nc
	#pandoc -V geometry:margin=1in -o build/guide.pdf $(PKG_DIR)/debian/diabaig.6
	rm -r $(PKG)
	cd build && rm *.ddeb *.buildinfo *.changes
