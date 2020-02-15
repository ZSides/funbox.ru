#pragma once
#include <stdint.h>
#include "flags.h"

typedef struct {
    uint8_t mac_src[6], mac_dst[6], ip_src[4], ip_dst[4];
    uint16_t port_src, port_dst;
    int8_t ipv;
    tcpFlag flag;
} tcpPacketData;