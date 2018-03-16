package com.rex.mecloud;

import com.rex.load.NativeLoad;

import static com.rex.utils.SignedUtil.getMethodSigned;
import static com.rex.utils.SignedUtil.jBoolen;
import static com.rex.utils.SignedUtil.jClassName;
import static com.rex.utils.SignedUtil.jInt;
import static com.rex.utils.SignedUtil.jLong;
import static com.rex.utils.SignedUtil.jString;
import static com.rex.utils.SignedUtil.jVoid;

/**
 * User: chengwangyong(chengwangyong@blinnnk.com)
 * Date: 2017/9/21
 * Time: 下午3:43
 */
public class MeAggregateObject extends JSONObject {

  public MeAggregateObject(String className) {
    objectPtr = createMeAggregateObject(className);
  }

  private native long createMeAggregateObject(String className);

  public native void whereEqualToWithAggregate(String key, String value);

  public native void whereEqualToIntWithAggregate(String key, int value);

  public native void whereNotEqualToWithAggregate(String key, String value);

  public native void whereNotEqualToIntWithAggregate(String key, int value);

  public native void whereGreaterWithAggregate(String key, String value);

  public native void whereGreaterIntWithAggregate(String key, int value);

  public native void whereLessWithAggregate(String key, String value);

  public native void whereLessIntWithAggregate(String key, int value);

  public native void setResponseKey(String key);

  public native void setDistinctKey(String key);

  public void setMethod(MeAggregateMethod method) {
    setMethod(method.getValue());
  }

  private native void setMethod(int method);

  static {
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeAggregateObject.class), "createMeAggregateObject",
            getMethodSigned(jString, jLong));

    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeAggregateObject.class), "whereEqualToWithAggregate",
            getMethodSigned(jString, jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeAggregateObject.class), "whereEqualToIntWithAggregate",
            getMethodSigned(jString, jInt, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeAggregateObject.class), "whereNotEqualToWithAggregate",
            getMethodSigned(jString, jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeAggregateObject.class), "whereNotEqualToIntWithAggregate",
            getMethodSigned(jString, jInt, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeAggregateObject.class), "whereGreaterWithAggregate",
            getMethodSigned(jString, jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeAggregateObject.class), "whereGreaterIntWithAggregate",
            getMethodSigned(jString, jInt, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeAggregateObject.class), "whereLessWithAggregate",
            getMethodSigned(jString, jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeAggregateObject.class), "whereLessIntWithAggregate",
            getMethodSigned(jString, jInt, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeAggregateObject.class), "setResponseKey",
            getMethodSigned(jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeAggregateObject.class), "setDistinctKey",
            getMethodSigned(jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeAggregateObject.class), "setMethod",
            getMethodSigned(jInt, jVoid));
  }

  public enum MeAggregateMethod {
    COUNT(1), ID(2), LIST(3), JSONOBJECT(4), DISTINCTCONT(5);

    private int value;

    MeAggregateMethod(int value) {
      this.value = value;
    }

    public int getValue() {
      return value;
    }
  }
}
