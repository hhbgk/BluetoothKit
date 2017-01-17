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
}key_value_t;

//L2 packet(2-504), L2 header+L2 payload
typedef struct {
    uint8_t cmd_id;
    uint8_t version:4;
    uint8_t reserve:4;
    key_value_t *data;
}data_hdr_t;

//L1 packet(8-512)
typedef struct packet_hdr{
    uint8_t magic;//8 bit
    uint8_t reserve:2;//2 bit
    uint8_t err_flag:1;
    uint8_t ack_flag:1;
    uint8_t version:4;
    uint16_t data_len;
    uint16_t crc16;
    uint16_t seq_id;
    //data_hdr_t *data_hdr;
    uint8_t cmd_id;
    uint8_t data_hdr_version:4;
    uint8_t data_hdr_reserve:4;
    //data_t *data;
    uint8_t key;
    uint16_t key_hdr_reserve:7;
    uint16_t key_hdr_value_len:9;
    uint8_t *value;
    key_value_t *key_value;
}packet_hdr_t;

packet_hdr_t * bd_bt_packet_wrap(int, int, int, uint8_t *);

#endif //BLUETOOTHKIT_BT_PACKET_H
