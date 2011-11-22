OSTYPE := $(shell uname -s)

BIN_DIR = ../../../Bin

INC_DIRS = \
	../../../../../Include \
	../../../../../Samples/NiHandTracker

SRC_FILES = ../../../../../Samples/NiHandTracker/*.cpp

ifeq ("$(OSTYPE)","Darwin")
	LDFLAGS += -framework OpenGL -framework GLUT
else
	USED_LIBS += glut
endif

USED_LIBS += OpenNI

EXE_NAME = Sample-NiHandTracker

include ../../Common/CommonCppMakefile

