/*
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
     * baud rate = FCY / (16*(U1BRG+1)      = 57.692
     * => U1BRG = (60MHz / (16*57.6kHz)) - 1  =  5.51*/
    U1MODEbits.BRGH = 0;
    //U1BRG = 64;
    U1BRG = 42;
    U1MODEbits.UARTEN = 1;      // on active l'UART
    U1STAbits.UTXEN = 1;        // on active l'émission

}

void setup_PWM(void){
    _TRISB9 = 0; // pour fixer les pins en sortie;PWM 2
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
    //PR2 = 799; // Si 40MHz
    PR2 = 1199; // car PR2 = FCY*PER - 1 (moins car commence à 0?) Si 60MHZ
    T2CONbits.TON = 1;  // starts timer1
    
}




void correction_PI(int16_t x, int16_t y, float *ref1, float *ref2)
{
    static int16_t old_x = 0;
    static int16_t old_y = 0;
    static float integ = 0;

    int16_t v1;
    int16_t v2;

    float err;
    float corr;

    // calcul vitesse roues
    v1 = x - old_x;
    v2 = y - old_y;

    // erreur
    err = (float)(v1 - v2);

    // intégrale
    integ += err;

    if(integ > INTEG_MAX) integ = INTEG_MAX;
    if(integ < INTEG_MIN) integ = INTEG_MIN;

    // correcteur PI
    corr = KP * err + KI * integ;

    // correction PWM
    *ref1 -= corr;
    *ref2 += corr;

    // saturation
    if(*ref1 > PWM_MAX) *ref1 = PWM_MAX;
    if(*ref1 < PWM_MIN) *ref1 = PWM_MIN;

    if(*ref2 > PWM_MAX) *ref2 = PWM_MAX;
    if(*ref2 < PWM_MIN) *ref2 = PWM_MIN;

    // mise à jour PWM
    OC1RS = PR2 * (*ref1);
    OC2RS = PR2 * (*ref2);

    // sauvegarde positions
    old_x = x;
    old_y = y;
}
void v_avant(int16_t seuil,int16_t pos_avant_rot){ // faire reset(voir en haut))
while (POS1CNT < seuil+pos_avant_rot || POS2CNT>pos_avant_rot-seuil){// change type 
    OC1RS = PR2/3 ;
    OC2RS = PR2/3 ;
    _LATB8 = 0 ; // roue 1
    _LATB11 = 1 ; //roue 2
}
OC1RS = 1 ;
OC2RS = 1 ;
}

void v_arriere(int16_t seuil,int16_t pos_avant_rot){
while (POS1CNT>pos_avant_rot-seuil || POS2CNT< pos_avant_rot+seuil){
    OC1RS = PR2/3 ;
    OC2RS = PR2/3 ;
    _LATB8 = 1 ;
    _LATB11 = 0 ;//roue 2
}
OC1RS = 1 ;// pour stopper la roue
OC2RS = 1 ;
}


int16_t mettre_to_pos(float cm){
    float inter = cm*68; // 7.906; // 1920/(4.5*2*3.14) 
    int16_t p = (int16_t)inter; 
    return p;

}

void correction_v2(int16_t pos1, int16_t pos2, float ref1, float ref2, float coef,signed int old_err){
    signed int err = 0;
    //int16_t delta_err = past_error - err;
    err = pos1 - pos2 ;
    OC2RS = OC2RS + coef*(err - old_err) + 0*err;
    OC1RS = OC2RS - coef*(err - old_err) - 0*err;   
}


