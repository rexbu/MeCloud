package com.rex.mecloud;

import android.util.Log;

import com.rex.load.NativeLoad;

import java.util.ArrayList;
import java.util.List;

import static com.rex.utils.SignedUtil.getMethodSigned;
import static com.rex.utils.SignedUtil.jBoolen;
import static com.rex.utils.SignedUtil.jClassName;
import static com.rex.utils.SignedUtil.jDouble;
import static com.rex.utils.SignedUtil.jFloat;
import static com.rex.utils.SignedUtil.jInt;
import static com.rex.utils.SignedUtil.jInts;
import static com.rex.utils.SignedUtil.jLong;
import static com.rex.utils.SignedUtil.jLongs;
import static com.rex.utils.SignedUtil.jString;
import static com.rex.utils.SignedUtil.jStrings;
import static com.rex.utils.SignedUtil.jVoid;

/**
 * Project Name:android
 * Author:CoorChice
 * Date:2017/7/21
 * Notes:
 */

public class JSONObject {

  public static long so = 0;
  public long objectPtr = 0;

  public JSONObject() {
    loadObjectPtr();
  }

  public JSONObject(String  text) {
    objectPtr = createJSONObjectWithText(text);
  }

  // 业务层可以不用这个方法
  public JSONObject(long objectPtr) {
    this.objectPtr = objectPtr;
    setObjectPtr("objectPtr", jLong);
  }

  public void loadObjectPtr() {
    objectPtr = createJSONObject();
  }

  public <T> void put(String key, T value) {
    if (value instanceof String) {
      jputString(key, (String) value);
    } else if (value instanceof Double) {
      jputDouble(key, (Double) value);
    } else if (value instanceof Integer) {
      jputInt(key, (Integer) value);
    } else if (value instanceof Long) {
      jputLong(key, (Long) value);
    } else if (value instanceof Float) {
      jputFloat(key, (Float) value);
    } else if (value instanceof Boolean) {
      jputBoolean(key, (Boolean) value);
    } else if (value instanceof MeObject) {
      jputObject(key, ((MeObject) value).objectPtr);
    }
  }

  public JSONObject jsonObject(String key) {
    return new JSONObject(jsonValue(key));
  }

  public List<JSONObject> arrayObject(String key) {
    List<JSONObject> jsonArray = new ArrayList<>();
    for (long ptr : arrayValue(key)) {
      JSONObject jsonObject = new JSONObject(ptr);
      jsonArray.add(jsonObject);
    }
    return jsonArray;
  }

  // JNI对接方法
  // 公有native api
  public native boolean has(String name);

  public native String jsonString();

  public native String stringValue(String key);

  public native double doubleValue(String key);

  public native int intValue(String key);

  public native long longValue(String key);

  public native float floatValue(String key);

  public native boolean booleanValue(String key);

  public native String[] stringArrayValue(String key);

  public native int[] intArrayValue(String key);

  // 私有native api
  private native long createJSONObject();

  private native long createJSONObjectWithText(String text);

  public native void setObjectPtr(String jfieldname, String jsign);

  private native void destroy();

  private native long jsonValue(String key);

  private native long[] arrayValue(String key);

  private native void jputString(String key, String value);

  private native void jputDouble(String key, double value);

  private native void jputInt(String key, int value);

  private native void jputLong(String key, long value);

  private native void jputFloat(String key, float value);

  private native void jputBoolean(String key, boolean value);

  private native void jputObject(String key, long objectPre);

  static {
    so = NativeLoad.INSTANCE.loadSo("libMeCloud.so");

    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "createJSONObject", getMethodSigned(jLong));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "createJSONObjectWithText", getMethodSigned(jString, jLong));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "setObjectPtr", getMethodSigned(jString, jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "destroy", getMethodSigned());
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "has", getMethodSigned(jString, jBoolen));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "jsonString", getMethodSigned(jString));

    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "stringValue", getMethodSigned(jString, jString));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "doubleValue", getMethodSigned(jString, jDouble));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "intValue", getMethodSigned(jString, jInt));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "longValue", getMethodSigned(jString, jLong));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "floatValue", getMethodSigned(jString, jFloat));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "booleanValue", getMethodSigned(jString, jBoolen));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "jsonValue", getMethodSigned(jString, jLong));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "arrayValue", getMethodSigned(jString, jLongs));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "intArrayValue",
      getMethodSigned(jString, jInts));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "stringArrayValue",
      getMethodSigned(jString, jStrings));

    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "jputString",
      getMethodSigned(jString, jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "jputDouble",
      getMethodSigned(jString, jDouble, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "jputInt",
      getMethodSigned(jString, jInt, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "jputLong",
      getMethodSigned(jString, jLong, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "jputFloat",
      getMethodSigned(jString, jFloat, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "jputBoolean",
      getMethodSigned(jString, jBoolen, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(JSONObject.class), "jputObject",
      getMethodSigned(jString, jLong, jVoid));


  }

  @Override
  protected void finalize() throws Throwable {
    destroy();
    objectPtr = 0;
    super.finalize();
  }
}
