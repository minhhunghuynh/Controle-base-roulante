#define FCY 6000000     
#include "libpic30.h"   
#include <xc.h> 
#include"configuration.h"
#include"Encoders.h"

int main(void) {
    setup_PWM(); // OCxR peut aller de 1 � 1199 (d�marre si 0)
    enc_config();
    
    
    OC1RS = PR2*0.45;
    OC2RS = PR2*0.55; // M�me � PR2 max il ne demarre pas toujours seul
    
 
    _TRISB4 = 0;
    T1CONbits.TCKPS = 11; // set le prescaler a 1:256
    PR1 = 3200; //0.13s 
    T1CONbits.TON = 1;  // starts timer 1
    int r = 0;
    int n = 0;
    
    while(r == 0){ //=> 6s pour lancer la roue (pr pas trop grand ecart) 
        if (n == 50){
            r = 1;
        }
        if (_T1IF){
            
            n = n +1;
            _T1IF = 0;}
    }
    _LATB4 = 1; // allume LED
    
    int16_t x = 0;
    int16_t y = 0;
    int16_t rep = 0;
    
    
    PR1 = 2343; // 0.01 s
    enc_reset(); // reset les pos
    // Temps tot = 256*2343/6M = 0.01s
    
    
    while(1) {
        
        if(_T1IF){
        _T1IF = 0; // reset flag
        //if (_LATB4 == 1){_LATB4 = 0;} permet de verifier avec picoscope que code bon
        //else if (_LATB4 == 0){_LATB4 = 1;}
        rep = rep + 1;
        if (rep == 1000){ //pour eviter un overflow mais ne devrait pas arriver sauf si trjet plus grand que 15m
            rep = 0;
            enc_reset();
        }
       x = enc_getPos1();
       y = enc_getPos2();
       
       correction(x,y);
        
        
        }
        
    }
    return 0;
}