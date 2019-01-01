LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := webrtc_ns
LOCAL_SRC_FILES := \
        noise_suppression.c \
        ns_core.c \
        fft4g.c 
       
include $(BUILD_SHARED_LIBRARY)