include ../../Common/CommonDefs.mak

BIN_DIR = ../../../Bin

INC_DIRS = ../../../../../Include

SRC_FILES = ../../../../../Samples/NiSimpleViewer/*.cpp

EXE_NAME = Sample-NiSimpleViewer

ifeq ("$(OSTYPE)","Darwin")
	LDFLAGS += -framework OpenGL -framework GLUT
else
	USED_LIBS += glut GL
endif

USED_LIBS += OpenNI

include ../../Common/CommonCppMakefile

