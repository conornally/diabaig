CC=clang
CCFLAGS += -DMACOS -Wno-unused-command-line-argument
BUILD=build/macos
TARGET=$(BUILD)/diabaig
MOD=macos

INSTALL_PATH=${HOME}/.local/bin

ARCH=$(shell uname -m)
ifeq ($(ARCH),x86_64)
	MOD+=.intel
endif

all:$(TARGET)
