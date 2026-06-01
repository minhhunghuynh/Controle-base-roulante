#define FCY 6000000     
#include "libpic30.h"   
#include <xc.h> 
#include"configuration.h"
#include"Encoders.h"

int main(void) {
    // Code qui devrait rencoyer la position de la roue 1 quand on envoie "p"
    setup_UART();
    setup_PWM(); // OCxR peut aller de 1 à 1199 (démarre si 0)
    enc_config();
    
    _TRISB2 = 0;
    enc_reset();
    // Setup du timer 1, on le veux tres long
    int16_t x = 0;
    int16_t y = 0;
    uint8_t lsb = 0;
    uint8_t msb = 0;
    
    _LATB2 = 1;
    while(1) {
        // x = enc_getPos1();
        y = enc_getPos1();
        //lsb = y & 0xFF; // Donne les 8 derniers bit
        msb = (y >>8) & 0xFF; // Donne les 8 premiers bits
        lsb = y;
        _LATB2 = 0;
        
        while (U1STAbits.UTXBF) {} // Attend buffer transmission finit
        U1TXREG = lsb; // 0 si LSB
        _LATB2 = 1;
        while (U1STAbits.UTXBF) {}
        U1TXREG = msb; // 1 si MSB
        
        
    }
    return 0;
}