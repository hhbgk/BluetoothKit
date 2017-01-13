package com.demuxer;

/**
 * Created by bob on 17-1-13.
 */

public class PayloadInfo {
    private byte key;//1byte
    private int reserve;//7bits
    private int valueLength;//9bits

    private byte[] value;//N bytes

    public byte getKey() {
        return key;
    }

    public void setKey(byte key) {
        this.key = key;
    }

    public int getReserve() {
        return reserve;
    }

    public void setReserve(int reserve) {
        this.reserve = reserve;
    }

    public int getValueLength() {
        return valueLength;
    }

    public void setValueLength(int valueLength) {
        this.valueLength = valueLength;
    }

    public byte[] getValue() {
        return value;
    }

    public void setValue(byte[] value) {
        this.value = value;
    }
}
