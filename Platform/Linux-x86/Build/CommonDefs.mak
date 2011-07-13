# some defaults
ifndef CFG
	CFG = Release
endif

# expand file list
SRC_FILES_LIST = $(wildcard $(SRC_FILES))

# define the intermediate directory
INT_DIR = $(CFG)

# define output directory
OUT_DIR = $(BIN_DIR)/$(CFG)

# full path to output file
OUTPUT_FILE = $(OUT_DIR)/$(OUTPUT_NAME)

# take this file's dir
COMMON_MAK_DIR = $(dir $(lastword $(MAKEFILE_LIST)))

# get the OS type
OSTYPE := $(shell uname -s)

# platform specific args
-include $(COMMON_MAK_DIR)Platform.$(PLATFORM)
