# OpenNI Android makefile.
# niLicense
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= ../../../../../Source/Utils/niLicense/niLicense.cpp

LOCAL_C_INCLUDES := 

LOCAL_CFLAGS := -fvisibility=hidden -DXN_FORMATS_EXPORTS 

LOCAL_SHARED_LIBRARIES := libusb OpenNI 

LOCAL_MODULE := niLicense

LOCAL_LDLIBS += -llog

include $(BUILD_EXECUTABLE)
