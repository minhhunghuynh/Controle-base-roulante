/*
 * File:   mouvement.c
 * Author: Raphaël Aouras
 *
 * Created on 8 mai 2026, 15:36
 */

#include "Encoders.h"
#define FCY 40000000 
#include "xc.h"



void correction (float ref1,float ref2, float coef, float corr,int16_t dist_r1,int16_t dist_r2){
    int x = enc_getPos1(dist_r1);
    int y = enc_getPos2(dist_r2);
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
    _LATB3 = 1;
    if (_T4IF){
        _T4IF = 0;
        _LATB3 = 0;
        
        *integr = *integr + coef_int*0.35*(coef_int*(err + old_err));

        float pwm2 = ref2*PR2 + coef*0.5*err + (*integr);
        float pwm1 = ref1*PR2 - coef*0.5*err - (*integr);    
        if (pwm1 > 1199){pwm1 = 1199;}
        if (pwm1<1){pwm1 = 1;}
        if (pwm2 > 1199){pwm2 = 1199;}
        if (pwm2<1){pwm2 = 1;}
        OC1RS = pwm1;
        OC2RS = pwm2;}}


void correction_rot(int16_t x, int16_t y ,float ref1,float ref2, float coef,int16_t dist_r1, int16_t dist_r2){
    x = enc_getPos1(dist_r1);
    y = enc_getPos2(dist_r2);
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
    
    pos1 = POS1CNT;
    pos2 = POS2CNT;
    if(pos1 > seuil){OC1RS = 1;}
    if(pos2 > seuil){OC2RS = 1;}
    
    //if (_T1IF){
      //  _T1IF = 0;
    correction_rot(pos1,pos2,0.3,0.3,10,seuil,seuil); // ref1 ref2 coef
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
    
    pos1 = POS1CNT;
    pos2 = POS2CNT;
    if(pos1 > seuil){OC1RS = 1;}
    if(pos2 > seuil){OC2RS = 1;}
    
    //if (_T1IF){
      //  _T1IF = 0;
    correction_rot(pos1,pos2,0.3,0.3,10,seuil,seuil); // ref1 ref2 coef
    }//}
    
OC1RS = 1 ;// pour stopper la roue
OC2RS = 1 ;
}


void acceleration(float acc,int16_t timer,int16_t* etape, int dist_acc,int16_t dist_r1, int16_t dist_r2){
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
        
        correction(ref_pwm,ref_pwm,coef,0,dist_r1,dist_r2);
      if(enc_getPos1(dist_r1)> dist_acc*0.5 & enc_getPos2(dist_r2)> dist_acc*0.5){
           *etape = 3 ;
            return;
        } 
    }
    *etape = 2 ;
    return;
}

void deceleration(float dec, float ref_pwm, int coef, int dist_pos, float minimum,int16_t* etape,int16_t dist_r1, int16_t dist_r2){
    //paramétrisation 
    
   
    
    while(dist_pos> enc_getPos1(dist_r1) && dist_pos > enc_getPos2(dist_r2)){
    if(_T4IF){
        _T4IF = 0;
        ref_pwm = ref_pwm - dec;
        if(ref_pwm<= minimum) {ref_pwm= minimum ;} // éviter d'avoir une vitesse ref nulle
        // avant d'avoir atteint la distance finale
        correction(ref_pwm,ref_pwm,coef,0,dist_r1,dist_r2);
    } 
    }

 //condition pour l'arrt total
    
    while(OC1RS!=1 || OC2RS !=1){
   if(dist_pos< enc_getPos1(dist_r1)){  
            OC1RS = 1;}
   if(dist_pos < enc_getPos2(dist_r2)){
            OC2RS = 1;}
    }
    //Condition de retour  attente instruction
    
    *etape = 0; // retour  l'etape d'attente des ordres
    return ;
    
}