#define FCY 40000000    
#include "libpic30.h"   
#include <xc.h> 
#include"configuration.h"
#include"Encoders.h"

int main(void) {
    setup_UART();
    setup_PWM(); // OCxR peut aller de 1 a 1199 (demarre si 0)
    enc_config(); // setup de la prise de position
    enc_reset(); // reset les pos (pas nécessaire)
    initAdc1();
    
    // Temps :
    float t_ech = 2.6; // En milisencode
    float f_ech = 10000;
    float n_ech = t_ech * 0.001*f_ech;
    
    
    
    _TRISA3 = 0;    // output pin
    _TRISA1 = 1;

    
    int16_t rep = 0;
    float liste[(signed int)n_ech]; 
    int16_t res[12];
    
    int prec = 0; // Si = 0 alors avant le signal etait petit sinon le signal etait grand
    int out = 0;
    uint8_t cnt = 0;
    
    
    T1CONbits.TCKPS = 01; // set le prescaler a 1:8
    // On veut 6 ms
    PR1= 3*0.001*FCY / 8; // 1.4*0.001*FCY /8=  

    
    
    int etape = 0;
    
    // A ajouter : 
    // Attendre autre chose que 0 le premier sera un start bit
    // Ensuite prendre n echantillons apres le strart 
    // Determiner la fréquence par methode de correlation
    // Stocher la réponse dans une liste
    // Analyser la reponse pour faire avancer la voiture
    
    // Fonction à créer : 
    // Fonction prenant n échantillons et les stockent dans une liste
    // Fonction qui détecte la fréquence (3 options de sortie : 0, 800 ou 1200)
    //ech_signal(100, 0.1,&liste);  // Adc va de 0 à 3.3 V et  0 a 4080 dif val
    
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
    int16_t y = 10;
    
    float temps[(int)n_ech];
    for (int t=0 ; t< (int)2*n_ech ; ++t){
    temps[t] = t * (1/(f_ech*FCY)) ; 
}
int i = 1;
int16_t lst[11];
           
T1CONbits.TON = 1;
float rif = 88604672;
    
_TRISB2 = 0;    // output pin
_LATB2 = 0;

    while(1) {
        
        if (etape ==0){
            ech_signal(n_ech, t_ech , &liste);
           
            // Ajout de la fonciton de verification
            //int r1 = TMR1 ;
            _LATB2 = 1;
            int r = correl(liste, (int)n_ech,&temps,rif); //fct
            _LATB2 = 0;
            /*
            int16_t y =(int16_t) (r);
            int8_t msb = (y >>8) & 0xFF; // Donne les 8 premiers bits
            int8_t lsb = y;
            while (U1STAbits.UTXBF) {}  // on attend que le buffer d'émission soit libre
            U1TXREG = lsb;
            */
             
            // if verif ajoute 1 au compteur et etape = 1 si compteur == 2
           
            if (r == 0){
                 
                cnt = cnt+1;
                
                
                if (cnt == 2){etape =1;
                TMR1 = 0 ; // Remet le compteur à zéro 
                _T1IF =0;
                int i = 1;
                while(!_T1IF){}
                _LATB2 = 0;
                // On veut 5 ms.
                    //T1CONbits.TCKPS = 0b10; // set le prescaler a 1:64
                PR1= 0.01*FCY /8;
                } 
                
            }
            else{cnt =0;}
        }// Réussi à passer à l'etape 1
        if (etape == 1){n_ech = 50; t_ech = 5;
           
            
            // fait mesure apres tout les compteur
            
            if (_T1IF){
               _T1IF = 0;
                if (i == 1){ech_signal(n_ech, t_ech, &L1); _LATB2 = 1;}
                if (i == 2){ech_signal(n_ech, t_ech, &L2); _LATB2 = 0;}
                if (i == 3){ech_signal(n_ech, t_ech, &L3); _LATB2 = 1;}
                if (i == 4){ech_signal(n_ech, t_ech, &L4); _LATB2 = 0;}
                if (i == 5){ech_signal(n_ech, t_ech, &L5); _LATB2 = 1;}
                if (i == 6){ech_signal(n_ech, t_ech, &L6); _LATB2 = 0;}
                if (i == 7){ech_signal(n_ech, t_ech, &L7); _LATB2 = 1;}
                if (i == 8){ech_signal(n_ech, t_ech, &L8); _LATB2 = 0;}
                if (i == 9){ech_signal(n_ech, t_ech, &L9); _LATB2 = 1;}
                if (i == 10){ech_signal(n_ech, t_ech,&L10); _LATB2 = 0;}
                if (i == 11){ech_signal(n_ech, t_ech,&L11); _LATB2 = 1;}
                i = i+1;
                if (i == 12){etape = 2;}
                //ech_signal(n_ech, t_ech,&liste);
                
            }
            
        }
        if (etape == 2){
            
            //signed int sl[(int)n_ech];
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
                
                
                // mettre la focniton pour chaque (sl en argument) 
                //int16_t y = lst[i] ;
                
            }
            etape = 3;
            i = 0;
            // Calcule la fréquence des 11 prochain puis passe etape 3 pour verif
        }
        
        
        
        if (etape == 3){
        i=0;
        while(1){
            
        // Partie pour envoyer info via UART
        int16_t y = lst[i] ;
        int8_t msb = (y >>8) & 0xFF; // Donne les 8 premiers bits
        int8_t lsb = y;
        while (U1STAbits.UTXBF) {}  // on attend que le buffer d'émission soit libre
        U1TXREG = lsb;
        
        
        i = i + 1;
        if (i ==10){i = 0;
        __delay_ms(1000);}
        }
    }
    }
    return 0;
}
