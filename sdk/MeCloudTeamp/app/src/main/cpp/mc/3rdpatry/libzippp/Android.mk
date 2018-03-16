LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ -g
#APP_PLATFORM := android-19

LOCAL_C_INCLUDES =	\
	$(LOCAL_PATH)/../libzip

LOCAL_SRC_FILES += \
	libzippp.cpp

#LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
LOCAL_MODULE := zippp

include $(BUILD_STATIC_LIBRARY)
