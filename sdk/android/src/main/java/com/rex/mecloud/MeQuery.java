package com.rex.mecloud;

import com.rex.load.NativeLoad;

import static com.rex.utils.SignedUtil.getMethodSigned;
import static com.rex.utils.SignedUtil.jClassName;
import static com.rex.utils.SignedUtil.jInt;
import static com.rex.utils.SignedUtil.jLong;
import static com.rex.utils.SignedUtil.jObject;
import static com.rex.utils.SignedUtil.jString;
import static com.rex.utils.SignedUtil.jStrings;
import static com.rex.utils.SignedUtil.jVoid;

/**
 * @date 17/7/20 2:53 AM
 * @author Rex
 */

public class MeQuery extends JSONObject {

  public MeQuery(String className) {
    objectPtr = createMeQueryWithClassName(className);
  }

  public MeQuery() {}

  public void loadObjectPtr() {
    objectPtr = createMeQuery();
  }

  public <T> void whereEqualTo(String key, T value) {
    if (value instanceof String) {
      whereEqualToString(key, (String) value);
    } else if (value instanceof Integer) {
      whereEqualToInt(key, (Integer) value);
    }
  }

  public <T> void whereNotEqualTo(String key, T value) {
    if (value instanceof String) {
      whereNotEqualToString(key, (String) value);
    } else if (value instanceof Integer) {
      whereNotEqualToInt(key, (Integer) value);
    }
  }

  public native long createMeQuery();

  public native long createMeQueryWithClassName(String className);

  public native void whereEqualToString(String key, String val);

  public native void whereNotEqualToString(String key, String val);

  public native void whereEqualToInt(String key, int value);

  public native void whereNotEqualToInt(String key, int value);

  public native void whereEqualOr(String key, String value);


  public native void whereEqualOrToInt(String key, int value);

  public native void whereGreaterToString(String key, String value);
  public native void whereGreaterToInt(String key, int value);
  public native void whereLessToString(String key, String value);

  public native void whereLessToInt(String key, int value);

  public native void selectKeys(String keys[], int num);

  public native void addSelectKey(String key);

  public native void addNotSelectKey(String key);

  public native void addAscendSortKeys(String key);

  public native void addDescendSortKeys(String key);

  public native void addLimit(long count);

  public native void addOffset(long location);

  public native void addAggregateObject(MeAggregateObject aggreateObject);

  public native void setAggregateObject(MeAggregateObject aggreateObject);

  static {
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "createMeQuery",
            getMethodSigned(jLong));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "createMeQueryWithClassName",
            getMethodSigned(jString, jLong));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "whereEqualToString",
            getMethodSigned(jString, jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "whereNotEqualToString",
            getMethodSigned(jString, jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "whereEqualToInt",
            getMethodSigned(jString, jInt, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "whereNotEqualToInt",
            getMethodSigned(jString, jInt, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "whereEqualOrToInt",
            getMethodSigned(jString, jInt, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "whereGreaterToString",
            getMethodSigned(jString, jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "whereGreaterToInt",
            getMethodSigned(jString, jInt, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "whereLessToString",
            getMethodSigned(jString, jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "whereLessToInt",
            getMethodSigned(jString, jInt, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "selectKeys",
            getMethodSigned(jStrings, jInt, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "addSelectKey",
            getMethodSigned(jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "addNotSelectKey",
            getMethodSigned(jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "addAscendSortKeys",
            getMethodSigned(jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "addDescendSortKeys",
            getMethodSigned(jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "addLimit",
            getMethodSigned(jLong, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "addOffset",
            getMethodSigned(jLong, jVoid));

    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "addAggregateObject",
            getMethodSigned(jObject(MeAggregateObject.class), jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeQuery.class), "setAggregateObject",
            getMethodSigned(jObject(MeAggregateObject.class), jVoid));
  }
}