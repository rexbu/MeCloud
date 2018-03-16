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

public class MeACL extends JSONObject{


  public MeACL(){
    super();
  }


  public MeACL(long objectPtr){
    super(objectPtr);
  }


  public native void setPublicReadAccess();
  public native void setPublicWriteAccess();
  public native void setRoleReadAccess(String role);
  public native void setRoleWriteAccess(String role);
  public native void setRoleReadAccessMeRole(long meRoleObjectPre);
  public native void setRoleWriteAccessMeRole(long meRoleObjectPre);
  public native void setUserReadAccess(String userId);
  public native void setUserWriteAccess(String userId);
  public native void setUserReadAccessMeUser(long meUserObjectPre);
  public native void setUserWriteAccessMeUser(long meUserObjectPre);

  static {
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeACL.class), "setPublicReadAccess", getMethodSigned());
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeACL.class), "setPublicWriteAccess", getMethodSigned());
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeACL.class), "setRoleReadAccess", getMethodSigned(jString,jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeACL.class), "setRoleWriteAccess", getMethodSigned(jString,jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeACL.class), "setRoleReadAccessMeRole", getMethodSigned(jLong, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeACL.class), "setRoleWriteAccessMeRole", getMethodSigned(jLong, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeACL.class), "setUserReadAccess", getMethodSigned(jString,jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeACL.class), "setUserWriteAccess", getMethodSigned(jString,jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeACL.class), "setUserReadAccessMeUser", getMethodSigned(jLong, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeACL.class), "setUserWriteAccessMeUser", getMethodSigned(jLong, jVoid));

  }

}
