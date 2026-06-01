#define FCY 40000000     
#include "libpic30.h"   
#include <xc.h> 
#include"configuration.h"
#include"rotation.h"
#include <math.h>

int16_t rep = 0;
signed int liste[100]; 
int correl(signed int *list, int nb_ech, int *temps){
    float start_bit = 0 ;
    float cor_f1 = 0, sor_f1=0 ; float cor_f2 = 0, sor_f2=0 ;
    float nrj_c_f1; float nrj_s_f1 = 0; //energie de la cor
    float nrj_c_f2; float nrj_s_f2 = 0;
    float sum ;
    float corel_f1 = 0 ; float corel_f2 = 0 ; 
    int f1 = 800 ;
    int f2 = 1200 ; 
    while(start_bit == 0){
        //on fait rien et on attend 
    }
    for(int i =0 ; i< nb_ech ; ++i){
        sum = sum + list[i]*list[i] ;  // déjà mis au carré
        cor_f1 = cor_f1+2045*cos(2*3.14*temps[i]*f1)*liste[i] ;
        sor_f1 = sor_f1+2045*sin(2*3.14*temps[i]*f1)*liste[i] ;
        cor_f2 = cor_f2+2045*cos(2*3.14*temps[i]*f2)*liste[i] ;
        sor_f2 = sor_f2+2045*sin(2*3.14*temps[i]*f2)*liste[i] ;
    }
    //pour mettre au carre
    nrj_c_f1 = cor_f1*cor_f1 ;   
    nrj_s_f1 = sor_f1*sor_f1 ; 
    nrj_c_f2 = cor_f2*cor_f2 ;   
    nrj_s_f2 = sor_f2*sor_f2 ; 
    // somme des énergies des corrélation des cos et sin 
    corel_f1 = nrj_c_f1+nrj_s_f1 ;
    corel_f2 = nrj_c_f2+nrj_s_f2 ;
    if((corel_f1 || corel_f2) >= sum*0.60){
        if (corel_f1 > corel_f2) return 1 ;  
        if (corel_f1 < corel_f2) return 0 ;
    }
    return 2; // Au cas où échec  
}

int main(void) {
    setup_UART();
    setup_PWM(); // OCxR peut aller de 1 a 1199 (demarre si 0)
    enc_config(); // setup de la prise de position
    enc_reset(); // reset les pos (pas nécessaire)
    initAdc1();
    
    int nb_ech ;
    int temps[nb_ech]; 
    signed int* list ;
//int seuil_f_middle = 0; 
//int f_middle = 1000 ;
//int* list_cos_f1 ;
for (int t=0 ; t< nb_ech ; ++t){
    temps[t] = t * (0.01/10000) ; 
}
    //seuil_f_middle += cos(2*pi*temps[t]*f_middle) ;
   
    correl(*list, nb_ech, *temps) ;
    _TRISA3 = 0;    // output pin
    _TRISA1 = 1;
    

    T1CONbits.TCKPS = 01; // set le prescaler a 1:8
    //24000 // Timer de 0.004s * 8 = 0.032
    PR1= 7500; 

    
    
    
    int prec = 0; // Si = 0 alors avant le signal etait petit sinon le signal etait grand
    int out = 0;
    int res = 1;
    uint8_t cnt = 10;
    
    T1CONbits.TON = 1;
    
    // A ajouter : 
    // Attendre autre chose que 0 le premier sera un start bit
    // Ensuite prendre n echantillons apres le strart 
    // Determiner la fréquence par methode de correlation
    // Stocher la réponse dans une liste
    // Analyser la reponse pour faire avancer la voiture
    
    // Fonction à créer : 
    // Fonction prenant n échantillons et les stockent dans une liste
    // Fonction qui détecte la fréquence (3 options de sortie : 0, 800 ou 1200)
    
    
    while(1) {
        
        
        ech(100, 0.1,&liste);  // Adc va de 0 à 3.3 V et  0 a 4080 dif val
        
        
        
        
        
        // Partie pour envoyer info via UART
        int16_t y = liste[50];
        int8_t msb = (y >>8) & 0xFF; // Donne les 8 premiers bits
        int8_t lsb = y;
        while (U1STAbits.UTXBF) {}  // on attend que le buffer d'émission soit libre
        U1TXREG = lsb;
        while (U1STAbits.UTXBF) {}  // on attend que le buffer d'émission soit libre
        U1TXREG = msb;
    }
    return 0;
}
