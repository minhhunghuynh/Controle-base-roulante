#define FCY 6000000     
#include "libpic30.h"   
#include <xc.h> 
#include"configuration.h"
#include"Encoders.h"

int main(void) {
    setup_PWM(); // OCxR peut aller de 1 ? 1199 (d?marre si 0)
    enc_config();
    
    
    OC1RS = PR2*0.6;
    OC2RS = PR2*0.615; // Valeur de base ajustée (surtout pour le démarage))
    int16_t dist_cm = 180;
    
    
    int16_t dist_pos = mettre_to_pos(dist_cm);
    
    int16_t x = 0; //Variable pour les pos de roues
    int16_t y = 0;
    
    
    T1CONbits.TCKPS = 11; // set le prescaler a 1:256
    PR1 = 2343; // 0.01 s
    enc_reset(); // reset les pos
    T1CONbits.TON = 1;  // starts timer 1
    // Temps tot = 256*2343/6M = 0.01s
    
    
    while(1) {
        if(dist_pos < x){  // Condition d'arret
            OC1RS = 1;}
        if(dist_pos < y){
            OC2RS = 1;}
        
        
        if(_T1IF){
            _T1IF = 0; // reset flag
            x = enc_getPos1();
            y = 65535 - enc_getPos2();
       
            correction(x,y);
        }
        
    }
    return 0;
}