#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define _FILENAME "rb919.cap"
//#define _FILENAME "dump_ultra_small.cap"

int main(int argc, char** argv) {
    const size_t gHeaderSize = 6, pHeaderSize = 4;
    int32_t gHeader[gHeaderSize], pHeader[pHeaderSize];
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
    
    fread(gHeader, sizeof(gHeader[0]), gHeaderSize, dump);
    for (size_t i = 0; i < gHeaderSize; ++i) {
        printf("%X ", gHeader[i]);
    }
    printf("\n");
    int c = 0;
    while (fread(pHeader, sizeof(pHeader[0]), pHeaderSize, dump)) {
        pData = (unsigned char*) malloc(pHeader[2] * sizeof(unsigned char)); // Might be faster than realloc(pData, pHeader[2])
        fread(pData, sizeof(pData[0]), pHeader[2], dump);
        printf("%i. ", 1 + c);
        for (size_t i = 6; i < 12; ++i) {
            printf("%x", pData[i]);
            printf(i - 11 ? ":" : " ");
        }
        printf("-> ");
        for (size_t i = 0; i < 6; ++i) {
            printf("%x", pData[i]);
            printf(i - 5 ? ":" : " ");
        }
        printf("\n");
        c++;
        free(pData);
    }
    return 0;
}