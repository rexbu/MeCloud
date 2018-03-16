package com.rex.mecloud;

/**
 * Created by Rex on 17/7/19.
 */

public class MeException extends Exception {
    public int      errCode;
    public String   errMsg;
    public String   info;//com.rex.mecloud.MeException

    public MeException(int errCode, String errMsg, String info){
        this.errCode = errCode;
        this.errMsg = errMsg;
        this.info = info;
    }
}
