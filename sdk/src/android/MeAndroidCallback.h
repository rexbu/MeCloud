/**
 * file :	MeAndroidCallback.h
 * author :	Rex
 * create :	2017-07-19 21:00
 * func : 
 * history:
 */

#ifndef	__MEANDROIDCALLBACK_H_
#define	__MEANDROIDCALLBACK_H_

#include <jni.h>
#include "bs.h"
#include "MeCallback.h"
#include "MeUser.h"

class MeObjectCallback : public MeCallback {
public:
    MeObjectCallback(const char *classname = NULL, MeObject *object = NULL);

    MeObjectCallback(jobject thiz, jobject callback);

    void setPara(jobject thiz, jobject callback);

    void setIsMeListCallback(bool_t isMeListCallback = BS_FALSE);

    virtual void done(MeObject *obj, MeException *err, uint32_t size = 1);

protected:
    jobject m_thiz;
    jobject m_callback;
    bool_t isMeListCallback;

};

class MeUserCallback : public MeObjectCallback {
public:
    MeUserCallback(MeUser *meUser);

    virtual void done(MeObject *obj, MeException *err, uint32_t size = 1);
};

#endif
