package com.demuxer;

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
    }
    static {
        System.loadLibrary("bt_band");
    }

    private native void nativeInit();
    private native void nativeGetCrc16();
}
