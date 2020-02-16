#pragma once

#include <stdint.h>
#include "library.h"

void analysis(library *lib) {
    // syn | syn/ack | ack
    // fin/ack | fin/ack | ack
    uint64_t whs, ghs;
    uint64_t closed_good, closed_bad, unclosed, g_ok, g_bad, g_unc;
    uint8_t of, cf;
    g_ok = g_bad = g_unc = 0;
    for (uint64_t i = 0, lib_size = lib->current_size; i < lib_size; ++i) {
        closed_good = closed_bad = unclosed = 0;
        of = cf = 0; // open flag sets to 1 if welcome hs found, close flag sets to 1 if goodbye hs found.
        whs = ghs = 0; // counters for welcome and goodbye handshakes
        for (uint64_t j = 0, strg_size = lib->storageArray[i].current_size - 2; j < strg_size; ++j) {
            if ((lib->storageArray[i].tcpArray[j].flag.SYN) && 
                ((lib->storageArray[i].tcpArray[j + 1].flag.SYN) && (lib->storageArray[i].tcpArray[j + 1].flag.ACK)) &&
                (lib->storageArray[i].tcpArray[j + 2].flag.ACK)) {
                //printf("welcome handshake at block #%i (packets %i-%i)\n", i, j, j + 2);
                whs += 1; // increase counter
                if (of == 1) { // if we found welcome handshake again -> previous was closed bad.
                    closed_bad += 1;
                }
                of = 1;
            }
            if (((lib->storageArray[i].tcpArray[j].flag.FIN) && (lib->storageArray[i].tcpArray[j].flag.ACK)) &&
                ((lib->storageArray[i].tcpArray[j + 1].flag.FIN) && (lib->storageArray[i].tcpArray[j + 1].flag.ACK)) &&
                (lib->storageArray[i].tcpArray[j + 2].flag.ACK)) {
                //printf("goodbye handshake at block #%i (packets %i-%i)\n", i, j, j + 2);
                ghs += 1; // increase counter
                cf = 1;
                if ((of == 1) && (cf == 1)) {
                    of = cf = 0;
                    closed_good += 1;
                }
            }
        }
        //printf("Welcome hs: %i Gb hs: %i oflag: %i cflag: %i\n", whs, ghs, of, cf);
        if ((ghs == 0) && (whs == 0)) { // no handshakes
            unclosed = 1;
            closed_good = closed_bad = 0;
        } else if ((of == 1) && (cf == 0)) { // eof w/o closing
            unclosed += 1;
        }
        g_ok += closed_good;
        g_bad += closed_bad;
        g_unc += unclosed;
        //printf("Block %i status: Closed good: %d; Closed bad: %d; Unclosed: %d\n", i, closed_good, closed_bad, unclosed);
    }
    printf("Closed regularly: %i\nClosed abnormally: %i\nUnclosed: %i\nTotal: %i\n", g_ok, g_bad, g_unc, g_ok + g_bad + g_unc);
}