//
// Created by huanghaibo on 2017/1/16.
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "bt_packet.h"
#include "debug.h"

packet_hdr_t *bd_bt_packet_wrap()
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
    packet->version = 0x0;
//    data[0] = 0xab;//magic
//    data[1] = 0x00;//reserve errorFlag ackFlag version
//    data[2] = 0x00;//payload length
//    data[3] = 0x05;//payload length

//    data[4] = 0xc5;//crc16[0];
//    data[5] = 0x89;//crc16[1];

//    data[6] = 0x00;//seq id
//    data[7] = 0x1e;//seq id
    packet->data_len = ((0x05 << 8) | 0x00);
    uint16_t data_len = (uint16_t) (((packet->data_len & 0xFF00) >> 8) | ((packet->data_len & 0x00FF) << 8));
    logi("==========data len=0x%04X, data_len=%d", packet->data_len, data_len);
    packet->crc16 = ((0x89<<8)| (0xc5 & 0xFF));
    packet->seq_id = ((0x1e<<8) | 0x0);

    int i= 0;
    uint8_t *buf = (uint8_t *) packet;
    for (i = 0; i < 8; i++)
    {
        loge("i=%d 0x%02x", i, (buf[i]));
    }

    if (data_len >= 2)
    {
        packet->data_hdr = malloc(sizeof(data_hdr_t));
        if (!packet->data_hdr)
        {
            free(packet);
            loge("%s malloc failed", __func__);
            return NULL;
        }
        packet->data_hdr->cmd_id = 0x06;//cmd id
        packet->data_hdr->version = 0x00;//version 4bits
        packet->data_hdr->reserve = 0x00;//reserve 4bits
        buf = (uint8_t *) packet->data_hdr;
        for (i = 0; i < 2; i++)
        {
            logw("i=%d 0x%02x", i, (buf[i]));
        }

        if (data_len>2)
        {
            packet->data_hdr->data = malloc(sizeof(data_t));
            if (!packet->data_hdr->data)
            {
                free(packet->data_hdr);
                free(packet);
                loge("%s malloc failed", __func__);
                return NULL;
            }
            packet->data_hdr->data->key = 0x10;//key
            packet->data_hdr->data->key_hdr_reserve= 0x0;//key header,7bit
            packet->data_hdr->data->key_hdr_value_len = 0x00;//key header, 9bit
            buf = (uint8_t *) packet->data_hdr->data;
            for (i = 0; i < 3; i++)
            {
                logi("i=%d 0x%02x", i, (buf[i]));
            }
        }
    }
    return packet;
}