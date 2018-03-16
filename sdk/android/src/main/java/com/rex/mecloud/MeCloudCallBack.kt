package com.rex.mecloud

/**
 * User: chengwangyong(chengwangyong@blinnnk.com)
 * Date: 2017/9/26
 * Time: 下午4:34
 */
interface MeCloudCallBack<in T>{
  fun done(t: T?, err: MeException?)
}