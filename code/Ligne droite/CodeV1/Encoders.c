/*
 * File:   Encoders.c
 * Author: M. Osee
 *
 * Created on 13 janvier 2013, 13:58
 */


#include "Encoders.h"


// Configures the quadrature encoders
void enc_config(uint8_t rpA, uint8_t rpB, uint8_t mode) {
    _QEA1R = rpA;               // CHA assignment to its RP pin
    _QEB1R = rpB;               // CHBA assignment to its RP pin
    QEI1CONbits.QEIM = mode;    // QEI configured in 4x mode without index
    QEI1CONbits.SWPAB = 0;      // no swap between CHA and CHB
    POS1CNT = 0;                // resets the tick counter
}


int16_t enc_getPos(void) {
	return POS1CNT;
}


void enc_reset(void) {
    POS1CNT = 0;            // resets the tick counter
}
