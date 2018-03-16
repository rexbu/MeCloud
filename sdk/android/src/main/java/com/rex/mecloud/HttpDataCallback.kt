package com.rex.mecloud

/**
 * User: chengwangyong(chengwangyong@blinnnk.com)
 * Date: 2017/9/4
 * Time: 上午10:56
 */
interface HttpDataCallback : MeCloudCallBack<JSONObject?> {
    fun progress(written: Long, totalWritten: Long, totalExpectWrite: Long)
}