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

void correction( float ref1,float ref2, float coef, float corr){
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



void v_arriere(int16_t seuil,int16_t pos_avant_rot){
    QEI1CONbits.SWPAB = 0; 
    QEI2CONbits.SWPAB = 0;
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
    // Ajout de la l'erreur mesurée experimentallement sur 120cm
    cm = cm + (cm *3/120) + (cm * 1/120);
    float inter = cm*67.906; // 1920/(4.5*2*3.14)
    // Le casting a été vérifié en C
    int16_t p = (int16_t)inter; 
    return p;
}

int16_t acceleration(float acc, float ref_pwm, int coef){
    int16_t etape = 0;
    if(_T3IF){
        _T3IF = 0;
        ref_pwm = ref_pwm + acc;
        }
    correction(ref_pwm,ref_pwm,coef,0);
    if (ref_pwm > 0.5 | ref_pwm > 0.5){ etape = 1;}
        return etape;
}

float absolu(float value){
    if (value < 0){value = -value;}
    return value;
               
}



void initAdc1(void)
{
 //AD1CON1 control register 1 of ADC1
            
        AD1CON1bits.FORM = 0b00; // choix de la division (de 0 a 4048)
        AD1CON1bits.ASAM = 1; // auto sample active
        AD1CON1bits.AD12B = 0; // Convertisseur sur 12 bits
                 
        AD1CON3bits.ADRC = 0; // choix de l'horloge (ici celle principale)
        AD1CON3bits.ADCS = 0; // sorte de prescaler sur l'horloge pour Tad

        AD1CON2bits.CSCNA = 0; // ?
        
        AD1CON2bits.CHPS = 0; // utilise que CH0
        AD1CSSLbits.CSS0 = 1; // Choix de l'entrée (AN0)
        AD1PCFGLbits.PCFG0 = 0; // AN0 en mode analogique
        
        AD1CON1bits.ADON = 1; // Activation de l'ADC
        // choix du mode d'activation, 
        // 0 = manuel en remettant le bit AD1CON1bits.DONE à 0
        // 2 = défini par le timer3 pour sample à chaque fois
        AD1CON1bits.SSRC = 2;
}

void ech_signal(float nb_ech, float DT, int *liste){
    int16_t e = 0;
    DT = DT * FCY * 0.001; // Transformation en cycle d'horloge
    float dt = DT/nb_ech; // durée en tre chaque echantillons
    
    T3CONbits.TCKPS = 00; // Prescaler 1:1 
    PR3 =(int16_t) dt;
    T3CONbits.TON = 1; // Démarage du timer
    
    while(e < (int16_t)nb_ech){
        if (AD1CON1bits.DONE) {
        AD1CON1bits.DONE = 0;
         //_T3IF = 0;
        liste[e] =(signed int) ADC1BUF0;
        e = e+1;
        
        }
    }
    
}
//#define N 66   // nombre d'échantillons FIXE

int correl(signed int *list, int nb_ech, int *temps)
{
    // Tables pré-calculées (66 valeurs pour 20kHz durant 3.3ms)
    /*
    float cos_800[66] = {
       1.0000, 0.9686, 0.8763, 0.7290, 0.5358, 0.3090, 0.0628, -0.1874, -0.4258, -0.6374, -0.8090, -0.9298, -0.9921, -0.9921, -0.9298, -0.8090, -0.6374, -0.4258, -0.1874, 0.0628, 0.3090, 0.5358, 0.7290, 0.8763, 0.9686, 1.0000, 0.9686, 0.8763, 0.7290, 0.5358, 0.3090, 0.0628, -0.1874, -0.4258, -0.6374, -0.8090, -0.9298, -0.9921, -0.9921, -0.9298, -0.8090, -0.6374, -0.4258, -0.1874, 0.0628, 0.3090, 0.5358, 0.7290, 0.8763, 0.9686, 1.0000, 0.9686, 0.8763, 0.7290, 0.5358, 0.3090, 0.0628, -0.1874, -0.4258, -0.6374, -0.8090, -0.9298, -0.9921, -0.9921, -0.9298, -0.8090
    };

    float sin_800[66] = {
        0.0000, 0.2487, 0.4818, 0.6845, 0.8443, 0.9511, 0.9980, 0.9823, 0.9048, 0.7705, 0.5878, 0.3681, 0.1253, -0.1253, -0.3681, -0.5878, -0.7705, -0.9048, -0.9823, -0.9980, -0.9511, -0.8443, -0.6845, -0.4818, -0.2487, -0.0000, 0.2487, 0.4818, 0.6845, 0.8443, 0.9511, 0.9980, 0.9823, 0.9048, 0.7705, 0.5878, 0.3681, 0.1253, -0.1253, -0.3681, -0.5878, -0.7705, -0.9048, -0.9823, -0.9980, -0.9511, -0.8443, -0.6845, -0.4818, -0.2487, -0.0000, 0.2487, 0.4818, 0.6845, 0.8443, 0.9511, 0.9980, 0.9823, 0.9048, 0.7705, 0.5878, 0.3681, 0.1253, -0.1253, -0.3681, -0.5878};
    float cos_1200[66] = {
        1.0000, 0.9298, 0.7290, 0.4258, 0.0628, -0.3090, -0.6374, -0.8763, -0.9921, -0.9686, -0.8090, -0.5358, -0.1874, 0.1874, 0.5358, 0.8090, 0.9686, 0.9921, 0.8763, 0.6374, 0.3090, -0.0628, -0.4258, -0.7290, -0.9298, -1.0000, -0.9298, -0.7290, -0.4258, -0.0628, 0.3090, 0.6374, 0.8763, 0.9921, 0.9686, 0.8090, 0.5358, 0.1874, -0.1874, -0.5358, -0.8090, -0.9686, -0.9921, -0.8763, -0.6374, -0.3090, 0.0628, 0.4258, 0.7290, 0.9298, 1.0000, 0.9298, 0.7290, 0.4258, 0.0628, -0.3090, -0.6374, -0.8763, -0.9921, -0.9686, -0.8090, -0.5358, -0.1874, 0.1874, 0.5358, 0.8090

    };

    float sin_1200[66] = {
        0.0000, 0.3681, 0.6845, 0.9048, 0.9980, 0.9511, 0.7705, 0.4818, 0.1253, -0.2487, -0.5878, -0.8443, -0.9823, -0.9823, -0.8443, -0.5878, -0.2487, 0.1253, 0.4818, 0.7705, 0.9511, 0.9980, 0.9048, 0.6845, 0.3681, 0.0000, -0.3681, -0.6845, -0.9048, -0.9980, -0.9511, -0.7705, -0.4818, -0.1253, 0.2487, 0.5878, 0.8443, 0.9823, 0.9823, 0.8443, 0.5878, 0.2487, -0.1253, -0.4818, -0.7705, -0.9511, -0.9980, -0.9048, -0.6845, -0.3681, -0.0000, 0.3681, 0.6845, 0.9048, 0.9980, 0.9511, 0.7705, 0.4818, 0.1253, -0.2487, -0.5878, -0.8443, -0.9823, -0.9823, -0.8443, -0.5878
    };
    */
    float cos_800[66] = 
     {1.0000, 0.8763, 0.5358, 0.0628, -0.4258, -0.8090, -0.9921, -0.9298, -0.6374, -0.1874, 0.3090, 0.7290, 0.9686, 0.9686, 0.7290, 0.3090, -0.1874, -0.6374, -0.9298, -0.9921, -0.8090, -0.4258, 0.0628, 0.5358, 0.8763, 1.0000, 0.8763, 0.5358, 0.0628, -0.4258, -0.8090, -0.9921, -0.9298, -0.6374, -0.1874, 0.3090, 0.7290, 0.9686, 0.9686, 0.7290, 0.3090, -0.1874, -0.6374, -0.9298, -0.9921, -0.8090, -0.4258, 0.0628, 0.5358, 0.8763, 1.0000, 0.8763, 0.5358, 0.0628, -0.4258, -0.8090, -0.9921, -0.9298, -0.6374, -0.1874, 0.3090, 0.7290, 0.9686, 0.9686, 0.7290, 0.3090};
    float sin_800[66] =
        {0.0000, 0.4818, 0.8443, 0.9980, 0.9048, 0.5878, 0.1253, -0.3681, -0.7705, -0.9823, -0.9511, -0.6845, -0.2487, 0.2487, 0.6845, 0.9511, 0.9823, 0.7705, 0.3681, -0.1253, -0.5878, -0.9048, -0.9980, -0.8443, -0.4818, -0.0000, 0.4818, 0.8443, 0.9980, 0.9048, 0.5878, 0.1253, -0.3681, -0.7705, -0.9823, -0.9511, -0.6845, -0.2487, 0.2487, 0.6845, 0.9511, 0.9823, 0.7705, 0.3681, -0.1253, -0.5878, -0.9048, -0.9980, -0.8443, -0.4818, -0.0000, 0.4818, 0.8443, 0.9980, 0.9048, 0.5878, 0.1253, -0.3681, -0.7705, -0.9823, -0.9511, -0.6845, -0.2487, 0.2487, 0.6845, 0.9511};
    float cos_1200[66] = 
        {1.0000, 0.7290, 0.0628, -0.6374, -0.9921, -0.8090, -0.1874, 0.5358, 0.9686, 0.8763, 0.3090, -0.4258, -0.9298, -0.9298, -0.4258, 0.3090, 0.8763, 0.9686, 0.5358, -0.1874, -0.8090, -0.9921, -0.6374, 0.0628, 0.7290, 1.0000, 0.7290, 0.0628, -0.6374, -0.9921, -0.8090, -0.1874, 0.5358, 0.9686, 0.8763, 0.3090, -0.4258, -0.9298, -0.9298, -0.4258, 0.3090, 0.8763, 0.9686, 0.5358, -0.1874, -0.8090, -0.9921, -0.6374, 0.0628, 0.7290, 1.0000, 0.7290, 0.0628, -0.6374, -0.9921, -0.8090, -0.1874, 0.5358, 0.9686, 0.8763, 0.3090, -0.4258, -0.9298, -0.9298, -0.4258, 0.3090};
    float sin_1200[66] =
        {0.0000, 0.6845, 0.9980, 0.7705, 0.1253, -0.5878, -0.9823, -0.8443, -0.2487, 0.4818, 0.9511, 0.9048, 0.3681, -0.3681, -0.9048, -0.9511, -0.4818, 0.2487, 0.8443, 0.9823, 0.5878, -0.1253, -0.7705, -0.9980, -0.6845, -0.0000, 0.6845, 0.9980, 0.7705, 0.1253, -0.5878, -0.9823, -0.8443, -0.2487, 0.4818, 0.9511, 0.9048, 0.3681, -0.3681, -0.9048, -0.9511, -0.4818, 0.2487, 0.8443, 0.9823, 0.5878, -0.1253, -0.7705, -0.9980, -0.6845, -0.0000, 0.6845, 0.9980, 0.7705, 0.1253, -0.5878, -0.9823, -0.8443, -0.2487, 0.4818, 0.9511, 0.9048, 0.3681, -0.3681, -0.9048, -0.9511};


    float cor_f1 = 0;float sor_f1 = 0;
    float cor_f2 = 0;float sor_f2 = 0;
    float sum = 0;

    for(int i = 0; i < nb_ech; i++) // ici
    {
        float x = (float)list[i] - 512;   // enlever offset ADC
        x = x;
        sum = sum + (absolu(x));
        

        cor_f1 =cor_f1 +  (x * cos_800[i]);
        sor_f1 = sor_f1 +  (x * sin_800[i]);

        cor_f2 = cor_f2 +  (x * cos_1200[i]);
        sor_f2 = cor_f2 +  (x * sin_1200[i]);
    }

    float corel_f1 = cor_f1*cor_f1 + sor_f1*sor_f1;
    float corel_f2 = cor_f2*cor_f2 + sor_f2*sor_f2;
    sum = sum*sum;
    //sum = 0.707*sum;
    //int16_t ref = 23;
    //if (corel_f1 > 0.2*sum){ ref = 0;}
    //if (corel_f1 == 0.2*sum){ref = 1;}
    //if (corel_f1 < 0.2*sum){ ref = 2;}
    //if (corel_f1 < corel_f2){ ref = 3;}
    //if (corel_f1 > corel_f2){ ref = 4;}
    
        //sum =0;
    // Détection
    if(corel_f1 > corel_f2 && corel_f1 > 0.3 * sum)
    {return 0;}   // 800 Hz détecté

    if(corel_f2 > corel_f1 && corel_f2 > 0.01 * sum)
    { return 1;}   // 1200 Hz détecté

    return 2;       // rien détecté
}