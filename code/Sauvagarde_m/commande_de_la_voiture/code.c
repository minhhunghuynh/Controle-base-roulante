#define FCY 6000000     
#include "libpic30.h"   
// DSPIC33FJ128MC802 Configuration Bit Settings
#include "configuration.h"
#include <xc.h>
#include"Encoders.h"


int main(void) {
setup_PWM(); // OCxR peut aller de 1 a 1199 (demarre si 0)
    enc_config(); // setup de la prise de position
    enc_reset(); // reset les pos (pas nécessaire)
    
    // Sens de rotation par defaut : vers l'avant
    _LATB8 = 1;
    _LATB11 = 1;
    
    
    // Allumage qd fil branché
    _TRISA3 = 0;    // output pin
    _TRISB4 = 0;  // input pin
    _LATA3 = 1; // Allume le pin
    //int deja =0;
    
    // reference de vitesse de voiture
    float ref1 = 0.5; 
    float ref2 = 0.5;
    OC1RS = 1;
    OC2RS = 1;
    
    // Coefficient de correction (proportionnelle)
    float coef = 200;
    
    // Ordre de distance 
    float dist_cm = 20;
    int16_t dist_pos = mettre_to_pos(dist_cm);
    
    
    int16_t x = 0; //Variable pour les pos de roues
    int16_t y = 0;
    float err = 0;
    
    //Creation de pointeur etape
    int16_t val_etape = 1;
    int16_t* etape = &val_etape; 
   
    
    
    // Partie acceleration :
    float acc = 0.005;
    float start_pwm = 0.2;
    int coef_corr_acc = 100;
    T3CONbits.TCKPS = 01; // set le prescaler a 1:8
    int16_t timer = 24000; // Timer de 0.004s * 8 = 0.032
    PR3 = timer; // demarage timer d'acc
    T3CONbits.TON = 1;
    
    
    //Partie deceleration 
    int16_t dist_decel = dist_pos*0.8 ; // distance de début de décélération
    
   
    // Timer de la correction
    // taille de PR1 = 15bits => valeur max = 32 768
    PR1 = 3000; //6000 => timer de t = 0.001s 
    T1CONbits.TON = 1;  // starts timer 1
    
    // Constantes de rotation
    float nb_degre = 90;
    float seuil = 9.95  *nb_degre;
    
    
    // terme intégrale
    float integr = 0;
    signed old_err = 0;
    int rep = 0;    
    float coef_int = 1;

    
    //int deja =0;
    while(1) {
        if (*etape == 1){
            acceleration(acc,timer,etape,dist_pos);
           
        }
       if (*etape == 2){
           
           if(dist_decel< enc_getPos1() & dist_decel< enc_getPos2()){
                *etape = 3;
                
            x = enc_getPos1();
            y = enc_getPos2();
            err = x-y;
            correction2(err, old_err, coef, coef_int,&rep,&integr);
            old_err = err;
            
            }
            }

        if (*etape == 3){
            deceleration(0.005, 0.5, 20, dist_pos, 0.25, etape);
        }
        if (*etape == 4){
            enc_reset();
            int16_t r = (int16_t) seuil ; 
            v_arriere(r);
            *etape = 5;
            }
        
    }
    return 0;
}