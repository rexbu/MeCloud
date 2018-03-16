/**
 * file :	McDevice.cpp
 * author :	Rex
 * create :	2016-11-25 16:26
 * func : 
 * history:
 */
#include <jni.h>
#include <iostream>
#include <string>
#include "bs.h"
#include "McDevice.h"
#include "McFile.h"

using namespace std;

static string g_device_id;
static string g_phone;
static string g_sim;
static string g_packagename;
static string g_device;
static string g_system;
static string g_manufacturer;

void mc_set_device(JNIEnv* env, jobject context){
	jclass deviceClass = env->FindClass("com/rex/utils/DeviceUtil");
	const char* s = NULL;

	jmethodID method = env->GetStaticMethodID(deviceClass, "getDeviceId", "(Landroid/content/Context;)Ljava/lang/String;");
	jstring deviceid = (jstring)env->CallStaticObjectMethod(deviceClass, method, context);
	if (deviceid!=NULL)
	{
		s = env->GetStringUTFChars(deviceid, NULL);
		debug_log("DeviceId: %s", s);
		g_device_id = s;
		env->ReleaseStringUTFChars(deviceid, s);	
	}

	method = env->GetStaticMethodID(deviceClass, "getLine1Number", "(Landroid/content/Context;)Ljava/lang/String;");
	deviceid = (jstring)env->CallStaticObjectMethod(deviceClass, method, context);
	if (deviceid!=NULL)
	{
		s = env->GetStringUTFChars(deviceid, NULL);
		debug_log("Line1Number: %s", s);
		g_phone = s;
		env->ReleaseStringUTFChars(deviceid, s);	
	}

	method = env->GetStaticMethodID(deviceClass, "getSimSerialNumber", "(Landroid/content/Context;)Ljava/lang/String;");
	deviceid = (jstring)env->CallStaticObjectMethod(deviceClass, method, context);
	if (deviceid!=NULL)
	{
		s = env->GetStringUTFChars(deviceid, NULL);
		debug_log("SimSerialNumber: %s", s);
		g_sim = s;
		env->ReleaseStringUTFChars(deviceid, s);	
	}

	char package[256] = {0};
	FILE* fp = fopen("/proc/self/cmdline", "r");
	 
    fread(package, sizeof(package), 1, fp);
    fclose(fp);
    debug_log("PackageName: %s", package);
    g_packagename = package;
	// method = env->GetStaticMethodID(deviceClass, "getPackageName", "(Landroid/content/Context;)Ljava/lang/String;");
	// deviceid = (jstring)env->CallStaticObjectMethod(deviceClass, method, context);
	// if (deviceid!=NULL)
	// {
	// 	s = env->GetStringUTFChars(deviceid, NULL);
	// 	debug_log("PackageName: %s", s);
	// 	g_package_name = s;
	// 	env->ReleaseStringUTFChars(deviceid, s);	
	// }

	method = env->GetStaticMethodID(deviceClass, "getDeviceModel", "()Ljava/lang/String;");
	deviceid = (jstring)env->CallStaticObjectMethod(deviceClass, method, context);
	if (deviceid!=NULL)
	{
		s = env->GetStringUTFChars(deviceid, NULL);
		debug_log("DeviceModel: %s", s);
		g_device = s;
		env->ReleaseStringUTFChars(deviceid, s);
	}

	method = env->GetStaticMethodID(deviceClass, "getSystemVersion", "()Ljava/lang/String;");
	deviceid = (jstring)env->CallStaticObjectMethod(deviceClass, method, context);
	if (deviceid!=NULL)
	{
		s = env->GetStringUTFChars(deviceid, NULL);
		debug_log("SystemVersion: %s", s);
		g_system = s;
		env->ReleaseStringUTFChars(deviceid, s);	
	}

	method = env->GetStaticMethodID(deviceClass, "getManufacturer", "()Ljava/lang/String;");
	deviceid = (jstring)env->CallStaticObjectMethod(deviceClass, method, context);
	if (deviceid!=NULL)
	{
		s = env->GetStringUTFChars(deviceid, NULL);
		debug_log("Manufacturer: %s", s);
		g_manufacturer = s;
		env->ReleaseStringUTFChars(deviceid, s);	
	}

	env->DeleteLocalRef(deviceClass);
}

const char* mc::device_id()
{   
    return g_device_id.c_str();
}

const char* mc::system_version(){
    return g_system.c_str();
}

const char* mc::device_version()
{
    return g_device.c_str();
}

int mc::device_index(){
    return 0;
}

const char* mc::bundle_id(){
    return NULL;
}

const char* mc::package_name(){
    return g_packagename.c_str();
}

std::map<pid_t, uint32_t> g_pid_inc;
void mc::guid(char objectid[24]){
    char uid[12];
    time_t t = time(0);
    memcpy(uid, &t, 4);
    
    const char* device = mc::device_id();
    uint32_t crc = bs_crc32(0, (void*)device, (uint32_t)strlen(device));
    memcpy(uid+4, &crc, 3);
    
    uint16_t pid = getpid();
    memcpy(uid+7, &pid, 2);
    
    std::map<pid_t, uint32_t>::iterator iter = g_pid_inc.find(pid);
    if (iter == g_pid_inc.end()) {
        memset(uid+9, 0, 3);
        g_pid_inc[pid] = 0;
    }
    else{
        uint32_t inc = iter->second + 1;
        iter->second = inc;
        memcpy(uid+9, &inc, 4);
    }
    
    for (int i=0; i<12; i++) {
        sprintf(objectid+i*2, "%x", uid[i]);
    }
}