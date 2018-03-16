package com.rex.mecloud;

import android.util.Log;

import com.rex.load.NativeLoad;

import static com.rex.utils.SignedUtil.getMethodSigned;
import static com.rex.utils.SignedUtil.jClassName;
import static com.rex.utils.SignedUtil.jInt;
import static com.rex.utils.SignedUtil.jLong;
import static com.rex.utils.SignedUtil.jString;

/**
 * Project Name:MeCloudTeamp
 * Author:CoorChice
 * Date:2017/7/25
 * Notes:
 */

public class MeFile extends MeObject {

  public String filePath;

  public MeFile(String className) {
    objectPtr = createMeFile(className);
  }

  public MeFile(String objectId, String className) {
    objectPtr = createMeFileWithObjectId(objectId, className);
  }


  private native long createMeFile(String className);

  private native long createMeFileWithObjectId(String objectId, String className);

  public native String filePath();

  public native String imageUrl(int width, int height);

  public native String imageUrlNormal();

  public native String imageCropUrl(int x, int y, int width, int height);

  static {
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeFile.class), "createMeFile",
      getMethodSigned(jString, jLong));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeFile.class), "createMeFileWithObjectId",
      getMethodSigned(jString, jString, jLong));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeFile.class), "filePath",
      getMethodSigned(jString));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeFile.class), "imageUrl",
      getMethodSigned(jInt, jInt, jString));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeFile.class), "imageUrlNormal",
      getMethodSigned(jString));
    NativeLoad.INSTANCE.registerJNIMethod(so, jClassName(MeFile.class), "imageCropUrl",
      getMethodSigned(jInt, jInt, jInt, jInt, jString));
  }
}
