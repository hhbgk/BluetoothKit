package com.demuxer;

import android.util.SparseArray;

import java.util.List;

/**
 * Created by bob on 17-1-13.
 */

public class PayloadInfo {
    private int CommandId;//8bit
    private int version;//4bit
    private int headerReserve;//4bit
    private byte[] payload;
    private SparseArray<byte[]> keyValue;
    private List<KeyInfo> keyInfo;

    public int getVersion() {
        return version;
    }

    public void setVersion(int version) {
        this.version = version;
    }

    public int getCommandId() {
        return CommandId;
    }

    public void setCommandId(int commandId) {
        CommandId = commandId;
    }

    int getHeaderReserve() {
        return headerReserve;
    }

    void setHeaderReserve(int headerReserve) {
        this.headerReserve = headerReserve;
    }

    public byte[] getPayload() {
        return payload;
    }

    public void setPayload(byte[] payload) {
        this.payload = payload;
    }

    public SparseArray<byte[]> getValue() {
        return keyValue;
    }

    public void setValue(SparseArray<byte[]> sparseArray) {
        this.keyValue = sparseArray;
//        sparseArray.get;

    }

    public List<KeyInfo> getKeyInfo() {
        return keyInfo;
    }

    public void setKeyInfo(List<KeyInfo> keyInfo) {
        this.keyInfo = keyInfo;
    }
}
