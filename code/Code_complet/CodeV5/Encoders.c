/*
 * File:   Encoders.c
 * Author: M. Osee
 *
 * Created on 13 janvier 2013, 13:58
 */
/*
 * File:   Encoders.c
 * Author: M. Osee
 *
 * Created on 13 janvier 2013, 13:58 
 */


#include "Encoders.h"
#define FCY 40000000  

#include <math.h>

// Configures the quadrature encoders
void enc_config(void) {
    _QEA1R = 12;               // CHA assignment to its RP12 pin
    _QEB1R = 13;               // CHBA assignment to its RP15 pin (Comprend pas l'utilité)
    QEI1CONbits.QEIM = ENC_X2_MODE;    // QEI configured in 4x mode without index
    QEI1CONbits.SWPAB = 1;      // no swap between CHA and CHB
    POS1CNT = 0;                // resets the tick counter
    _QEA2R = 7;               // CHA assignment to its RP7 pin
    _QEB2R = 6;               // CHBA assignment to its RP6 pin
    QEI2CONbits.QEIM = ENC_X2_MODE;    // QEI configured in 4x mode without index
    QEI2CONbits.SWPAB = 0;      // swap pour compte dans sens positif
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
     * baud rate = FCY / (16*(U1BRG+1) 
     * => U1BRG = (40MHz / (16*57.6kHz)) - 1  =  5.51*/
    U1MODEbits.BRGH = 0;
    U1BRG = 42;
    U1MODEbits.UARTEN = 1;      // on active l'UART
    U1STAbits.UTXEN = 1;        // on active l'émission
}

void setup_PWM(void){
    _TRISB9 = 0; // PWM 2
    _TRISB10 = 0; // PWM 1
    _TRISB8 = 0; // dir 2
    _TRISB11 = 0; // dir 1
    
    OC1CONbits.OCTSEL = 0;
    OC2CONbits.OCTSEL = 0;
    
    
    _RP10R = 18; // Page 167 remap vers OC1 
    _RP9R = 19; 
    
    OC1R = 0;
    OC1RS = 1; // Min 1 max PR2
    OC1CONbits.OCM = 5; //page 212 : commence en low et repete la PWM sur le pin
    
    OC2R = 0;           
    OC2RS = 1;          
    OC2CONbits.OCM = 5;
    
    //On veut PER = 200µs
    //PR2 = 799; // Si 40MHz
    PR2 = 1199; // car PR2 = FCY*PER - 1 (moins car commence à 0?) Si 60MHZ
    T2CONbits.TON = 1;  // starts timer1
    
}


int16_t mettre_to_pos(float cm){
    // Ajout de la l'erreur mesurée experimentallement sur 120cm
    cm = cm + (cm *3/120) + (cm * 1/120);
    float inter = cm*67.906; // 1920/(4.5*2*3.14)
    // Le casting a été vérifié en C
    int16_t p = (int16_t)inter; 
    return p;
}


float absolu(float value){
    if (value < 0){value = -1*value;}
    return value;
               
}

void sendb(int *list){
    int i=0;
        //while(i < 8){
    while(1==1){
    // Envoye l'info via UART en boucle
        int8_t y = list[i] ;
        //int8_t msb = (y >>8) & 0xFF; // Donne les 8 premiers bits
        //int8_t lsb = y;
        while (U1STAbits.UTXBF) {}  // on attend que le buffer d'émission soit libre
        U1TXREG = y; // Besoin que du LSB car info = 0 1 ou 2
        i = i + 1;
        if (i>8){i = 0;}
        
        }

}

int octet_to_int(int *liste){
    int out = 0;
    int multi = 128;
    for (int i = 0; i < 8; i++){
        out = out + (liste[i]*multi);
        multi = multi*0.5;
    }
    return out;
}

int ispair(int number){
    if (number == 0 || number == 2 ||number == 4 || number == 6 || number == 8 || number == 10)
    {return 0;}
    else {return 1;}
}


