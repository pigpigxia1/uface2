LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=audiocap.c
LOCAL_MODULE := audiocap  
LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)
