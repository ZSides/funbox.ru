#pragma once

#include <stdint.h>
#include "flags.h"
#include "tcppacketdata.h"
#include <stdlib.h>
#include <stdio.h>

void printInfo(tcpPacketData *tp) {
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

// function-helper in library_push.
int8_t isArraysEqual(uint8_t *a1, uint8_t *a2, uint64_t cnt) {
    while (cnt--) {
        if(a1[cnt] != a2[cnt]) {
            return 0;
        }
    }
    return 1;
}
typedef struct {
    uint64_t current_size, resize_size, resize_base_value;
    tcpPacketData *tcpArray;
} storage;

void storage_init(storage *strg) {
    strg->current_size = 0;
    strg->resize_size = 1<<10;
    strg->resize_base_value = 1<<9;
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

tcpPacketData storage_pop(storage *strg) {
    return strg->tcpArray[strg->current_size--];
}

void storage_destroy(storage *strg) {
    free(strg->tcpArray);
}

void storage_clear(storage *strg) {
    storage_destroy(strg);
    storage_init(strg);
}

void storage_print(storage *strg, uint64_t from, uint64_t to) {
    for(; from < to; ++from) {
        printf("%i) ", from);
        printInfo(&strg->tcpArray[from]);
    }
}

void storage_print_a(storage *strg) {
    storage_print(strg, 0, strg->current_size);
}


typedef struct {
    uint64_t current_size, resize_size, resize_base_value;
    storage *storageArray;
} library;

void library_init(library *lib) {
    lib->current_size = 0;
    lib->resize_size = 1<<10;
    lib->resize_base_value = 1<<9;
    lib->storageArray = (storage *) malloc(sizeof(storage) * lib->resize_size);
}

void library_resize(library *lib) {
    lib->resize_size += lib->resize_base_value;
    lib->storageArray = (storage *) realloc(lib->storageArray, sizeof(storage) * lib->resize_size);
}

int library_push(library *lib, tcpPacketData element) {
    if (lib->current_size == 0) {
        storage tmpstorage;
        storage_init(&tmpstorage);
        storage_push(&tmpstorage, element);
        lib->storageArray[lib->current_size] = tmpstorage;
        lib->current_size += 1;
        if (lib->current_size == lib->resize_size) {
            library_resize(lib);
        }
    } else {
        uint8_t is_push_req = 0;
        uint64_t id_if_push_req;
        for (uint64_t i = 0, s = lib->current_size; i < s; ++i) {
            if (((isArraysEqual(lib->storageArray[i].tcpArray[0].ip_dst, element.ip_dst, 4)) && 
                (lib->storageArray[i].tcpArray[0].port_dst == element.port_dst) &&
                (isArraysEqual(lib->storageArray[i].tcpArray[0].ip_src, element.ip_src, 4)) && 
                (lib->storageArray[i].tcpArray[0].port_src == element.port_src)) || 
                ((isArraysEqual(lib->storageArray[i].tcpArray[0].ip_dst, element.ip_src, 4)) && 
                (lib->storageArray[i].tcpArray[0].port_dst == element.port_src) &&
                (isArraysEqual(lib->storageArray[i].tcpArray[0].ip_src, element.ip_dst, 4)) && 
                (lib->storageArray[i].tcpArray[0].port_src == element.port_dst))) {
                is_push_req = 1;
                id_if_push_req = i;
                break;
            }
        }
        if (is_push_req) { // если такой уже есть, то мы должны запушить его туда
            storage_push(&lib->storageArray[id_if_push_req], element);
        } else { // создаем новый
            storage tmpstorage;
            storage_init(&tmpstorage);
            storage_push(&tmpstorage, element);
            lib->storageArray[lib->current_size] = tmpstorage;
            lib->current_size += 1;
            if (lib->current_size == lib->resize_size) {
                library_resize(lib);
            }
        }
    }
    return lib->current_size;
}

void library_print(library *lib) {
    for (uint64_t i = 0, s = lib->current_size; i < s; ++i) {
        printf("Block #%d:\n", i);
        storage_print_a(&lib->storageArray[i]);
    }
}

void library_clear(library *lib) {
    for(int64_t i = 0, s = lib->current_size; i < s; ++i) {
        storage_destroy(&lib->storageArray[i]);
    }
}