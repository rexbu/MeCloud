package com.rex.mecloud;

import static com.rex.utils.SignedUtil.getMethodSigned;
import static com.rex.utils.SignedUtil.jClassName;
import static com.rex.utils.SignedUtil.jLong;
import static com.rex.utils.SignedUtil.jString;
import static com.rex.utils.SignedUtil.jVoid;

import com.rex.load.NativeLoad;


/**
 * Created by Visionin on 17/7/18.
 */

public class MeRole extends MeObject{
  public MeRole(String roleName) {
    this.objectPtr = createMeRole(roleName);
    }

  public <T> void setUser(T t) {
    if (t instanceof String) {
      setUserId((String) t);
    } else if (t instanceof Long) {
      setUserObject((Long) t);
    }
    }

  public native long createMeRole(String roleName);

  public native void setUserId(String userId);

  public native void setUserObject(long meUserObjectPre);

    static {
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeRole.class), "createMeRole", getMethodSigned(jString, jLong));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeRole.class), "setUserId", getMethodSigned(jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeRole.class), "setUserObject", getMethodSigned(jLong, jVoid));
    }
}
