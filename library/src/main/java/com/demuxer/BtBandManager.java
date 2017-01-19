package com.demuxer;

/**
 * Created by bob on 17-1-16.
 */

public class BtBandManager {
    String tag = getClass().getSimpleName();
    private static BtBandManager instance = null;
    private int mVersion;

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
        payloadInfo.setCommandId(0x06);
//        SparseArray<byte[]> sparseArray = new SparseArray<>();
//        sparseArray.put(0x06, null);
//        payloadInfo.setValue(sparseArray);
        nativeWrapData(payloadInfo, mVersion);
    }

    public byte[] wrapData(PayloadInfo payloadInfo, int version){
        if (payloadInfo == null){
            throw new NullPointerException("payloadInfo can not be null");
        }

//        return nativeWrap(payloadInfo.getCommandId(), payloadInfo.getVersion(), payloadInfo.getPayload());
        return nativeWrapData(payloadInfo, version);
    }

    public byte[] wrapData(PayloadInfo payloadInfo){
        return wrapData(payloadInfo, mVersion);
    }

    private void setVersion(int version){
        mVersion = version;
    }

    public int getVersion(){
        return mVersion;
    }
    static {
        System.loadLibrary("queue");
        System.loadLibrary("bt_band");
    }

    private native void nativeInit();
    //private native void nativeGetCrc16();
    private native byte[] nativeWrap(int cmdId, int version, byte[] payload);
    private native byte[] nativeWrapData(PayloadInfo payloadInfo, int version);
}
