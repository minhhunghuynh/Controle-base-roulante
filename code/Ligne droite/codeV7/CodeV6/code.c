#define FCY 6000000     
#include "libpic30.h"   
#include <xc.h> 
#include"configuration.h"
#include"Encoders.h"

#include <xc.h>

#define KP 0.02
#define KI 0.005

#define PWM_MAX 0.95
#define PWM_MIN 0.05

#define INTEG_MAX 100
#define INTEG_MIN -100


int main(void) {
    setup_PWM(); // OCxR peut aller de 1 ? 1199 (d?marre si 0)
    enc_config();
    _LATB8 = 1; // output pour test
    _LATB11 = 1;
    enc_reset(); // reset les pos
    float ref1 = 0.5; // valeur autour de laquelle on varie
    float ref2 = 0.5;
    OC1RS = PR2*ref1;
    OC2RS = PR2*ref2; // Valeur de base ajustée (surtout pour le démarage))
    
    
    float coef = 0.5;
    
    
    int16_t dist_cm = 240;
    
    
    int16_t dist_pos = mettre_to_pos(dist_cm);
    
    int16_t x = 0; //Variable pour les pos de roues
    int16_t y = 0;
  
    T1CONbits.TCKPS = 11; // set le prescaler a 1:256
    PR1 = 1600;//2343; // 0.01 s
    T1CONbits.TON = 1;  // starts timer 1
    // Temps tot = 256*2343/6M = 0.01s
    
    
    signed long old_err = 0;
    
    
    while(1) {
        if(dist_pos < x){  // Condition d'arret
            OC1RS = 1;}
        if(dist_pos < y){
            OC2RS = 1;}
       
       
        
        if(_T1IF){
            _T1IF = 0; // reset flag
            x = enc_getPos1();
            y = enc_getPos2();

            correction_PI(x, y, &ref1, &ref2);
             old_err = x - y;
}
            
       }
    
    return 0;
}
