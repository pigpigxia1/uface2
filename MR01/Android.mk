LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=main.c uart.c card_machine.c card.c
#LOCAL_SRC_FILES:= uart.c
LOCAL_MODULE := card_machine  
LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)
