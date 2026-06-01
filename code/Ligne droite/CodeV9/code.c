#define FCY 6000000     
#include "libpic30.h"   
#include <xc.h> 
#include"configuration.h"
#include"Encoders.h"

int main(void) {
    setup_PWM(); // OCxR peut aller de 1 ? 1199 (d?marre si 0)
    enc_config();
    _LATB8 = 1; // output pour test
    _LATB11 = 1;
    enc_reset(); // reset les pos
    float ref1 = 0.5; // valeur autour de laquelle on varie
    float ref2 = 0.5;
    OC1RS = PR2*ref1;
    OC2RS = PR2*ref2; // Valeur de base ajust�e (surtout pour le d�marage))
    
    
    float coef = 200;
    
    
    int16_t dist_cm = 240;
    
    
    int16_t dist_pos = mettre_to_pos(dist_cm);
    
    int16_t x = 0; //Variable pour les pos de roues
    int16_t y = 0;
    
    signed err = 0; // signed == signed int
    int16_t pwm1 = 0;
    int16_t pwm2 = 0;
  
    //T1CONbits.TCKPS = 01; // set le prescaler a 1:8
    // taille de PR1 = 15bits => valeur max = 32 768
    PR1 = 6000; // timer de t = 0.001s 
    T1CONbits.TON = 1;  // starts timer 1
    
    
   
    
    
    
    while(1) {
        if(dist_pos < enc_getPos1()){  // Condition d'arret
            OC1RS = 1;}
        if(dist_pos < enc_getPos2()){
            OC2RS = 1;}
       
       
        
        if(_T1IF){
        _T1IF = 0; // reset flag  
        correction(x,y,ref1,pwm1,pwm2,coef,err);  
       }
        
    }
    return 0;
}

