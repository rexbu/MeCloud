LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS := -D__ANDROID__ -D__MAIN_LOOP__
#APP_PLATFORM := android-8

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/../../bs/		\
    $(LOCAL_PATH)/../../basic/

LOCAL_SRC_FILES += 	\
	AsyncFrame.cpp	\
	AsyncSocket.cpp	\
	AsyncQueue.cpp	\
	SocketFrame.cpp

#LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
LOCAL_STATIC_LIBRARIES := \
	libbs				\
	libbasic

LOCAL_MODULE := async

include $(BUILD_STATIC_LIBRARY)
