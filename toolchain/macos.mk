CC=clang
CCFLAGS += -DMACOS -Wno-unused-command-line-argument
BUILD=build/macos
TARGET=$(BUILD)/diabaig
PKG=diabaig.win

all:$(TARGET)
