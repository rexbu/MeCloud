/**
 * file :	MeSMS.h
 * author :	Rex
 * create :	2017-04-04 11:47
 * func : 
 * history:
 */

#ifndef	__MESMS_H_
#define	__MESMS_H_

#include "MeCloud.h"
class MeSMS {
public:
    static void sendSMS(const char* phone, MeCallback *callback);
    static const char* getSMSUrl(const char* phone);
};

#endif
