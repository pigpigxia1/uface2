LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=rfid.c  
LOCAL_MODULE := rfid  
LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)
