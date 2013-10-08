# OpenNI Samples Android makefile.
# NiSkeletonBenchmark
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= ../../../../../Samples/NiSkeletonBenchmark/NiSkeletonBenchmark.cpp

LOCAL_C_INCLUDES := 

LOCAL_CFLAGS:= -fvisibility=hidden -DXN_EXPORTS

LOCAL_SHARED_LIBRARIES := libc libusb OpenNI

LOCAL_MODULE := NiSkeletonBenchmark

LOCAL_LDLIBS += -llog

include $(BUILD_EXECUTABLE)
