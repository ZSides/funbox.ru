#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "flags.h"
//#define _FILENAME "rb919.cap"
//#define _FILENAME "dump_ultra_small.cap"
#define _FILENAME "dump_ncat_1sess.cap"

typedef struct {
    unsigned char mac_src[6], mac_dst[6];
    uint16_t port_src, port_dst;
    char ipv;
    tcpFlag flag;
} tcpPacketData;

tcpPacketData parseTCP(unsigned char *d, const uint16_t *etype) {
    /*
    * HELLO HANDSHAKE: SYN-> SYN/ACK<- ACK->
    * GOODBYE HANDSHAKE: FIN/ACK-> FIN/ACK<- ACK-> 
    */
    tcpPacketData tcp;
    for (size_t i = 6; i < 12; ++i) {
        tcp.mac_src[i - 6] = d[i];
    }
    for (size_t i = 0; i < 6; ++i) {
        tcp.mac_dst[i] = d[i];
    }
    tcp.port_src = (d[34] << 8) + d[35];
    tcp.port_dst = (d[36] << 8) + d[37];
    tcp.ipv = 4 + 2 * (*etype != 0x0800);
    fillflag(&tcp.flag, tcp.ipv == 4 ? d[47] : d[51]);
    return tcp;
}

void printInfo(const tcpPacketData *tp) {
    printf("IPv%i. ", tp->ipv);
    for(int i = 0; i < 6; ++i)
        printf("%X ", tp->mac_src[i]);
    printf(": %i -> ", tp->port_src);
    for(int i = 0; i < 6; ++i)
        printf("%X ", tp->mac_dst[i]);
    printf(": %i ", tp->port_dst);
    printf("| FIN:%i SYN:%i RST:%i PSH:%i ACK:%i URG:%i", tp->flag.FIN, tp->flag.SYN, tp->flag.RST, tp->flag.PSH, tp->flag.ACK, tp->flag.URG);
    printf("\n");
}

int main(int argc, char** argv) {
    const size_t gHeaderSize = 6, pHeaderSize = 4;
    int32_t gHeader[gHeaderSize], pHeader[pHeaderSize];
    uint16_t eth_type;
    unsigned char *pData;
    tcpPacketData tcpPacket;
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
    for (size_t i = 0; i < gHeaderSize; ++i) {
    //    printf("%X ", gHeader[i]);
    }
    printf("\n");
    while (fread(pHeader, sizeof(pHeader[0]), pHeaderSize, dump)) {
        pData = (unsigned char*) malloc(pHeader[2] * sizeof(unsigned char)); // Might be faster than realloc(pData, pHeader[2])
        fread(pData, sizeof(pData[0]), pHeader[2], dump);
        eth_type = (pData[12] << 8) + pData[13];
        if ((pData[23] == 6) && (eth_type == 0x0800 || eth_type == 0x86DD)) {
            tcpPacket = parseTCP(pData, &eth_type);
            printInfo(&tcpPacket);
        }
        free(pData);
    }
    return 0;
}