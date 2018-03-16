LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CPP_EXTENSION := .cpp .cc
LIBZIP_SRC_FILES := $(wildcard $(LOCAL_PATH)/*.c)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/
LOCAL_SRC_FILES := $(LIBZIP_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_MODULE := zip

ifeq ($(TARGET_ARCH_ABI),x86)
    LOCAL_CFLAGS += -ffast-math -mtune=atom -mssse3 -mfpmath=sse
endif

#LOCAL_LDLIBS := -lz

include $(BUILD_STATIC_LIBRARY)
