#define FCY 6000000     
#include "libpic30.h"   
#include <xc.h> 
#include"configuration.h"
#include"Encoders.h"

int main(void) {
    setup_UART();
    setup_PWM(); // OCxR peut aller de 1 à 1199 (démarre si 0)
    enc_config();
    enc_reset(); 
    
    OC1RS = PR2/2;
    _TRISB2 = 0;
    
    int16_t x = 0;
    int16_t y = 0;
    uint8_t lsb = 0;
    uint8_t msb = 0;
    uint16_t r = 0;
    _LATB2 = 1;
    
    T1CONbits.TCKPS = 11; // set le prescaler a 1:256
    PR1 = 32768; // durée max
    // Temps tot = 256*32768/40M ~ 0.2s
    // A été verifié par latb2 
    
    T1CONbits.TON = 1;  // starts timer1
    
    x = enc_getPos1();
    
    
    while(1) {
        
        if(_T1IF){
        _T1IF = 0; // reset flag
        _LATB2 = 0;
        
        y = enc_getPos1();
        
        r = (y-x); //32,768; // toujours a 0 car on divise par énormément
        x = y;
        lsb = r; // donne les 8 dernier bits
        msb = (r >>8) & 0xFF; // Donne les 8 premiers bits
        
        
        while (U1STAbits.UTXBF) {} // Attend buffer transmission finit
        U1TXREG = lsb; // 0 si LSB
        _LATB2 = 1;
         while (U1STAbits.UTXBF) {} // Attend buffer transmission finit
        U1TXREG = msb; // 0 si LSB
        
        }
        
    }
    return 0;
}