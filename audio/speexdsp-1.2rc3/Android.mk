LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE:= libspeex
LOCAL_CFLAGS = -DFIXED_POINT -DUSE_KISS_FFT -DEXPORT="" -UHAVE_CONFIG_H
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_SRC_FILES :=testdenoise.c \
libspeex/buffer.c \
libspeex/fftwrap.c \
libspeex/filterbank.c \
libspeex/jitter.c \
libspeex/kiss_fft.c \
libspeex/kiss_fftr.c \
libspeex/mdf.c \
libspeex/preprocess.c \
libspeex/resample.c \
libspeex/scal.c \
libspeex/smallft.c 


LOCAL_MODULE := testdenoise  

LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)