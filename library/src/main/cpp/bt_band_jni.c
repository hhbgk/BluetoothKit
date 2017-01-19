//
// Created by bob on 17-1-16.
//

#include <stdlib.h>
#include <android/log.h>
#include <jni.h>
#include <assert.h>
#include "debug.h"
#include "bt_packet.h"

#define JNI_CLASS "com/demuxer/BtBandManager"
#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))

static JavaVM *gJVM = NULL;
static jobject gObj = NULL;
static packet_hdr_t g_data = {0};

static void jni_init(JNIEnv *env, jobject thiz)
{
    logi("%s", __func__);
    (*env)->GetJavaVM(env, &gJVM);
    gObj = (*env)->NewGlobalRef(env, thiz);
    jclass clazz = (*env)->GetObjectClass(env, thiz);
    if(clazz == NULL)
    {
        (*env)->ThrowNew(env, "java/lang/NullPointerException", "Unable to find exception class");
    }
}

static jbyteArray jni_bt_wrap_data(JNIEnv *env, jobject thiz, jobject jPayloadInfo, jint version)
{
    logi("%s", __func__);

    int total_size = 8;//L1 header
    uint16_t payload_len = 0x0;
    int sparse_size = 0;//key value size
    jmethodID m_get = NULL;
    jmethodID m_keyAt = NULL;
    int cmd_id;
    uint8_t *payload = NULL;
    packet_hdr_t *packet = &g_data;
    assert(packet != NULL);

    memset(packet, 0, sizeof(packet_hdr_t));
    packet->q_value = queue_create();
    bd_bt_set_magic(packet, 0xab);
    bd_bt_set_err_flag(packet, 0x0);
    bd_bt_set_ack_flag(packet, 0x1);
    bd_bt_set_version(packet, version);

    bd_bt_set_seq_id(packet, 0x1e00);
    jclass cls_payloadInfo = (*env)->GetObjectClass(env, jPayloadInfo);
    //method in class PayloadInfo
    jmethodID m_getValue = (*env)->GetMethodID(env, cls_payloadInfo, "getValue", "()Landroid/util/SparseArray;");
    jmethodID m_getCommandId = (*env)->GetMethodID(env, cls_payloadInfo, "getCommandId", "()I");

    jobject jSparseArray = (jobject) (*env)->CallObjectMethod(env, jPayloadInfo, m_getValue);
    if (jSparseArray != NULL)
    {
        jclass cls_sparseArray = (*env)->FindClass(env, "android/util/SparseArray");
        m_keyAt = (*env)->GetMethodID(env, cls_sparseArray, "keyAt", "(I)I");
        m_get = (*env)->GetMethodID(env, cls_sparseArray, "get", "(I)Ljava/lang/Object;");
        jmethodID m_size = (*env)->GetMethodID(env, cls_sparseArray, "size", "()I");
        sparse_size = (*env)->CallIntMethod(env, jSparseArray, m_size);
        loge("sparse_size===%d", sparse_size);
    }
    else
    {
        logw("key value is empty");
    }

    cmd_id = (*env)->CallIntMethod(env, jPayloadInfo, m_getCommandId);
    if (cmd_id >= 0)
    {
        payload_len += 2;//L2 packet header
        bd_bt_set_cmdId_version(packet, cmd_id, 0x0);
    }

    for (int i = 0; i < sparse_size; ++i)
    {
        uint8_t *pArray = NULL;
        unsigned int value_len = 0;
        int key = (*env)->CallIntMethod(env, jSparseArray, m_keyAt, i);
        logi("key=0x%02x", key);
        payload_len += 1;//key byte
        payload_len += 2;//key header byte
        jobjectArray jobjectArray1 = (*env)->CallObjectMethod(env, jSparseArray, m_get, key);
        if (jobjectArray1 != NULL)
        {
            pArray = (uint8_t *) (*env)->GetByteArrayElements(env, jobjectArray1, NULL);
            value_len = (unsigned int)(*env)->GetArrayLength(env, jobjectArray1);
            payload_len += value_len;//key value byte
            logw("value_len=%u", value_len);
            bd_bt_set_key_value(packet, key, pArray, value_len);

            for (int j = 0; j < value_len; ++j)
            {
                loge("valueArray=%d, j=%d", pArray[j], j);
            }
            (*env)->ReleaseByteArrayElements(env, jobjectArray1, (jbyte *) pArray, NULL);
        }
        else
        {
            bd_bt_set_key_value(packet, key, pArray, value_len);
        }
    }
    total_size += payload_len;

    if (payload_len > 0)
    {
        payload = calloc(payload_len, sizeof(uint8_t));
        if (!payload)
        {
            loge("%s calloc failed", __func__);
            return NULL;
        }
        bd_bt_set_payload_length(packet, payload_len);
    }

    uint8_t *buf = (uint8_t *) packet;
    int q_size = queue_empty(packet->q_value) ? 0 : queue_elements(packet->q_value);

    key_value_t *key_value = NULL;
    for (int i = 0; i < q_size; ++i) {
        queue_get(packet->q_value, (void **) &key_value);
    }

    if (payload && buf)
    {
        if (cmd_id >= 0)
            memcpy(payload, buf+8, 2);

        if (key_value != NULL)
        {
            uint8_t *key_value_buf = (uint8_t *) key_value;
            memcpy(payload+2, key_value_buf, 1+2+key_value->value_len);
            memcpy(buf + 10, key_value_buf, 1+2+key_value->value_len);
        }
        else
        {
            logw("key value is null");
        }

        bd_bt_set_crc16(packet, payload, payload_len);
        logw("crc16=%02x", packet->crc16);

        for (int i = 0; i < payload_len; i++)
        {
            logw("payload i=%d 0x%02x", i, (payload[i]));
        }
    }

    loge("total_size=%02x, payload_len=%d", total_size, payload_len);
    for (int i = 0; i < total_size; i++)
    {
        logi("i=%d 0x%02x", i, (buf[i]));
    }
    jbyteArray jbyteArray1 = (*env)->NewByteArray(env, total_size);
    (*env)->SetByteArrayRegion(env, jbyteArray1, 0, total_size, (const jbyte *) buf);

    return jbyteArray1;
}

static jbyteArray jni_bt_wrap(JNIEnv *env, jobject thiz, jint cmdId, jint version, jbyteArray jarray)
{
    logi("%s", __func__);
    int size = 0;
    uint8_t *cArray = NULL;
    int total_size = 8;//L1 header
    if (cmdId > 0)
    {
        total_size += 2;//L2 packet header
    }

    if (jarray != NULL)
    {
        size = (*env)->GetArrayLength(env, jarray);
        total_size += size;
        cArray = (uint8_t *) (*env)->GetByteArrayElements(env, jarray, NULL);
        (*env)->ReleaseByteArrayElements(env, jarray, (jbyte *) cArray, NULL);
    }

    packet_hdr_t *data = bd_bt_packet_wrap(cmdId, version, size, cArray);

    uint8_t *buf = (uint8_t *) data;
    loge("total_size=%02x, size=%d", total_size, size);
    jbyteArray jbyteArray1 = (*env)->NewByteArray(env, total_size);
    (*env)->SetByteArrayRegion(env, jbyteArray1, 0, total_size, (const jbyte *) buf);

    return jbyteArray1;
}

static JNINativeMethod g_methods[] =
{
        {"nativeInit", "()V", (void*) jni_init},
        {"nativeWrap", "(II[B)[B", (void*) jni_bt_wrap},
        {"nativeWrapData", "(Lcom/demuxer/PayloadInfo;I)[B", (void*) jni_bt_wrap_data},
 //       {"nativeGetCrc16", "(S[BI)C", (void*) jni_getCrc16},
};

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv *env = NULL;
    if ((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_6) != JNI_OK){
        return JNI_ERR;
    }
    assert(env != NULL);

    jclass klass = (*env)->FindClass(env, JNI_CLASS);
    if (klass == NULL){
        return JNI_ERR;
    }
    (*env)->RegisterNatives(env, klass, g_methods, NELEM(g_methods));

    return JNI_VERSION_1_6;
}