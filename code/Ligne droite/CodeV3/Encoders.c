*
 * File:   Encoders.c
 * Author: M. Osee
 *
 * Created on 13 janvier 2013, 13:58
 */


#include "Encoders.h"


// Configures the quadrature encoders
void enc_config(void) {
    _QEA1R = 12;               // CHA assignment to its RP12 pin
    _QEB1R = 13;               // CHBA assignment to its RP15 pin (Comprend pas l'utilité)
    QEI1CONbits.QEIM = ENC_X2_MODE;    // QEI configured in 4x mode without index
    QEI1CONbits.SWPAB = 0;      // no swap between CHA and CHB
    POS1CNT = 0;                // resets the tick counter
    _QEA2R = 7;               // CHA assignment to its RP7 pin
    _QEB2R = 6;               // CHBA assignment to its RP6 pin
    QEI2CONbits.QEIM = ENC_X2_MODE;    // QEI configured in 4x mode without index
    QEI2CONbits.SWPAB = 0;      // no swap between CHA and CHB
    POS2CNT = 0;                // resets the tick counter
}


int16_t enc_getPos1(void) {
	return POS1CNT;
}

int16_t enc_getPos2(void) {
	return POS2CNT;
}

void enc_reset(void) {
    POS1CNT = 0;   
    POS2CNT = 0; // resets the tick counter
}

void setup_UART(void){
    // Par défaut on ne met pas en high speed (pr moins de bruit)
    extClockConfig(); //set FCY a 40MHz
    /* Configuration du Peripheral Pin Select (PPS) pour connecter le signal
     * Rx de l'UART1 à RB6/RP6 et le signal Tx à RB5/RP5 */
	_U1RXR = 3;    // U1RX -> RP3
	_RP4R = 3;     // RP4 -> U1Tx

    // format de trame 8N1, à 57600 bits/s
    U1MODEbits.PDSEL = 0;       // 8 bits, pas de parité
    U1MODEbits.STSEL = 0;       // 1 stop bit
    /* En mode standard, le débit est donné par :
     * baud rate = FCY / (16*(U1BRG+1)      = 57.692
     * => U1BRG = (60MHz / (16*57.6kHz)) - 1  =  5.51*/
    U1MODEbits.BRGH = 0;
    //U1BRG = 64;
    U1BRG = 42;
    U1MODEbits.UARTEN = 1;      // on active l'UART
    U1STAbits.UTXEN = 1;        // on active l'émission

}

void setup_PWM(void){
    _TRISB9 = 0; // PWM 2
    _TRISB10 = 0; // PWM 1
    _TRISB8 = 0; // dir 2
    _TRISB11 = 0; // dir 1
    
    _RP10R = 18; // Page 167 remap vers OC1 
    _RP9R = 19; 
    
    OC1R = 0;
    OC1RS = 1; // Min 1 max PR2
    OC1CONbits.OCM = 5; //page 212 : commence en low et repete la PWM sur le pin
    
    OC2R = 0;           
    OC2RS = 1;          
    OC2CONbits.OCM = 5;
    
    //On veut PER = 200µs
    PR2 = 799; // Si 40MHz
    //PR2 = 1199; // car PR2 = FCY*PER - 1 (moins car commence à 0?) Si 60MHZ
    T2CONbits.TON = 1;  // starts timer1
    
}