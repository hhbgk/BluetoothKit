package com.demuxer;

import android.util.SparseArray;

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
       /* byte[] data = nativeWrap();
        for (int i = 0; i < data.length; i++){
            Log.e(tag, String.format(Locale.US, "0x%x", data[i]));
        }*/
        PayloadInfo payloadInfo = new PayloadInfo();
        SparseArray<byte[]> sparseArray = new SparseArray<>();
        byte[] bytes = new byte[1];
        bytes[0] = 1;
        sparseArray.put(0x10, bytes);
        bytes[0] = 3;
        sparseArray.put(0x11, bytes);
        bytes[0] = 5;
        sparseArray.put(0x12, bytes);
        payloadInfo.setValue(sparseArray);
        nativeSetData(payloadInfo);
    }

    public byte[] wrapData(PayloadInfo payloadInfo, int version){
        if (payloadInfo == null){
            throw new NullPointerException("payloadInfo can not be null");
        }

        return nativeWrap(payloadInfo.getCommandId(), payloadInfo.getVersion(), payloadInfo.getPayload());
    }

    public byte[] wrapData(PayloadInfo payloadInfo){
        return wrapData(payloadInfo, 0);
    }
    static {
        System.loadLibrary("bt_band");
    }

    private native void nativeInit();
    //private native void nativeGetCrc16();
    private native byte[] nativeWrap(int cmdId, int version, byte[] payload);
    private native void nativeSetData(PayloadInfo payloadInfo);
}
