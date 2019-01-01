LOCAL_PATH:= $(call my-dir)

#$(info my-dir=  $(call my-dir) )
#include $(all-subdir-makefiles)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=MAIN.c \
	uart.c \
	gpio.c \
	CARD_EMULATE_API.c \
	NATG_API.c \
	FM175XX.c  \
	MIFARE.c   \
	READER_API.c
	


LOCAL_MODULE := NFC  

LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)