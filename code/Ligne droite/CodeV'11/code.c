#define FCY 6000000     
#include "libpic30.h"   
#include <xc.h> 
#include"configuration.h"
#include"Encoders.h"

int main(void) {
    setup_PWM(); // OCxR peut aller de 1 ? 1199 (d?marre si 0)
    enc_config();
    _LATB8 = 1; // pour sens de rotation
    _LATB11 = 1;
    
    // Ajout pin pour allumage qd fil
    _TRISA3 = 0;    // output pin
    _TRISB4 = 0;  // input pin
    _LATA3 = 1; // Allume le pin
    

    
    
    
    enc_reset(); // reset les pos
    float ref1 = 0.55; // valeur autour de laquelle on varie
    float ref2 = 0.4;
    //OC1RS = PR2*ref1;
    //OC2RS = PR2*ref2; // Valeur de base ajustée (surtout pour le démarage))
    OC1RS = 1;
    OC2RS = 1;
    
    float coef = 200;
    
    
    float dist_cm = 240;
    
    
    int16_t dist_pos = mettre_to_pos(dist_cm);
    
    int16_t x = 0; //Variable pour les pos de roues
    int16_t y = 0;
    
    float err = 0; // signed = signed int 
    int pwm1 = 0;
    int pwm2 = 0;
  
    int16_t etape = 10;
    
    // Partie acceleration :
    float acc = 0.01;
    T3CONbits.TCKPS = 01; // set le prescaler a 1:8
    int16_t timer = 24000; // Timer de 0.004s * 8 = 0.032

    
    //T1CONbits.TCKPS = 01; // set le prescaler a 1:8
    // taille de PR1 = 15bits => valeur max = 32 768
    PR1 = 30000;//3000; //6000 => timer de t = 0.001s 
    T1CONbits.TON = 1;  // starts timer 1
    
    float seuil = 9.95  *180*3;
    
    
    // terme intégrale
    float integr = 0;
    signed old_err = 0;
    int rep = 0;    
    
    
    int deja =0;
    while(1) {
        if (_RB4) {
            if (deja == 0){
            deja = deja+1;
            etape = 0;}}
        if (!_RB4) {
            etape = 10;
            enc_reset();
            integr = 0;
            OC1RS = 1;
            OC2RS = 1;
            __delay_ms(4000);
            deja = 0;
        } 
        
        if(dist_pos< enc_getPos1()){  // Condition d'arret
            OC1RS = 1;}
        if(dist_pos < enc_getPos2()){
            OC2RS = 1;
            etape = 3;
            }
        
        if (etape == 0){
            acceleration(acc,timer);
            etape = 1;
        }
        if (etape == 1){
            if (_T1IF){
                _T1IF = 0;
                //float corr =0;// 16*dist_cm/120;
                x = enc_getPos1();
                y = enc_getPos2();
                err = x - y;
                float ab_err = err;
                float ab_old_err = old_err;
                if (err < 0){ab_err = -err;}
                if (old_err < 0 ){ab_old_err = -old_err;}
                
                if (ab_err >= ab_old_err){integr =integr + err;
                rep = rep +1;}
                if (ab_err < ab_old_err){integr = integr - rep*(old_err - err);
                rep = 0;}
                
                old_err = err;
                
                //if (x > (int16_t)(2*dist_pos/3)){integr = 0;}
                
                float pwm2 = ref2*PR2 + coef*0.25*err + 2*integr;
                float pwm1 = ref1*PR2 - coef*0.25*err - 2*integr;    
                if (pwm1 > 1199){pwm1 = 1199;}
                if (pwm1<1){pwm1 = 1;}
                if (pwm2 > 1199){pwm2 = 1199;}
                if (pwm2<1){pwm2 = 1;}
                OC1RS = pwm1;
                OC2RS = pwm2;
                //correction(ref1,ref2,coef,corr);}
            
            
            }}
        
        if (etape == 2){
            int16_t r = (int16_t) seuil ; 
            v_avant(r);
            etape = 3;
            }
        
    }
    return 0;
}
