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

void correction (float ref1,float ref2, float coef, float corr){
    int x = enc_getPos1();
    int y = enc_getPos2();
    float err = x - y;
     
    
    float pwm2 = ref2*PR2 + coef*1.3*err;
    float pwm1 = ref1*PR2 - coef*1.3*err;    if (pwm1 > 1199){pwm1 = 1199;}
    if (pwm1<1){pwm1 = 1;}
    if (pwm2 > 1199){pwm2 = 1199;}
    if (pwm2<1){pwm2 = 1;}
    OC1RS = pwm1;
    OC2RS = pwm2;
}


void correction2(float err, float old_err,float coef,float coef_int, int *rep, float *integr){
    
    float ref2 = 0.5;
    float ref1 = 0.5;
    float a_err;
    float a_old_err;
    _LATB3 = 1;
    if (_T4IF){
        _T4IF = 0;
        _LATB3 = 0;
        a_err = absolu(err);
        a_old_err = absolu(old_err);
        if (a_err >= a_old_err){ // si l'erreur a augmentée ou est restée constante : 
            *integr =*integr + err;
            *rep = *rep +1;}

        if (a_err < a_old_err){*integr = *integr - *rep *(old_err - err); // retire si l erreur a ete corigee
            *rep = 0;}

        float pwm2 = ref2*PR2 + coef*0.5*err + coef_int*0.35*(*integr);
        float pwm1 = ref1*PR2 - coef*0.5*err - coef_int*0.35*(*integr);    
        if (pwm1 > 1199){pwm1 = 1199;}
        if (pwm1<1){pwm1 = 1;}
        if (pwm2 > 1199){pwm2 = 1199;}
        if (pwm2<1){pwm2 = 1;}
        OC1RS = pwm1;
        OC2RS = pwm2;}}





void correction_rot(int16_t x, int16_t y ,float ref1,float ref2, float coef){
    x = enc_getPos1();
    y = enc_getPos2();
    signed err = x - y;
    
    float pwm2 = ref2*PR2 + coef*err;
    float pwm1 = ref1*PR2 - coef*err;
    if (pwm1 > 1199){pwm1 = 1199;}
    //if (pwm1<0){pwm1 = 0;}
    if (pwm2 > 1199){pwm2 = 1199;}
    //if (pwm2<0){pwm2 = 0;}
    OC1RS = pwm1;
    OC2RS = pwm2;
}


void v_avant(uint16_t seuil){ // faire reset(voir en haut))
    QEI1CONbits.SWPAB = 1;
    QEI2CONbits.SWPAB = 1; // Pour compter dans le bon sens 
    _LATB8 = 0 ; // roue 1
    _LATB11 = 1 ; //roue 2
    uint16_t pos1 = 0;
    uint16_t pos2 = 0;
    OC1RS = PR2*0.3; //roue gauche(avec bille avant))
    OC2RS = PR2*0.3; // roue droite
while (pos1 < seuil || pos2<seuil){// change type && POS2CNT > seuil
    
    pos1 = enc_getPos1();
    pos2 = enc_getPos2();
    if(pos1 > seuil){OC1RS = 1;}
    if(pos2 > seuil){OC2RS = 1;}
    
    //if (_T1IF){
      //  _T1IF = 0;
    correction_rot(pos1,pos2,0.3,0.3,10); // ref1 ref2 coef
    }//}
OC1RS = 1 ;
OC2RS = 1 ;
} 



void v_arriere(int16_t seuil){

    QEI1CONbits.SWPAB = 0; 
    QEI2CONbits.SWPAB = 0;
    _LATB8 = 1 ;//roue 1
    _LATB11 = 0 ;//roue 2
    uint16_t pos1 = 0;
    uint16_t pos2 = 0;
    OC1RS = PR2*0.3; //roue gauche(avec bille avant))
    OC2RS = PR2*0.3; // roue droite
while (pos1 < seuil || pos2<seuil){// change type && POS2CNT > seuil
    
    pos1 = enc_getPos1();
    pos2 = enc_getPos2();
    if(pos1 > seuil){OC1RS = 1;}
    if(pos2 > seuil){OC2RS = 1;}
    
    //if (_T1IF){
      //  _T1IF = 0;
    correction_rot(pos1,pos2,0.3,0.3,10); // ref1 ref2 coef
    }//}
    
OC1RS = 1 ;// pour stopper la roue
OC2RS = 1 ;
}



int16_t mettre_to_pos(float cm){
    // Ajout de la l'erreur mesurée experimentallement sur 120cm
    cm = cm + (cm *3/120) + (cm * 1/120);
    float inter = cm*67.906; // 1920/(4.5*2*3.14)
    // Le casting a été vérifié en C
    int16_t p = (int16_t)inter; 
    return p;
}

void acceleration(float acc,int16_t timer,int16_t* etape, int dist_acc){
    float pwm1 = 1;
    float pwm2 = 1;
    float ref_pwm = 0.2;
    int x;
    int y;
    float err;
    int coef  = 100;
    //PR4 = timer; 
    //T4CONbits.TON = 1;
    while (ref_pwm < 0.5 & ref_pwm < 0.5){
        if(_T4IF){
            _T4IF = 0;
            
            ref_pwm = ref_pwm + acc;
        }
        
        correction(ref_pwm,ref_pwm,coef,0);
      if(enc_getPos1()> dist_acc*0.5 & enc_getPos2()> dist_acc*0.5){
           *etape = 3 ;
            return;
        } 
    }
    *etape = 2 ;
    return;
}

void deceleration(float dec, float ref_pwm, int coef, int dist_pos, float minimum,int16_t* etape){
    //paramétrisation 
    
   
    
    while(dist_pos> enc_getPos1() && dist_pos > enc_getPos2()){
    if(_T4IF){
        _T4IF = 0;
        ref_pwm = ref_pwm - dec;
        if(ref_pwm<= minimum) {ref_pwm= minimum ;} // éviter d'avoir une vitesse ref nulle
        // avant d'avoir atteint la distance finale
        correction(ref_pwm,ref_pwm,coef,0);
    } 
    }

 //condition pour l'arrt total
    
    while(OC1RS!=1 || OC2RS !=1){
   if(dist_pos< enc_getPos1()){  
            OC1RS = 1;}
   if(dist_pos < enc_getPos2()){
            OC2RS = 1;}
    }
    //Condition de retour  attente instruction
    
    *etape = 0; // retour  l'etape d'attente des ordres
    return ;
    
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


