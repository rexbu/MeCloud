LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ 
#APP_PLATFORM := android-8

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/../push/		\
    $(LOCAL_PATH)/../utp/		\
    $(LOCAL_PATH)/../async/		\
    $(LOCAL_PATH)/../../bs/		\
    $(LOCAL_PATH)/../../basic/

LOCAL_SRC_FILES += 	\
	p2p.cpp

#LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
LOCAL_STATIC_LIBRARIES := \
	libutp				\
	libasync			\
	libbs				

LOCAL_MODULE := p2p

include $(BUILD_STATIC_LIBRARY)
