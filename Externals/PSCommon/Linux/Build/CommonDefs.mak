ifndef _COMMON_DEFS_MAKE_
_COMMON_DEFS_MAKE_=1

# some defaults
ifndef CFG
    CFG = Release
endif

# find out the platform on which we're running
MACHINE = $(shell uname -m)
ifneq (,$(findstring x86_64,$(MACHINE)))
	HOST_PLATFORM = x64
else ifneq (,$(findstring x86,$(MACHINE)))
	HOST_PLATFORM = x86
else ifneq (,$(findstring i686,$(MACHINE)))
	HOST_PLATFORM = x86
else ifneq (,$(findstring i386,$(MACHINE)))
	HOST_PLATFORM = x86
else ifneq (,$(findstring arm,$(MACHINE)))
	HOST_PLATFORM = Arm
else
	DUMMY:=$(error Can't determine host platform)
endif

# now check if this is a cross-compilation or not
ifeq "$(PLATFORM)" ""
	PLATFORM = $(HOST_PLATFORM)
else
	ifneq "$(PLATFORM)" "$(HOST_PLATFORM)"
		# cross compiling. Take CXX and STAGING_DIR from environment
		PLATFORM_UPPER = $(shell echo $(PLATFORM) | tr 'a-z' 'A-Z')
		DUMMY:=$(eval CXX = $($(PLATFORM_UPPER)_CXX))
		DUMMY:=$(eval TARGET_SYS_ROOT = $($(PLATFORM_UPPER)_STAGING))
		
		ifeq "$(and $(CXX), $(TARGET_SYS_ROOT))" ""
			DUMMY:=$(error Cross-Compilation error. Can't find $(PLATFORM_UPPER)_CXX and $(PLATFORM_UPPER)_STAGING)
		endif
	endif
endif

# expand file list
SRC_FILES_LIST = $(wildcard $(SRC_FILES))

# define the intermediate directory
INT_DIR = $(PLATFORM)-$(CFG)

# define output directory
OUT_DIR = $(BIN_DIR)/$(PLATFORM)-$(CFG)

# full path to output file
OUTPUT_FILE = $(OUT_DIR)/$(OUTPUT_NAME)

# take this file's dir
COMMON_MAK_DIR = $(dir $(lastword $(MAKEFILE_LIST)))

# get the OS type
OSTYPE := $(shell uname -s)

# platform specific args
include $(COMMON_MAK_DIR)Platform.$(PLATFORM)

endif # _COMMON_DEFS_MAKE_