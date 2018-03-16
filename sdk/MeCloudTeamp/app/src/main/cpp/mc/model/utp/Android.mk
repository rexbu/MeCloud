LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ 
#APP_PLATFORM := android-8

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/../../bs/		\
    $(LOCAL_PATH)/../../basic/	\
    $(LOCAL_PATH)/../async/

LOCAL_SRC_FILES += 	\
	utp.cpp			

#LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
LOCAL_STATIC_LIBRARIES := \
	libbs		\
	libbasic	\
	libasync

LOCAL_MODULE := utp

include $(BUILD_STATIC_LIBRARY)
