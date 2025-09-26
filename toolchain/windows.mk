CC=x86_64-w64-mingw32-gcc
CCFLAGS += -DWINDOWS
BUILD=build/windows
TARGET=$(BUILD)/diabaig.exe
LDLIBS=-L. -L/usr/x86_64-w64-mingw32/lib -lm -lpdcurses -lwinpthread
MOD=windows
PKG_FILES += /usr/x86_64-w64-mingw32/bin/libiconv-2.dll /usr/x86_64-w64-mingw32/bin/libintl-8.dll /usr/x86_64-w64-mingw32/bin/libncursesw6.dll /usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll /usr/x86_64-w64-mingw32/bin/libpdcurses.dll 	


all:$(TARGET)
	@cp /usr/x86_64-w64-mingw32/bin/libiconv-2.dll 		$(BUILD)/
	@cp /usr/x86_64-w64-mingw32/bin/libintl-8.dll 		$(BUILD)/
	@cp /usr/x86_64-w64-mingw32/bin/libncursesw6.dll 	$(BUILD)/
	@cp /usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll $(BUILD)/
	@cp /usr/x86_64-w64-mingw32/bin/libpdcurses.dll 	$(BUILD)/

package:
	@mkdir $(BUILD)/$(PKG)
	@cp $(BUILD)/*.dll $(BUILD)/$(PKG)
	@cp $(TARGET) $(BUILD)/$(PKG)
	@cd $(BUILD) && zip -r $(PKG).zip $(PKG) && mv $(PKG).zip ..
	@rm -r $(BUILD)/$(PKG)
