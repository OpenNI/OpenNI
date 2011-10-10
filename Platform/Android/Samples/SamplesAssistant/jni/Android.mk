# OpenNISamplesAssistant.jni Android makefile.
# libOpenNISamplesAssistant.jni.so
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS) 

# set path to source
MY_PREFIX := $(LOCAL_PATH)/
OPENNI_DIR := $(LOCAL_PATH)/../../../../..

# list all source files
MY_SRC_FILES := \
	$(MY_PREFIX)*.cpp

# expand the wildcards
MY_SRC_FILE_EXPANDED := $(wildcard $(MY_SRC_FILES))

# make those paths relative to here
LOCAL_SRC_FILES := $(MY_SRC_FILE_EXPANDED:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := \
	$(MY_PREFIX) \
	$(OPENNI_DIR)/Include/

LOCAL_CFLAGS:= -fvisibility=hidden -DXN_EXPORTS

LOCAL_LDFLAGS += -Wl,--export-dynamic --dynamic-linker

LOCAL_LDLIBS := -llog
LOCAL_LDLIBS += $(OPENNI_DIR)/Platform/Android/libs/armeabi-v7a/libOpenNI.so

#LOCAL_SHARED_LIBRARIES := OpenNI

LOCAL_PREBUILT_LIBS := libc 

LOCAL_MODULE:= OpenNISamplesAssistant.jni

include $(BUILD_SHARED_LIBRARY)
