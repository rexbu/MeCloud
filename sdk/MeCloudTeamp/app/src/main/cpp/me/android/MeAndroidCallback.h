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

class MeObjectCallback: public MeCallback{
public:
	MeObjectCallback(jobject thiz, jobject callback);
	
	void setPara(jobject thiz, jobject callback);
	virtual void done(MeObject* obj, MeException* err, uint32_t size = 1);

protected:
	jobject		m_thiz;
	jobject		m_callback;
};

#endif
