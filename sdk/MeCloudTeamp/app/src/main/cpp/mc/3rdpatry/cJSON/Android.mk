LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ -g
#APP_PLATFORM := android-19

LOCAL_SRC_FILES += \
	cJSON.c	

#LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
LOCAL_MODULE := cjson

include $(BUILD_STATIC_LIBRARY)
