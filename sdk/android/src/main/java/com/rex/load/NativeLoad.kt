package com.rex.load

/**
 * Created by Visionin on 16/8/12.
 */
object NativeLoad {
  init {
    System.loadLibrary("native")
  }

  var so: Long = 0

  external fun loadSo(so: String): Long

  external fun registerJNIMethod(so: Long, className: String, funcName: String,
                                 signature: String): Int

  /*
 * 类型 相应的签名
 * boolean Z
 * byte B
 * char C
 * short S
 * int I
 * long J
 * float F
 * double D
 * void V
 * object L用/分隔包的完整类名： Ljava/lang/String;
 * Array [签名 [I [Ljava/lang/Object;
 * Method (参数1类型签名 参数2类型签名···)返回值类型签名
 * 复制代码
 * 特别注意：Object后面一定有分号（；）结束的,多个对象参数中间也用分号(;)来分隔
 * 例子：
 * 方法签名
 * void f1() ()V
 * int f2(int, long) (IJ)I
 * boolean f3(int[]) ([I)B
 * double f4(String, int) (Ljava/lang/String;I)D
 * void f5(int, String [], char) (I[Ljava/lang/String;C)V
 */

}
