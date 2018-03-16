/**
 * file :	native_object.h
 * author :	bushaofeng
 * create :	2016-08-22 15:35
 * func : 
 * history:
 */

#ifndef	__NATIVE_OBJECT_H_
#define	__NATIVE_OBJECT_H_

#include <jni.h>

namespace android{
	jclass nativeClass(const char* className);
	jobject nativeObject(jclass c);
	jmethodID nativeMethod(jobject object, const char* sig);
};

#endif
