package com.rex.mecloud;

import android.app.Application;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;

import com.rex.load.NativeLoad;
import com.rex.utils.DeviceUtil;
import com.rex.utils.SignedUtil;

import java.io.BufferedInputStream;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;

import static com.rex.utils.SignedUtil.getMethodSigned;
import static com.rex.utils.SignedUtil.jBoolen;
import static com.rex.utils.SignedUtil.jBytes;
import static com.rex.utils.SignedUtil.jClassName;
import static com.rex.utils.SignedUtil.jInt;
import static com.rex.utils.SignedUtil.jLong;
import static com.rex.utils.SignedUtil.jObject;
import static com.rex.utils.SignedUtil.jString;
import static com.rex.utils.SignedUtil.jVoid;

/**
 * Created by Visionin on 17/7/18.
 */

public class MeCloud extends Handler {
  protected static volatile MeCloud mInstance = null;

  /**
   * 单例
   *
   * @return
   */
  public static MeCloud shareInstance() {
    if (mInstance == null) {
      synchronized (MeCloud.class) {
        if (mInstance == null) {
          mInstance = new MeCloud();
        }
      }
    }
    return mInstance;
  }

  private MeCloud() {
    super(Looper.getMainLooper());
    config();
  }

  /**
   * app 第一次启动的时候 请调用这个方法
   * @param context application 的 context 严禁传Activity的 Context
   */
  public void createDeviceId(Application context) {
    setDevice(context);
  }

  public Bitmap decryptBitmap(byte[] bytes) {
    byte[] crypto = decrypt(bytes);
    return BitmapFactory.decodeByteArray(crypto, 0, crypto.length);
  }

  public InputStream decryptBitmapToStream(byte[] bytes) {
    byte[] decrypts = decrypt(bytes);
    return new ByteArrayInputStream(decrypts);
  }

  public byte[] Bitmap2Bytes(Bitmap bm) {
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    bm.compress(Bitmap.CompressFormat.PNG, 100, baos);
    return baos.toByteArray();
  }

  public byte[] file2Bytes(File file) {
    int size = (int) file.length();
    byte[] bytes = new byte[size];
    try {
      BufferedInputStream buf = new BufferedInputStream(new FileInputStream(file));
      buf.read(bytes, 0, bytes.length);
      buf.close();
    } catch (FileNotFoundException e) {
      e.printStackTrace();
    } catch (IOException e) {
      e.printStackTrace();
    }
    return bytes;
  }

  public void storeJSONToCache(JSONObject json, String key) {
    saveJSONToCache(json.objectPtr, key);
  }

  public JSONObject readJSONFromCache(String key) {
    JSONObject object = new JSONObject(getJSONFromCache(key));
    return object;
  }

  public void deleteJSONFromCache(String key) {
    removeJSONFromCache(key);
  }

  public void callback(long response, final MeCallback callback, final MeException err) {
    if (err == null) {
      final JSONObject obj = new JSONObject(response);
      post(new Runnable() {
        @Override
        public void run() {
          callback.done(obj, err);
        }
      });
    } else {
      post(new Runnable() {
        @Override
        public void run() {
          callback.done(null, err);
        }
      });
    }
  }

  public void callbackList(final long[] response, final MeListCallback callback,
                           final MeException err) {
    if (err == null) {
      final JSONObject[] objs = new JSONObject[response.length];
      for (int i = 0; i < objs.length; i++) {
        objs[i] = new JSONObject(response[i]);
      }
      post(new Runnable() {
        @Override
        public void run() {
          callback.done(objs, err);
        }
      });
    } else {
      post(new Runnable() {
        @Override
        public void run() {
          callback.done(null, err);
        }
      });
    }
  }

  public void doneCallback(final long response, final HttpDataCallback callback,
                           final MeException err) {
    if (err == null) {
      final JSONObject obj = new JSONObject(response);
      post(new Runnable() {
        @Override
        public void run() {
          callback.done(obj, err);
        }
      });
    } else {
      post(new Runnable() {
        @Override
        public void run() {
          callback.done(null, err);
        }
      });
    }
  }

  public void progressCallback(final HttpDataCallback callback, final long written,
                               final long totleWriten, final long totalExpectWrite) {
    MeCloud.shareInstance().post(new Runnable() {
      @Override
      public void run() {
        callback.progress(written, totleWriten, totalExpectWrite);
      }
    });
  }

  public void doneCallback(final long response, final HttpFileCallback callback,
                           final MeException err) {
    if (err == null) {
      final JSONObject obj = new JSONObject(response);
      post(new Runnable() {
        @Override
        public void run() {
          callback.done(obj, err);
        }
      });
    } else {
      post(new Runnable() {
        @Override
        public void run() {
          callback.done(null, err);
        }
      });
    }
  }

  public void progressCallback(final HttpFileCallback callback, final long written,
                               final long totleWriten, final long totalExpectWrite) {
    MeCloud.shareInstance().post(new Runnable() {
      @Override
      public void run() {
        callback.progress(written, totleWriten, totalExpectWrite);
      }
    });
  }

  public native void config();

  public native void setBaseUrl(String baseUrl);

  public native String cookie();

  public native void setTimeout(int time);

  public native void addHttpHeader(String key, String value);

  public native void setShowLog(boolean showLog);

  public native byte[] crypto(byte[] bitmap);

  public native byte[] decrypt(byte[] bitmap);

  private native void setDevice(Context context);

  public native void saveJSONToCache(long json, String key);

  private native long getJSONFromCache(String key);

  private native void removeJSONFromCache(String key);

  // 网络请求这块
  public native void login(String username, String password, MeCallback callback);

  public native void signUp(String username, String password, MeCallback callback);

  public native void changePassword(String username, String newPassword, MeCallback callback);

  public native void save(MeObject object, MeCallback callback);

  public native void getObjectWithID(String objectId, String className, MeCallback callback);

  public native void getObjectsWithQuery(MeQuery query, MeListCallback callback);

  public native void findWithJoin(MeJoinQuery joinQuery, MeListCallback callback);

  public native void deleteObject(MeObject object, MeCallback callback);

  public native String download(String filename, String url, HttpFileCallback callback);

  public native void uploadFile(String type, String path, HttpFileCallback callback);

  public native void uploadData(String type, byte[] data, HttpFileCallback callback);

  public native void postData(String url, byte[] data, HttpDataCallback callback);

  public native void saveWithUrl(String url, JSONObject object, MeCallback callback);

  public native void getWithUrl(String url, JSONObject object, MeCallback callback);

  static {
    long so = NativeLoad.INSTANCE.loadSo("libMeCloud.so");
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "config",
            getMethodSigned(jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "setBaseUrl",
            getMethodSigned(jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "cookie",
            getMethodSigned(jString));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "setTimeout",
            getMethodSigned(jInt, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "addHttpHeader",
            getMethodSigned(jString, jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "setShowLog",
            getMethodSigned(jBoolen, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "setDevice",
            getMethodSigned(jObject(Context.class), jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "crypto",
            getMethodSigned(jBytes, jBytes));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "decrypt",
            getMethodSigned(jBytes, jBytes));

    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "login",
            getMethodSigned(jString, jString,jObject(MeCallback.class), jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "signUp",
            getMethodSigned(jString, jString, jObject(MeCallback.class), jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "changePassword",
            getMethodSigned(jString, jString, jObject(MeCallback.class), jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "save",
            getMethodSigned(jObject(MeObject.class), jObject(MeCallback.class), jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "getObjectWithID",
            getMethodSigned(jString, jString, jObject(MeCallback.class), jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "getObjectsWithQuery",
            getMethodSigned(jObject(MeQuery.class), jObject(MeListCallback.class), jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "findWithJoin",
            getMethodSigned(jObject(MeJoinQuery.class), jObject(MeListCallback.class), jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "deleteObject",
            getMethodSigned(jObject(MeObject.class), jObject(MeCallback.class), jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "download",
            getMethodSigned(jString, jString, jObject(HttpFileCallback.class), jString));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "uploadFile",
            getMethodSigned(jString, jString, jObject(HttpFileCallback.class), jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "uploadData",
            getMethodSigned(jString, jBytes, jObject(HttpFileCallback.class), jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "postData",
            getMethodSigned(jString, jBytes, jObject(HttpDataCallback.class), jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "saveWithUrl",
            getMethodSigned(jString, jObject(JSONObject.class), jObject(MeCallback.class), jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "getWithUrl",
            getMethodSigned(jString, jObject(JSONObject.class), jObject(MeCallback.class), jVoid));

    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "saveJSONToCache",
            getMethodSigned(jLong, jString, jVoid));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "getJSONFromCache",
            getMethodSigned(jString, jLong));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeCloud.class), "removeJSONFromCache",
            getMethodSigned(jString, jVoid));
  }
}
