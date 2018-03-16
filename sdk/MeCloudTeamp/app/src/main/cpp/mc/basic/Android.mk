LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ -g
#APP_PLATFORM := android-19

SRCFILES = $(wildcard $(LOCAL_PATH)/*.cpp $(LOCAL_PATH)/*/*.cpp)
SRCS = $(patsubst $(LOCAL_PATH)/%, ./%,$(SRCFILES)) 

LOCAL_C_INCLUDES =	\
	$(LOCAL_PATH)/../3rdparty/cJSON		\
	$(LOCAL_PATH)/../3rdparty/libzip	\
	$(LOCAL_PATH)/../3rdparty/libzippp	\
	$(LOCAL_PATH)/../bs
	
LOCAL_SRC_FILES += \
	$(SRCS)

LOCAL_STATIC_LIBRARIES := \
	libbs		\
	libcjson	\
	libzippp	\
	libzip	

LOCAL_SHARED_LIBRARIES := libnative
LOCAL_LDLIBS    := -llog -landroid -lz -ldl

LOCAL_MODULE := MobileCross

include $(BUILD_SHARED_LIBRARY)
