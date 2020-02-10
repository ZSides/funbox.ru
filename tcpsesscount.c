#include <stdio.h>
#define _FILENAME "rb919.cap"

int main(int argc, char** argv) {
    unsigned char sysinfo[24], buffer[256];
    size_t buffsize;
    /*if (argc == 1) {
        printf("Usage: tcpsesscount filename\n");
        return 0;
    }*/
    FILE *dump = fopen(_FILENAME, "rb");

    if (dump == NULL) {
        printf("Can't open file %s\n", _FILENAME);
        return 0;
    }
    
    for (int i = 0, sz = fread(sysinfo, sizeof(sysinfo[0]), sizeof(sysinfo), dump); i < sz; ++i) {
        printf("%X ", sysinfo[i]);
        if (i % 4 == 3) {
            printf("| ");
        }
    }
    printf("File version: %d.%d\n", sysinfo[4], sysinfo[6]);
    while (buffsize = fread(buffer, sizeof(buffer[0]), sizeof(buffer), dump)) {
        for (int i = 0; i < buffsize; ++i) {
            printf("%X ", buffer[i]);
            if (i % 4 == 3) {
                printf("| ");
            }
        }
        break;
    }
    printf("\n");
    return 0;
}