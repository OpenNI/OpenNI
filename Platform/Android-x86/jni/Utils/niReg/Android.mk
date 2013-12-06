# OpenNI Android makefile.
# niReg
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= ../../../../../Source/Utils/niReg/niReg.cpp

LOCAL_C_INCLUDES := 

LOCAL_CFLAGS := -fvisibility=hidden -DXN_FORMATS_EXPORTS 

LOCAL_SHARED_LIBRARIES := libusb OpenNI 

LOCAL_MODULE := niReg

LOCAL_LDLIBS += -llog

include $(BUILD_EXECUTABLE)
