BIN_DIR = ../../../Bin

INC_DIRS = \
	../../../../../Include \
	../../../../../Source

SRC_FILES = \
	../../../../../Source/Modules/nimCodecs/*.cpp

LIB_NAME = nimCodecs
USED_LIBS = OpenNI

ifneq ($(wildcard /usr/lib/libjpeg.so /usr/lib32/libjpeg.so /usr/lib64/libjpeg.so /usr/local/lib/libjpeg.so /usr/local/lib32/libjpeg.so /usr/local/lib64/libjpeg.so),)
  USED_LIBS += jpeg
else
  INC_DIRS  += ../../../../../Source/External/LibJPEG
  SRC_FILES += ../../../../../Source/External/LibJPEG/*.c
endif
  
include ../../CommonMakefile



