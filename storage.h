#include <stdint.h>
#include "flags.h"
#include "tcppacketdata.h"
#include <stdlib.h>
typedef struct {
    int64_t current_size, resize_size, resize_base_value;
    tcpPacketData *tcpArray;
} storage;

void storage_init(storage *strg) {
    strg->current_size = 0;
    strg->resize_size = 3;
    strg->resize_base_value = 5;
    strg->tcpArray = (tcpPacketData *) malloc(sizeof(tcpPacketData) * strg->resize_size);
}

void storage_resize(storage *strg) {
    strg->resize_size += strg->resize_base_value;
    strg->tcpArray = (tcpPacketData *) realloc(strg->tcpArray, sizeof(tcpPacketData) * strg->resize_size);
}

int storage_push(storage *strg, tcpPacketData element) {
    strg->tcpArray[strg->current_size] = element;
    strg->current_size += 1;
    if (strg->current_size == strg->resize_size) {
        storage_resize(strg);
    }
    return strg->current_size;
}