package com.rex.mecloud;


import android.text.TextUtils;

import com.rex.load.NativeLoad;

import static com.rex.utils.SignedUtil.getMethodSigned;
import static com.rex.utils.SignedUtil.jClassName;
import static com.rex.utils.SignedUtil.jLong;
import static com.rex.utils.SignedUtil.jObject;
import static com.rex.utils.SignedUtil.jString;
import static com.rex.utils.SignedUtil.jVoid;

/**
 * username
 * username
 * password
 * mobile：手机号
 * Created by Visionin on 17/7/18.
 */

public class MeUser extends MeObject {

  private static MeUser user;

  public MeUser() {}

  public MeUser(JSONObject object) {
    objectPtr = createUserWithJSONObject(object.objectPtr);
  }

  @Override
  public void loadObjectPtr() {
    objectPtr = createUser();
  }

  public static MeUser current() {
    if (user != null) {
      return user;
    }
    long ptr = currentUser();
    if (ptr == 0) {
      return null;
    } else {
      user = new MeUser();
    }
    user.objectPtr = ptr;
    return user;
  }

  public void logoutUser() {
    logout();
    user = null;
  }

  public String getUserName() {
    return stringValue("username");
  }

  public String getDeviceId() {
    return stringValue("device");
  }

  public String getUserId() {
    return stringValue("_id");
  }

  public static void saveLoginUser(MeUser user) {
    MeUser.user = user;
    saveLoginUser(user.objectPtr);
  }

  public void resetUser() {
    user = null;
  }

  public static String getEncodePassword(String username, String passWord) {
    return MeUser.encodePassword(username, passWord);
  }

  public static String getDeviceID() {
    return MeUser.device();
  }

  public static String getNewUsername() {
    MeObject object = new MeObject();
    return object.objectId();
  }

  private native long createUser();

  private native long createUserWithJSONObject(long jsonObjectPtr);

  private static native void saveLoginUser(long userPtr);

  private static native long currentUser();

  private native void logout();

  private static native String encodePassword(String username, String password);

  private static native String device();

  static {
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeUser.class), "createUser",
            getMethodSigned(jLong));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeUser.class), "createUserWithJSONObject",
            getMethodSigned(jLong, jLong));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeUser.class), "currentUser",
            getMethodSigned(jLong));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeUser.class), "logout",
            getMethodSigned());
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeUser.class), "saveLoginUser",
      getMethodSigned(jLong, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeUser.class), "encodePassword",
            getMethodSigned(jString, jString, jString));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeUser.class), "device",
            getMethodSigned(jString));
  }

  @Override
  protected void finalize() throws Throwable {
  }
}
