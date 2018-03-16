LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ -g
#APP_PLATFORM := android-19

SRCFILES = $(wildcard $(LOCAL_PATH)/*.cpp $(LOCAL_PATH)/*/*.cpp)
SRCS = $(patsubst $(LOCAL_PATH)/%, ./%,$(SRCFILES)) 

LOCAL_C_INCLUDES =	\
	$(LOCAL_PATH)/../android/jni/mc/3rdparty/libzip		\
	$(LOCAL_PATH)/../android/jni/mc/3rdparty/cJSON		\
	$(LOCAL_PATH)/../android/jni/mc/3rdparty/libzippp 	\
	$(LOCAL_PATH)/../android/jni/mc/bs 			\
	$(LOCAL_PATH)/../android/jni/mc/basic
	
LOCAL_SRC_FILES += \
	$(SRCS)

LOCAL_SHARED_LIBRARIES := libMobileCross libnative

LOCAL_LDLIBS    := -ldl -llog
LOCAL_MODULE    := MeCloud

include $(BUILD_SHARED_LIBRARY)