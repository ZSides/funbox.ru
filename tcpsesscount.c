#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "flags.h"
#include "storage.h"
#include "tcppacketdata.h"
//#define _FILENAME "rb919.cap"
//#define _FILENAME "dump_ultra_small.cap"
#define _FILENAME "dump_ncat_1sess.cap"



tcpPacketData parseTCP(uint8_t *d, const uint16_t *etype) {
    tcpPacketData tcp;
    for (int32_t i = 6; i < 12; ++i) {
        tcp.mac_src[i - 6] = d[i];
    }
    for (int32_t i = 0; i < 6; ++i) {
        tcp.mac_dst[i] = d[i];
    }
    for (int32_t i = 26; i < 30; ++i) {
        tcp.ip_src[i - 26] = d[i];
    }
    for (int32_t i = 30; i < 34; ++i) {
        tcp.ip_dst[i - 30] = d[i];
    }
    tcp.port_src = (d[34] << 8) + d[35];
    tcp.port_dst = (d[36] << 8) + d[37];
    tcp.ipv = 4;
    fillflag(&tcp.flag, d[47]);
    return tcp;
}

void printInfo(const tcpPacketData *tp) {
    printf("IPv%i. ", tp->ipv);
    for(int32_t i = 0; i < 6; ++i)
        printf((i != 5 ? "%X:" : "%X "), tp->mac_src[i]);
    printf("-> ");
    for(int32_t i = 0; i < 6; ++i)
        printf((i != 5 ? "%X:" : "%X "), tp->mac_dst[i]);
    printf("| ");
    for (int32_t i = 0; i < 4; ++i)
        printf((i != 3 ? "%d." : "%d"), tp->ip_src[i]);
    printf(":%i -> ", tp->port_src);
    for (int32_t i = 0; i < 4; ++i)
        printf((i != 3 ? "%d." : "%d"), tp->ip_dst[i]);
    printf(":%i ", tp->port_dst);
    printf("| FIN:%i SYN:%i RST:%i PSH:%i ACK:%i URG:%i | ", tp->flag.FIN, tp->flag.SYN, tp->flag.RST, tp->flag.PSH, tp->flag.ACK, tp->flag.URG);
    printf("\n");
}

int main(int argc, char** argv) {
    const size_t gHeaderSize = 6, pHeaderSize = 4;
    int32_t gHeader[gHeaderSize], pHeader[pHeaderSize];
    uint16_t eth_type;
    uint8_t *pData;
    tcpPacketData tcpPacket;
    storage strg;
    storage_init(&strg);
    if (argc != 2) {
        printf("Usage: tcpsesscount filename\n");
    //    return 0; // uncomment before release
    }
    FILE *dump = fopen(_FILENAME, "rb");

    if (dump == NULL) {
        printf("Can't open file %s\n", _FILENAME);
        return 0;
    }
    
    fread(gHeader, sizeof(gHeader[0]), gHeaderSize, dump);
    for (size_t i = 0; i < gHeaderSize; ++i) printf("%X(Integer: %d) ", gHeader[i], gHeader[i]);
    printf("\n");
    while (fread(pHeader, sizeof(pHeader[0]), pHeaderSize, dump)) {
        pData = (uint8_t*) malloc(pHeader[2] * sizeof(uint8_t)); // Might be faster than realloc(pData, pHeader[2])
        fread(pData, sizeof(pData[0]), pHeader[2], dump);
        eth_type = (pData[12] << 8) + pData[13];
        if ((pData[23] == 6) && (eth_type == 0x0800)) {
            tcpPacket = parseTCP(pData, &eth_type);
            printInfo(&tcpPacket);
        }
        storage_push(&strg, tcpPacket);
        free(pData);
    }
    return 0;
}