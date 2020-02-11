#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define _FILENAME "rb919.cap"
//#define _FILENAME "dump_ultra_small.cap"

int main(int argc, char** argv) {
    int32_t gHeader[6], pHeader[4];
    unsigned char *pData;
    if (argc != 2) {
        printf("Usage: tcpsesscount filename\n");
    //    return 0; // uncomment before release
    }
    FILE *dump = fopen(_FILENAME, "rb");

    if (dump == NULL) {
        printf("Can't open file %s\n", _FILENAME);
        return 0;
    }
    
    for (size_t i = 0, gHeaderSize = fread(gHeader, sizeof(gHeader[0]), 6, dump); i < gHeaderSize; ++i) {
        printf("%X ", gHeader[i]);
    }
    printf("\n");
    int c = 0;
    while (fread(pHeader, sizeof(pHeader[0]), 4, dump)) {
        pData = (unsigned char*) malloc(pHeader[2] * sizeof(unsigned char));
        fread(pData, sizeof(pData[0]), pHeader[2], dump);
        c++;
        free(pData);
    }
    printf("Number of packets: %d\n", c);
    return 0;
}