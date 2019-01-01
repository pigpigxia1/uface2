LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=main.c uart.c \
				CRC.c \
				bt.c
LOCAL_MODULE := bt  
LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)
