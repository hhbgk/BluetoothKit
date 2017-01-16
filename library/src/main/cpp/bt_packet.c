//
// Created by huanghaibo on 2017/1/16.
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "bt_packet.h"
#include "debug.h"

void bd_band_wrap()
{
    packet_hdr_t *packet = malloc(sizeof(packet_hdr_t));
    if (!packet)
    {
        loge("%s malloc failed", __func__);
        return;
    }

    packet->magic = 0xab;//magic
    packet->reserve = 0x0;//reserve errorFlag ackFlag version
    packet->err_flag = 0x0;//payload length
    packet->ack_flag = 0x0;//payload length
    packet->version = 0x0;
    packet->data_len = 0x05;
    packet->crc16 = (0xc5<<8)|0x89;
    packet->seq_id = (0x00<<8)| 0x1e;//seq id
    packet->data_hdr = malloc(sizeof(data_hdr_t));
    if (!packet->data_hdr)
    {
        free(packet);
        loge("%s malloc failed", __func__);
        return;
    }

    packet->data_hdr->cmd_id = 0x06;//cmd id
    packet->data_hdr->version = 0x0;//version 4bits & reserve 4bits
    packet->data_hdr->reserve = 0x0;
    packet->data_hdr->data = malloc(sizeof(data_t));
    if (!packet->data_hdr->data)
    {
        free(packet->data_hdr);
        free(packet);
        loge("%s malloc failed", __func__);
        return;
    }
    packet->data_hdr->data->key = 0x10;//key
    packet->data_hdr->data->key_hdr_reserve= 0x0;//key header
    packet->data_hdr->data->key_hdr_value_len = 0x00;//key header
}