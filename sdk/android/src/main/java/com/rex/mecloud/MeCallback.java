package com.rex.mecloud;

/**
 * Created by Rex on 17/7/19.
 */

public interface MeCallback extends MeCloudCallBack<JSONObject>{
  void done(JSONObject response, MeException error);
}
