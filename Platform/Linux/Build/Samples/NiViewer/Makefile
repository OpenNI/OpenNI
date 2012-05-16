include ../../Common/CommonDefs.mak

BIN_DIR = ../../../Bin

INC_DIRS = \
	../../../../../Include \
	../../../../../Samples/NiViewer

SRC_FILES = ../../../../../Samples/NiViewer/*.cpp

ifeq ("$(OSTYPE)","Darwin")
	LDFLAGS += -framework OpenGL -framework GLUT
else
	USED_LIBS += glut GL
endif

USED_LIBS += OpenNI

EXE_NAME = NiViewer

include ../../Common/CommonCppMakefile

