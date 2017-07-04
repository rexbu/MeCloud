LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ -g
#APP_PLATFORM := android-19

SRCFILES = $(wildcard $(LOCAL_PATH)/*.cpp $(LOCAL_PATH)/*/*.cpp)
SRCS = $(patsubst $(LOCAL_PATH)/%, ./%,$(SRCFILES)) 

LOCAL_C_INCLUDES =	\
	$(LOCAL_PATH)/../../mc	\
	$(LOCAL_PATH)/../../mc/basic	\
	$(LOCAL_PATH)/../../mc/bs 		\
	$(LOCAL_PATH)/../../mc/me		\
	$(LOCAL_PATH)/../../third/sszip	\
	$(LOCAL_PATH)/../../third/cjson	\
	$(LOCAL_PATH)/../../third/oepnssl 			\
	$(LOCAL_PATH)/../../third/openssl/include 	\
	$(LOCAL_PATH)/../../third/curl				\
	$(LOCAL_PATH)/../../third/curl/include 		\
	$(LOCAL_PATH)/../../third/curl/include/curl \
	
LOCAL_SRC_FILES += \
	$(SRCS)

LOCAL_MODULE := me

include $(BUILD_STATIC_LIBRARY)
