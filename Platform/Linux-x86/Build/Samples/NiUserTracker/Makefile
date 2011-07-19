OSTYPE := $(shell uname -s)

BIN_DIR = ../../../Bin

INC_DIRS = ../../../../../Include

SRC_FILES = \
	../../../../../Samples/NiUserTracker/main.cpp \
	../../../../../Samples/NiUserTracker/SceneDrawer.cpp	

EXE_NAME = Sample-NiUserTracker

ifneq "$(GLES)" "1"
ifeq ("$(OSTYPE)","Darwin")
	LDFLAGS += -framework OpenGL -framework GLUT
else
	USED_LIBS += glut
endif
else
	DEFINES += USE_GLES
	USED_LIBS += GLES_CM IMGegl srv_um
	SRC_FILES += ../../../../../Samples/NiUserTracker/opengles.cpp
endif

USED_LIBS += OpenNI

include ../../Common/CommonCppMakefile

