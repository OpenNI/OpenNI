# OpenNI Android makefile.
# libnimMockNodes.so
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS) 

# set path to source
MY_PREFIX := $(LOCAL_PATH)/../../../../../Source/Modules/nimMockNodes/

# list all source files
MY_SRC_FILES := $(MY_PREFIX)*.cpp

# expand the wildcards
MY_SRC_FILE_EXPANDED := $(wildcard $(MY_SRC_FILES))

# make those paths relative to here
LOCAL_SRC_FILES := $(MY_SRC_FILE_EXPANDED:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := 

LOCAL_CFLAGS := -fvisibility=hidden

LOCAL_SHARED_LIBRARIES := OpenNI

LOCAL_MODULE := nimMockNodes

include $(BUILD_SHARED_LIBRARY)

