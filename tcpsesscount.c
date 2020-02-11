#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define _FILENAME "rb919.cap"
//#define _FILENAME "dump_ultra_small.cap"

int main(int argc, char** argv) {
    int32_t sysinfo[6], header[4];
    size_t buffsize;
    if (argc != 2) {
        printf("Usage: tcpsesscount filename\n");
    //    return 0; // uncomment before release
    }
    FILE *dump = fopen(_FILENAME, "rb");

    if (dump == NULL) {
        printf("Can't open file %s\n", _FILENAME);
        return 0;
    }
    
    for (int i = 0, sz = fread(sysinfo, sizeof(sysinfo[0]), 6, dump); i < sz; ++i) {
        printf("%X ", sysinfo[i]);
    }
    printf("\n");
    int c = 0;
    unsigned char *data = NULL;
    while (fread(header, sizeof(header[0]), 4, dump)) {
        data = (unsigned char*) malloc(header[2] * sizeof(unsigned char));
        fread(data, sizeof(data[0]), header[2], dump);
        c++;
        free(data);
    }
    printf("Number of pockets: %d\n", c);
    return 0;
}