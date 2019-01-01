LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=vg_test.c  
LOCAL_MODULE := vg_test  
LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)
