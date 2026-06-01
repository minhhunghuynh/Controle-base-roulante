#define FCY 40000000    
#include "libpic30.h"   
#include <xc.h> 
#include"configuration.h"
#include"Encoders.h"

/*
Code divisé en 4 parties :
    - Etape 0 : Attente du start bit (lis sur l'ADC et Conversion)
    - Etape 1 : Effectue les mesures du reste de la trame sur l'ADC
    - Etape 2 : Convertit ces mesure vers bit 0 ou 1 (2 si pas au dessus de seuil)
    - Etape 3 : Envoie en UART du pin RB4 la trame (sans le start bit0)
*/




int main(void) {
    
    setup_UART();
    setup_PWM(); // OCxR peut aller de 1 a 1199 (demarre si 0)
    enc_config(); // setup de la prise de position
    enc_reset(); // reset les pos
    initAdc1(); // Initialisation de l'ADC(10bit,0-1024,mesures via timer3,...)
    
    // Pour test
    _TRISB2 = 0;    // output pin
    _LATB2 = 0;   


    // Paramètre d'échantillonnage :
    float t_ech = 2.6; // En milisencode
    float f_ech = 10000;
    float n_ech = t_ech * 0.001*f_ech;

    // Stockage des echantillonnnages
    float liste[(signed int)n_ech]; 
    signed int L1[(int)n_ech];
    signed int L2[(int)n_ech];
    signed int L3[(int)n_ech];
    signed int L4[(int)n_ech];
    signed int L5[(int)n_ech];
    signed int L6[(int)n_ech];
    signed int L7[(int)n_ech];
    signed int L8[(int)n_ech];
    signed int L9[(int)n_ech];
    signed int L10[(int)n_ech];
    signed int L11[(int)n_ech];


    // Timer
    T1CONbits.TCKPS = 01; // set le prescaler a 1:8
    PR1= 4*0.001*FCY / 8; // On veut 4 ms pour apres la detection du start bit
    // Vecteur temps
    float temps[(int)n_ech];
    for (int t=0 ; t< (int)2*n_ech ; ++t){
    temps[t] = t * (1/(f_ech*FCY)) ; }

    // Stockage  de la trame finale
    int16_t lst[11];

    // Seuil de correlation
    float rif = 88604672; // Valeur arbitraire pour le seuil de correl ((512*26)^2/2)
    
    // Paramettre etape 0 :
    uint8_t cnt = 0; // Compte le nombre de mesure successive au bit 0 pour le start bit
    int etape = 0;

    // Paramettre etape 1 :
    int i = 1;
    
    // Parametre UART
    int16_t y = 10;
    
  
    // Boucle infinie
    while(1) {
        if (etape ==0){
            ech_signal(n_ech, t_ech , &liste);
            int r = correl(liste, (int)n_ech,&temps,rif); //fct
            // if verif ajoute 1 au compteur et etape = 1 si compteur == 2
            if (r == 0){
                cnt = cnt+1;
                if (cnt == 2){
                    etape = 1; 
                    T1CONbits.TON = 1; // Demare le timer 1
                    i = 1; // reset i
                }    
            }
            else{cnt =0;}
        }


        if (etape == 1){
            // fait une mesure apres chaque timer
            
            if (_T1IF){
               _T1IF = 0;
                if (i == 1){PR1= 0.01*FCY /8; ech_signal(n_ech, t_ech, &L1);}
                if (i == 2){ech_signal(n_ech, t_ech, &L2); }
                if (i == 3){ech_signal(n_ech, t_ech, &L3); }
                if (i == 4){ech_signal(n_ech, t_ech, &L4); }
                if (i == 5){ech_signal(n_ech, t_ech, &L5); }
                if (i == 6){ech_signal(n_ech, t_ech, &L6); }
                if (i == 7){ech_signal(n_ech, t_ech, &L7); }
                if (i == 8){ech_signal(n_ech, t_ech, &L8); }
                if (i == 9){ech_signal(n_ech, t_ech, &L9); }
                if (i == 10){ech_signal(n_ech, t_ech,&L10);}
                if (i == 11){ech_signal(n_ech, t_ech,&L11);}
                i = i+1;
                if (i == 12){etape = 2;}
            }}


        if (etape == 2){
            // Fait la correlation sur chaque info
            n_ech = (int)n_ech;
            for (int i = 0; i < 11; i++){
                if (i == 0){lst[i] = correl(L1, n_ech,&temps,0);}
                if (i == 1){lst[i] = correl(L2, n_ech,&temps,0);}
                if (i == 2){lst[i] = correl(L3, n_ech,&temps,0);}
                if (i == 3){lst[i] = correl(L4, n_ech,&temps,0);}
                if (i == 4){lst[i] = correl(L5, n_ech,&temps,0);}
                if (i == 5){lst[i] = correl(L6, n_ech,&temps,0);}
                if (i == 6){lst[i] = correl(L7, n_ech,&temps,0);}
                if (i == 7){lst[i] = correl(L8, n_ech,&temps,0);}
                if (i == 8){lst[i] = correl(L9, n_ech,&temps,0);}
                if (i == 9){lst[i] = correl(L10, n_ech,&temps,0);}
                if (i == 10){lst[i] = correl(L11, n_ech,&temps,0);}   
            }
            etape = 3;}
        
        
        if (etape == 3){
        i=0;
        while(1){
            // Envoye l'info via UART en boucle
            int16_t y = lst[i] ;
            int8_t msb = (y >>8) & 0xFF; // Donne les 8 premiers bits
            int8_t lsb = y;
            while (U1STAbits.UTXBF) {}  // on attend que le buffer d'émission soit libre
            U1TXREG = lsb; // Besoin que du LSB car info = 0 1 ou 2
            i = i + 1;
            if (i ==10){i = 0;
            __delay_ms(1000);}
        }
    }
        
        
    }
    return 0;
}
