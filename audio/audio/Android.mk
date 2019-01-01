LOCAL_PATH:= $(call my-dir)



include $(CLEAR_VARS)
LOCAL_CFLAGS = -DFIXED_POINT -DUSE_KISS_FFT -DEXPORT="" -UHAVE_CONFIG_H
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
#LOCAL_C_INCLUDES:= external/tinyalsa/include
LOCAL_SRC_FILES:= audiotest.c \
				  pcm.c \
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
				  
LOCAL_MODULE := audiotest
LOCAL_SHARED_LIBRARIES:= libcutils libutils
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_CFLAGS = -DFIXED_POINT -DUSE_KISS_FFT -DEXPORT="" -UHAVE_CONFIG_H
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SRC_FILES:= server.c \
				  pcm.c \
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
LOCAL_MODULE := server
LOCAL_SHARED_LIBRARIES:= libcutils libutils
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_CFLAGS = -DFIXED_POINT -DUSE_KISS_FFT -DEXPORT="" -UHAVE_CONFIG_H
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SRC_FILES:= client.c \
				  pcm.c \
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
LOCAL_MODULE := client
LOCAL_SHARED_LIBRARIES:= libcutils libutils
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= testclient.c \
				  
LOCAL_MODULE := testclient

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)