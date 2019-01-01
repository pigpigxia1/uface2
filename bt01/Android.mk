LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=main.c
LOCAL_MODULE := bt1  
LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)
