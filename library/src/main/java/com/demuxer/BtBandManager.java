package com.demuxer;

import android.util.Log;

import java.util.Locale;

/**
 * Created by bob on 17-1-16.
 */

public class BtBandManager {
    String tag = getClass().getSimpleName();
    private static BtBandManager instance = null;

    public static BtBandManager getInstance(){
        if (instance == null){
            synchronized (BtBandManager.class){
                if (instance == null){
                    instance = new BtBandManager();
                }
            }
        }

        return instance;
    }

    private BtBandManager(){
        nativeInit();
        byte[] data = nativeWrap();
        for (int i = 0; i < data.length; i++){
            Log.e(tag, String.format(Locale.US, "0x%x", data[i]));
        }
    }
    static {
        System.loadLibrary("bt_band");
    }

    private native void nativeInit();
    //private native void nativeGetCrc16();
    private native byte[] nativeWrap();
}
