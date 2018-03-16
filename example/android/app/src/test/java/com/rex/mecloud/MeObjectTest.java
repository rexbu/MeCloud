package com.rex.mecloud;

import android.content.Intent;

import org.junit.Test;

import static org.junit.Assert.*;

/**
 * Project Name:android
 * Author:CoorChice
 * Date:2017/7/21
 * Notes:
 */
public class MeObjectTest {
  @Test
  public void put() throws Exception {
    int value = 100;
    put("test", value);
    put("test", 0.00000009);
  }


  public  <T> void put(String key, T value){
    if (value instanceof Integer){
      System.out.println(((Integer) value).intValue());
    } else if (value instanceof Double){
      System.out.println(Double.class.getName());
    }
  }

}