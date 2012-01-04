OSTYPE := $(shell uname -s)

BIN_DIR = ../../../Bin

INC_DIRS = ../../../../../Include

SRC_FILES = ../../../../../Samples/NiSimpleViewer/*.cpp

EXE_NAME = Sample-NiSimpleViewer

ifeq ("$(OSTYPE)","Darwin")
	LDFLAGS += -framework OpenGL -framework GLUT
else
	USED_LIBS += glut
endif

USED_LIBS += OpenNI

include ../../Common/CommonCppMakefile

