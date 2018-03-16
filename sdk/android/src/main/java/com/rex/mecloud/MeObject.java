package com.rex.mecloud;

import android.util.Log;

import com.rex.load.NativeLoad;

import static com.rex.utils.SignedUtil.getMethodSigned;
import static com.rex.utils.SignedUtil.jBoolen;
import static com.rex.utils.SignedUtil.jClassName;
import static com.rex.utils.SignedUtil.jDouble;
import static com.rex.utils.SignedUtil.jFloat;
import static com.rex.utils.SignedUtil.jInt;
import static com.rex.utils.SignedUtil.jLong;
import static com.rex.utils.SignedUtil.jObject;
import static com.rex.utils.SignedUtil.jString;
import static com.rex.utils.SignedUtil.jVoid;

/**
 * Created by Visionin on 17/7/18.
 */

public class MeObject extends JSONObject {

  public MeObject() {}

  public MeObject(String className) {
    objectPtr = createMeObjectWithClassName(className);
  }

  public MeObject(String className, long meObjectPre) {
    objectPtr = createMeObjectWithJSONObject(className, meObjectPre);
  }

  public MeObject(String objectId, String className) {
    objectPtr = createMeObjectWithObjectId(objectId, className);
  }

  @Override
  public void loadObjectPtr() {
    this.objectPtr = createMeObject();
  }

  // JNI 对接方法
  // 公有native api
  public native String objectId();

  public native String className();

  public native void setClassName(String className);

  // 私有native api
  private native long createMeObject();

  private native long createMeObjectWithClassName(String className);

  private native long createMeObjectWithJSONObject(String className, long jsonObject);

  private native long createMeObjectWithObjectId(String objectId, String className);

  // 权限操作
  private native void setACL(long meACLObjectPre);

  private native long getACL();

  static {

    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeObject.class), "createMeObject", getMethodSigned(jLong));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeObject.class), "createMeObjectWithClassName", getMethodSigned(jString, jLong));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeObject.class), "createMeObjectWithJSONObject", getMethodSigned(jString, jLong, jLong));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeObject.class), "createMeObjectWithObjectId", getMethodSigned(jString, jString, jLong));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeObject.class), "objectId", getMethodSigned(jString));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeObject.class), "className", getMethodSigned(jString));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeObject.class), "setClassName", getMethodSigned(jString, jVoid));

    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeObject.class), "setACL", getMethodSigned(jLong, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeObject.class), "getACL", getMethodSigned(jLong));
  }
}
