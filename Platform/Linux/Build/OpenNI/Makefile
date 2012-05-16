OSTYPE := $(shell uname -s)

BIN_DIR = ../../Bin

INC_DIRS = \
	../../../../Include \
	../../../../Source \
	../../../../Externals/TinyXml

SRC_FILES = \
	../../../../Source/OpenNI/*.cpp \
	../../../../Source/OpenNI/Linux/*.cpp \
	../../../../Externals/TinyXml/*.cpp

ifeq ("$(OSTYPE)","Darwin")
	INC_DIRS += /opt/local/include
	LIB_DIRS += /opt/local/lib
	LDFLAGS += -framework CoreFoundation -framework IOKit
endif

LIB_NAME = OpenNI
USED_LIBS = usb-1.0 dl pthread
ifneq ("$(OSTYPE)","Darwin")
	USED_LIBS += rt 
endif
DEFINES = XN_EXPORTS

include ../Common/CommonCppMakefile


