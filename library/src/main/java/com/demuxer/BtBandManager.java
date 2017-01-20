package com.demuxer;

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
       /*
        PayloadInfo payloadInfo = new PayloadInfo();
        payloadInfo.setCommandId(0x06);
        SparseArray<byte[]> sparseArray = new SparseArray<>();
//        sparseArray.put(0x06, new byte[]{5,4,3,2,1});
        sparseArray.put(0x10, null);
        payloadInfo.setValue(sparseArray);
        byte[] data = nativeWrapData(payloadInfo, mVersion);
        for (int i = 0; i < data.length; i++)
            Log.e(tag, String.format(Locale.US, "%02x", data[i]));
        */
    }

    public byte[] wrapData(PayloadInfo payloadInfo, int version){
        if (payloadInfo == null){
            throw new NullPointerException("payloadInfo can not be null");
        }

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

    public boolean release(){
        return nativeRelease();
    }
    static {
        System.loadLibrary("queue");
        System.loadLibrary("bt_band");
    }

    private native void nativeInit();
    private native boolean nativeRelease();
    private native byte[] nativeWrapData(PayloadInfo payloadInfo, int version);
}
