//
// Created by huanghaibo on 2017/1/16.
//

#ifndef BLUETOOTHKIT_BT_PACKET_H
#define BLUETOOTHKIT_BT_PACKET_H

#include <stdio.h>


//payload
typedef struct {
    uint8_t key;
    uint16_t key_hdr_reserve:7;
    uint16_t key_hdr_value_len:9;
    uint8_t *value;
}data_t;

//L2 packet(2-504), L2 header+L2 payload
typedef struct {
    uint8_t cmd_id;
    uint8_t version:4;
    uint8_t reserve:4;
    data_t *data;
}data_hdr_t;

//L1 packet(8-512)
typedef struct {
    uint8_t magic;//8 bit
    uint8_t reserve:2;//2 bit
    uint8_t err_flag:1;
    uint8_t ack_flag:1;
    uint8_t version:4;
    uint16_t data_len;
    uint16_t crc16;
    uint16_t seq_id;
    data_hdr_t *data_hdr;
}packet_hdr_t;

#endif //BLUETOOTHKIT_BT_PACKET_H
