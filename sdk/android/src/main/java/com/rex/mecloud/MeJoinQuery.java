package com.rex.mecloud;

import com.rex.load.NativeLoad;
import com.rex.utils.SignedUtil;

/**
 * User: chengwangyong(chengwangyong@blinnnk.com)
 * Date: 2017/9/21
 * Time: 下午2:09
 */
public class MeJoinQuery extends JSONObject {

  protected String className = null;

  public MeJoinQuery(String className) {
    this(className, false);
  }

  public MeJoinQuery(String className, boolean nestQuery) {
    this.className = className;
    objectPtr = createMeJoinQuery(className, nestQuery);
  }

  private native long createMeJoinQuery(String className, boolean nestQuery);


  public native void addSelectKeyWithJoin(String key);

  public native void addNotSelectKeyWithJoin(String key);

  public native void addAscendWithJoin(String key);

  public native void addDescendWithJoin(String key);

  public native void matchEqualTo(String key, String value);

  public native void matchGreater(String key, String value);

  public native void matchLess(String key, String value);

  public native void matchEqualToInt(String key, int value);

  public native void matchGreaterToInt(String key, int value);

  public native void matchLessToInt(String jkey, int value);


  public void addForeignTable(String fromTable, String foreignKey, String localKey) {
    addForeignTable(fromTable, foreignKey, localKey, fromTable);
  }

  public native void addForeignTable(String fromTable, String foreignKey, String localKey,
                                     String document);

  //调用这个方法前 请注意 传入的MeJoinQuery nestQuery为 false 否则无法查询到数据
  public native void addMeJoinQueryPtr(MeJoinQuery joinQuery);

  public native void addLimitWithJoin(int count);

  /**
   * 异步get回调接口， jni中调用
   *
   * @param callback
   */
  public void getCallback(final long response, final MeCallback callback, final MeException err) {
    if (err == null) {
      final JSONObject obj = new JSONObject(response);
      MeCloud.shareInstance().post(new Runnable() {
        @Override
        public void run() {
          callback.done(obj, err);
        }
      });
    } else {
      MeCloud.shareInstance().post(new Runnable() {
        @Override
        public void run() {
          callback.done(null, err);
        }
      });
    }
  }

  public void findCallback(final long[] response, final MeListCallback callback,
                           final MeException err) {
    if (err == null) {
      final JSONObject[] objs = new JSONObject[response.length];
      for (int i = 0; i < objs.length; i++) {
        objs[i] = new JSONObject(response[i]);
      }
      MeCloud.shareInstance().post(new Runnable() {
        @Override
        public void run() {
          callback.done(objs, err);
        }
      });
    } else {
      MeCloud.shareInstance().post(new Runnable() {
        @Override
        public void run() {
          callback.done(null, err);
        }
      });
    }
  }

  static {
    NativeLoad.INSTANCE.registerJNIMethod(JSONObject.so, SignedUtil.jClassName(MeJoinQuery.class), "createMeJoinQuery",
      SignedUtil.getMethodSigned(SignedUtil.jString, SignedUtil.jBoolen, SignedUtil.jLong));
    NativeLoad.INSTANCE.registerJNIMethod(JSONObject.so, SignedUtil.jClassName(MeJoinQuery.class), "addSelectKeyWithJoin",
      SignedUtil.getMethodSigned(SignedUtil.jString, SignedUtil.jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(JSONObject.so, SignedUtil.jClassName(MeJoinQuery.class), "addNotSelectKeyWithJoin",
      SignedUtil.getMethodSigned(SignedUtil.jString, SignedUtil.jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(JSONObject.so, SignedUtil.jClassName(MeJoinQuery.class), "addAscendWithJoin",
      SignedUtil.getMethodSigned(SignedUtil.jString, SignedUtil.jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(JSONObject.so, SignedUtil.jClassName(MeJoinQuery.class), "addDescendWithJoin",
      SignedUtil.getMethodSigned(SignedUtil.jString, SignedUtil.jVoid));

    NativeLoad.INSTANCE.registerJNIMethod(JSONObject.so, SignedUtil.jClassName(MeJoinQuery.class), "matchEqualTo",
      SignedUtil.getMethodSigned(SignedUtil.jString, SignedUtil.jString, SignedUtil.jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(JSONObject.so, SignedUtil.jClassName(MeJoinQuery.class), "matchEqualToInt",
      SignedUtil.getMethodSigned(SignedUtil.jString, SignedUtil.jInt, SignedUtil.jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(JSONObject.so, SignedUtil.jClassName(MeJoinQuery.class), "matchGreater",
      SignedUtil.getMethodSigned(SignedUtil.jString, SignedUtil.jString, SignedUtil.jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(JSONObject.so, SignedUtil.jClassName(MeJoinQuery.class), "matchGreaterToInt",
      SignedUtil.getMethodSigned(SignedUtil.jString, SignedUtil.jInt, SignedUtil.jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(JSONObject.so, SignedUtil.jClassName(MeJoinQuery.class), "matchLess",
      SignedUtil.getMethodSigned(SignedUtil.jString, SignedUtil.jString, SignedUtil.jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(JSONObject.so, SignedUtil.jClassName(MeJoinQuery.class), "matchLessToInt",
      SignedUtil.getMethodSigned(SignedUtil.jString, SignedUtil.jInt, SignedUtil.jVoid));

    NativeLoad.INSTANCE.registerJNIMethod(JSONObject.so, SignedUtil.jClassName(MeJoinQuery.class), "addForeignTable",
      SignedUtil.getMethodSigned(SignedUtil.jString, SignedUtil.jString, SignedUtil.jString, SignedUtil.jString
        , SignedUtil.jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(JSONObject.so, SignedUtil.jClassName(MeJoinQuery.class), "addMeJoinQueryPtr",
      SignedUtil.getMethodSigned(SignedUtil.jObject(MeJoinQuery.class), SignedUtil.jVoid));

    NativeLoad.INSTANCE.registerJNIMethod(JSONObject.so, SignedUtil.jClassName(MeJoinQuery.class), "addLimitWithJoin",
      SignedUtil.getMethodSigned(SignedUtil.jInt, SignedUtil.jVoid));
  }
}
