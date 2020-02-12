#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define _FILENAME "rb919.cap"
//#define _FILENAME "dump_ultra_small.cap"

typedef struct {
    unsigned char mac_src[6], mac_dst[6];
    u_int16_t port_src, port_dst;
} tcpPacketData;

tcpPacketData parseTCP(unsigned char *d) {
    tcpPacketData tcp;
    for (size_t i = 6; i < 12; ++i) {
        tcp.mac_src[i - 6] = d[i];
    }
    for (size_t i = 0; i < 6; ++i) {
        tcp.mac_dst[i] = d[i];
    }
    tcp.port_src = (d[34] << 8) + d[35];
    tcp.port_dst = (d[36] << 8) + d[37];
    return tcp;
}

int main(int argc, char** argv) {
    const size_t gHeaderSize = 6, pHeaderSize = 4;
    int32_t gHeader[gHeaderSize], pHeader[pHeaderSize];
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
        tcpPacket = parseTCP(pData);
        for(int i = 0; i < 6; ++i)
            printf("%X ", tcpPacket.mac_src[i]);
        printf(": %i -> ", tcpPacket.port_src);
        for(int i = 0; i < 6; ++i)
            printf("%X ", tcpPacket.mac_dst[i]);
        printf(": %i\n", tcpPacket.port_dst);
        free(pData);
    }
    return 0;
}