//
// Created by huanghaibo on 2017/1/16.
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bt_packet.h"
#include "debug.h"
static uint16_t const crc16_table[256] =
        {
                0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
                0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
                0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
                0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
                0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
                0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
                0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
                0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
                0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
                0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
                0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
                0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
                0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
                0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
                0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
                0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
                0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
                0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
                0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
                0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
                0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
                0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
                0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
                0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
                0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
                0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
                0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
                0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
                0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
                0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
                0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
                0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
        };

packet_hdr_t *bd_bt_create_packet(void)
{
    packet_hdr_t *packet = calloc(1, sizeof(packet_hdr_t));
    if (!packet)
    {
        loge("%s malloc failed", __func__);
        return NULL;
    }

    packet->magic = 0xab;//magic
    packet->reserve = 0x0;//reserve errorFlag ackFlag version
    packet->err_flag = 0x0;
    packet->ack_flag = 0x0;
    return packet;
}

void bd_bt_set_version(packet_hdr_t *packet, int version)
{
    assert(packet != NULL);
    uint8_t ver = (uint8_t) (version & 0x000F);
    packet->version = (uint8_t) (version < 0 ? 0 : ver);
}
void bd_bt_set_payload_length(packet_hdr_t *packet, uint16_t payload_len)
{
    assert(packet != NULL);
    packet->payload_len = (payload_len >> 8 )|(payload_len<<8);
}
void bd_bt_set_key_value(packet_hdr_t *packet, int key, uint8_t *value, size_t size)
{
    assert(packet != NULL);
    if (key > 0)
    {
        key_value_t *key_value = calloc(1, sizeof(key_value_t));
        key_value->key = (uint8_t) (key & 0xFF);
        key_value->key_hdr_reserve = 0x0;
        key_value->key_hdr_value_len = (uint16_t) (size & 0x1F);
        if (value == NULL || size == 0)
        {
            logw("%s: value is null", __func__);
        }
        else
        {
            key_value->value = calloc(size, sizeof(uint8_t));

            memcpy(key_value->value, value, size);
        }
        queue_put(packet->q_value, key_value);
    }
    else
    {
        logw("key is less than 0: %d", key);
    }
}

void bd_bt_set_magic(packet_hdr_t *packet, int magic)
{
    packet->magic = (uint8_t) (magic & 0xFF);//magic
}
void bd_bt_set_err_flag(packet_hdr_t *packet, int errFlag)
{
    packet->err_flag = (uint8_t) (errFlag & 0x01);
}
void bd_bt_set_ack_flag(packet_hdr_t *packet, int ackFlag)
{
    packet->ack_flag = (uint8_t) (ackFlag & 0x01);
}

static uint16_t crc16_byte(uint16_t crc, const uint8_t data)
{
    return (crc >> 8) ^ crc16_table[(crc ^ data) & 0xff];
}

uint16_t bd_crc16(uint16_t crc, uint8_t const *buffer, uint16_t len)
{
    while (len--)
        crc = crc16_byte(crc, *buffer++);
    return crc;
}
//CRC只计算L1的payload, 即L2数据
void bd_bt_set_crc16(packet_hdr_t *packet, uint8_t *buf, int size)
{
    uint16_t crc = bd_crc16(0, buf, size);
    packet->crc16 = (crc >> 8 )| ( crc<<8 );
}
void bd_bt_set_seq_id(packet_hdr_t *packet, int seqId)
{
    uint16_t seq_id = (uint16_t) (seqId & 0xFFFF);
    packet->seq_id = (seq_id);
}
void bd_bt_set_cmdId_version(packet_hdr_t *packet, int cmdId, int version)
{
    packet->cmd_id = (uint8_t) (cmdId & 0x0F);
    packet->data_hdr_version = (uint8_t) (version & 0x08);
}
packet_hdr_t *bd_bt_packet_wrap(int cmdId, int version, int size, uint8_t *payload)
{
    packet_hdr_t *packet = malloc(sizeof(packet_hdr_t));
    if (!packet)
    {
        loge("%s malloc failed", __func__);
        return NULL;
    }

    packet->magic = 0xab;//magic
    packet->reserve = 0x0;//reserve errorFlag ackFlag version
    packet->err_flag = 0x0;
    packet->ack_flag = 0x0;
    uint8_t ver = (uint8_t) (version & 0x000F);
    loge("%s---ver=%02x", __func__, ver);
    packet->version = (uint8_t) (version < 0 ? 0 : ver);

//    data[0] = 0xab;//magic
//    data[1] = 0x00;//reserve errorFlag ackFlag version
//    data[2] = 0x00;//payload length
//    data[3] = 0x05;//payload length

//    data[4] = 0xc5;//crc16[0];
//    data[5] = 0x89;//crc16[1];

//    data[6] = 0x00;//seq id
//    data[7] = 0x1e;//seq id
/*    packet->data_len = ((0x05 << 8) | 0x00);
    uint16_t data_len = (uint16_t) (((packet->data_len & 0xFF00) >> 8) | ((packet->data_len & 0x00FF) << 8));
    logi("==========data len=0x%04X, data_len=%d", packet->data_len, data_len);
    packet->crc16 = ((0x89<<8)| (0xc5 & 0xFF));
    packet->seq_id = ((0x1e<<8) | 0x0);

    int i= 0;
    uint8_t *buf = (uint8_t *) packet;


    //if (data_len >= 2)
    if (cmdId > 0)
    {
        packet->cmd_id = (uint8_t) (cmdId & 0x00FF);//0x06;//cmd id
        loge("cmd id=%02x", packet->cmd_id);
        packet->data_hdr_version = 0x00;//version 4bits
        packet->data_hdr_reserve = 0x00;//reserve 4bits


        if ((size > 2) && !payload)
        {
            packet->key_value = malloc(size);
            if (!packet->key_value)
            {
                free(packet);
                loge("%s malloc failed", __func__);
                return NULL;
            }

            packet->key = 0x10;//key
            packet->key_hdr_reserve= 0x0;//key header,7bit
            packet->key_hdr_value_len = 0x00;//key header 9bit, key value length
            for (i = 0; i < 13; i++)
            {
                logi("i=%d 0x%02x", i, (buf[i]));
            }
        }
    }*/
    return packet;
}