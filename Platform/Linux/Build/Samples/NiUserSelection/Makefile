include ../../Common/CommonDefs.mak

BIN_DIR = ../../../Bin

INC_DIRS = ../../../../../Include

SRC_FILES = ../../../../../Samples/NiUserSelection/*.cpp

EXE_NAME = Sample-NiUserSelection

ifeq "$(GLUT_SUPPORTED)" "1"
	ifeq ("$(OSTYPE)","Darwin")
		LDFLAGS += -framework OpenGL -framework GLUT
	else
		USED_LIBS += glut GL
	endif
else
	ifeq "$(GLES_SUPPORTED)" "1"
		DEFINES += USE_GLES
		USED_LIBS += GLES_CM IMGegl srv_um
	else
		DUMMY:=$(error No GLUT or GLES!)
	endif
endif

USED_LIBS += OpenNI

include ../../Common/CommonCppMakefile

