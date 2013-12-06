# OpenNI Samples Android makefile.
# Sample-SimpleRead
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= ../../../../../Samples/NiSimpleRead/NiSimpleRead.cpp

LOCAL_C_INCLUDES := 

LOCAL_CFLAGS:= -fvisibility=hidden -DXN_EXPORTS

LOCAL_SHARED_LIBRARIES := libc libusb OpenNI

LOCAL_MODULE := Sample-SimpleRead

LOCAL_LDLIBS += -llog

include $(BUILD_EXECUTABLE)
