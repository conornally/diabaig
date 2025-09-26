CC=x86_64-w64-mingw32-gcc
CCFLAGS += -DWINDOWS
BUILD=build/windows
TARGET=$(BUILD)/diabaig.exe
LDLIBS=-L. -Llib/windows -lm -lpdcurses -lwinpthread
MOD=windows
PKG_FILES += $(wildcard lib/windows/*.dll)


all:$(TARGET)
	@cp -v lib/windows/*.dll $(BUILD)/

package:
	@mkdir $(BUILD)/$(PKG)
	@cp $(BUILD)/*.dll $(BUILD)/$(PKG)
	@cp $(TARGET) $(BUILD)/$(PKG)
	@cd $(BUILD) && zip -r $(PKG).zip $(PKG) && mv $(PKG).zip ..
	@rm -r $(BUILD)/$(PKG)
