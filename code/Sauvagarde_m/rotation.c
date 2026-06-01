/*
 * File:   Encoders.c
 * Author: M. Osee
 *
 * Created on 13 janvier 2013, 13:58
 */


#include "rotation.h"
#include "configuration.h"
#include <libpic30.h>


// Configures the quadrature encoders
void enc_config(void) { //règle POS1CNT et POS2CNT avec registre QEA 
    _QEA1R = 12;               // CHA assignment to its RP12 pin
    _QEB1R = 13;               // CHBA assignment to its RP15 pin (Comprend pas l'utilit?)
    QEI1CONbits.QEIM = ENC_X2_MODE;    // QEI configured in 4x mode without index
    QEI1CONbits.SWPAB = 0;      // no swap between CHA and CHB
    POS1CNT = 0;                // resets the tick counter
    _QEA2R = 7;               // CHA assignment to its RP7 pin
    _QEB2R = 6;               // CHBA assignment to its RP6 pin
    QEI2CONbits.QEIM = ENC_X2_MODE;    // QEI configured in 4x mode without index
    QEI2CONbits.SWPAB = 1;      // swap pour compte dans sens positif
    POS2CNT = 0;                // resets the tick counter
}


int16_t enc_getPos1(void) {
	return POS1CNT;
}

int16_t enc_getPos2(void) {
	return POS2CNT;
}

void enc_reset(void) {
    POS1CNT = 0 ;   
    POS2CNT = 6000 ; // resets the tick counter;problème si remplace par 2^16
}

void setup_UART(void){
    // Par d?faut on ne met pas en high speed (pr moins de bruit)
    extClockConfig(); //set FCY a 40MHz
    /* Configuration du Peripheral Pin Select (PPS) pour connecter le signal
     * Rx de l'UART1 ? RB6/RP6 et le signal Tx ? RB5/RP5 */
	_U1RXR = 3;    // U1RX -> RP3
	_RP4R = 3;     // RP4 -> U1Tx

    // format de trame 8N1, ? 57600 bits/s
    U1MODEbits.PDSEL = 0;       // 8 bits, pas de parit?
    U1MODEbits.STSEL = 0;       // 1 stop bit
    /* En mode standard, le d?bit est donn? par :
     * baud rate = FCY / (16*(U1BRG+1)      = 57.692
     * => U1BRG = (60MHz / (16*57.6kHz)) - 1  =  5.51*/
    U1MODEbits.BRGH = 0;
    //U1BRG = 64;
    U1BRG = 42;
    U1MODEbits.UARTEN = 1;      // on active l'UART
    U1STAbits.UTXEN = 1;        // on active l'?mission

}

void setup_PWM(void){ // coordonne timer avec OC1 et OC2
    _TRISB9 = 0; // PWM 2
    _TRISB10 = 0; // PWM 1
    _TRISB8 = 0; // dir 2
    _TRISB11 = 0; // dir 1
    
    _RP10R = 18; // Page 167 remap vers OC1 
    _RP9R = 19; // OC2 vers pin 9; pour la roue 2
    
    OC1R = 0;
    OC1RS = 1; // Min 1 max PR2
    OC1CONbits.OCM = 5; //page 212 : commence en low et repete la PWM sur le pin
    
    OC2R = 0;           
    OC2RS = 1;          
    OC2CONbits.OCM = 5;
    
    //On veut PER = 200?s
    //PR2 = 799; // Si 40MHz
    PR2 = 1199; // car PR2 = FCY*PER - 1 (moins car commence ? 0?) Si 60MHZ
    T2CONbits.TON = 1;  // starts timer1
    
}
void v_avant( (uint16_t) float seuil){ // faire reset(voir en haut))
    OC1RS = PR2*0.6 ;//roue gauche(avec bille avant))
    OC2RS = PR2*0,6;// roue droite
    _LATB8 = 0 ; // roue 1
    _LATB11 = 1 ; //roue 2
while (POS1CNT < seuil){// change type && POS2CNT > seuil
    x = enc_getPos1();
    y = 65535 - enc_getPos2();
    //correction(x,y,ref1,ref2,coef);
}
OC1RS = 1 ;
OC2RS = 1 ;
} 

/*void v_arriere(float seuil){
    enc_reset();
    _LATB8 = 1 ;
    _LATB11 = 0 ;//roue 2
    OC1RS = PR2/3 ;
    OC2RS = PR2/3 ;
    while ((float)POS1CNT >-seuil && (float)POS2CNT<seuil){
        if (abs(POS1CNT)-abs(POS2CNT)>val_empirique){//prendre valeur petite pour avoir précision
            float qot = abs(POS1CNT/POS2CNT) // attention divsion par 0
            OC2RS = OC2RS*qot
        } if (abs(POS2CNT)-abs(POS1CNT)>val_empirique){//prendre valeur petite pour avoir précision
            float qot = abs(POS2CNT/POS1CNT)
            OC1RS = OC1RS*qot
        }
    }
OC1RS = 1 ;// pour stopper la roue
OC2RS = 1 ;
} 
 */

void correction(int16_t pos1, int16_t pos2, float ref1, float ref2, float coef1, float coef2){
    int16_t err = 0;
    int16_t err_ant = 0 ; // mettre en variable non locale
    int16_t der = (err - err_ant) ; // mettre en variable non locale
    if (pos1 > pos2 ){
        err = pos1 - pos2;
        //if (OC1RS > 0.55*PR2){ // on le restricte pour ne pas avoir d'oscillation extrême entre éteint et allum
            OC2RS = ref2*PR2 - coef2*der; //+ coef1*err -
            OC1RS = ref1*PR2 + coef2*der;} // + coef1*err -
            pos1_ant = pos1 ;
            pos2_ant = pos2 ;
            err_ant = err ;
    }
    if (pos2 > pos1){
        err = pos2 - pos1;
        if (err > ){ // on leç restricte pr pas explosion
            OC2RS = ref2*PR2 - coef1*err - coef2*der ;
            OC1RS = ref1*PR2 + coef1*err + coef2*der; }
        pos1_ant = pos1 ;
        pos2_ant = pos2 ;
        err_ant = err ;
    }
}



/*void correction(int16_t pos1, int16_t pos2){
    if (pos1 > pos2){ // Verifie quelle roue est la plus loin
        if (OC1RS > 0.45*PR2){ // on le restricte pour ne pas avoir d'oscillation extr?me entre etteint et allum?
            //_LATB2 = 1;
        //OC2RS = OC2RS + (PR2*0.001);
        OC1RS = OC1RS - (0.03*OC1RS);}
        
    }
    if (pos2 > pos1){
        if (OC1RS < PR2*0.6){
        
        OC1RS = OC1RS + (0.03*OC1RS);}
        //OC2RS = OC2RS - (PR2*0.001); 
        
    }
}*/

