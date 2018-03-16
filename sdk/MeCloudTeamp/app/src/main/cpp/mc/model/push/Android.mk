LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS := -D__ANDROID__ -D__MAIN_LOOP__
#APP_PLATFORM := android-8

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/../async/		\
    $(LOCAL_PATH)/../../bs/		\
    $(LOCAL_PATH)/../../basic/

LOCAL_SRC_FILES += 	\
	WebSocket.cpp	\
	Push.cpp

#LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
LOCAL_STATIC_LIBRARIES := \
	libbs				\
	libbasic			\
	libasync

LOCAL_MODULE := push-core

include $(BUILD_STATIC_LIBRARY)
