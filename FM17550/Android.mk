LOCAL_PATH:= $(call my-dir)

#$(info my-dir=  $(call my-dir) )
#include $(all-subdir-makefiles)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=main.c \
	uart.c \
	gpio.c \
	test.c \
	SRC/FM175xx/fm175xx.c \
	SRC/IOS14443/cpu_card.c \
	SRC/IOS14443/mifare_card.c \
	SRC/IOS14443/NTAG.c \
	SRC/IOS14443/type_a.c \
	SRC/IOS14443/type_b.c \
	SRC/IOS14443/ultralight.c \
	SRC/Utility/Utility.c 
	

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/SRC/FM175xx/ \
	$(LOCAL_PATH)/SRC/IOS14443/ \
	$(LOCAL_PATH)/SRC/Utility/ 


LOCAL_MODULE := FM17550  

LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)