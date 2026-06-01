#define FCY 40000000    
#include "libpic30.h"   
#include <xc.h> 
#include"configuration.h"
#include"Encoders.h"
#include"ADC.h"

/*
Code divisé en 4 parties pour lreception du essage et transfo en bit :
    - Etape 0 : Attente du start bit (lis sur l'ADC et Conversion)
    - Etape 1 : Effectue les mesures du reste de la trame sur l'ADC
    - Etape 2 : Convertit ces mesure vers bit 0 ou 1 (2 si pas au dessus de seuil)
    - Etape 3 : Envoie en UART du pin RB4 la trame (sans le start bit0)
*/





int main(void) {
    
    setup_UART(); // Refixe la f de l'horloge
    setup_PWM(); // OCxR peut aller de 1 a 1199 (demarre si 0)
    enc_config(); // setup de la prise de position
    enc_reset(); // reset les pos
    initAdc1(); // Initialisation de l'ADC(10bit,0-1024,mesures via timer3,...)
    
    // Pour test
    _TRISB2 = 0;    // output pin
    _LATB2 = 1;   


    // Paramètre d'échantillonnage :
    float t_ech = 2.6; // En milisencode
    float f_ech = 10000;
    int n_ech = (int)(t_ech * 0.001*f_ech);
    //n_ech = (int) n_ech;

    // Stockage des echantillonnnages
    float liste[(signed int)n_ech];
    // liste dans lesquelles on stocke les données
    signed int L1[n_ech]; signed int L2[n_ech];signed int L3[n_ech];signed int L4[n_ech];signed int L5[n_ech];signed int L6[n_ech];signed int L7[n_ech];signed int L8[n_ech];signed int L9[n_ech];signed int L10[n_ech];signed int L11[n_ech];

    int tab[11] = { &L1, &L2, &L3, &L4, &L5, &L6, &L7, &L8, &L9, &L10, &L11 };  //Liste des adresses (en gros un tableau)

    // Timer
    T1CONbits.TCKPS = 0b10;  // set le prescaler a 1:64
    PR1= (0.01)*FCY /64; // Chaques echantillonnage du message se fait a un intervale de 10ms
    // Vecteur temps
    float temps[(int)2*n_ech];
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
    
    // Paramettre etape 2 : 
    int ordre = 2;
    int sens = 2;
    int parite = 2;
    int sum =0;
    
                // Parametre de déplacement de la voiture
    int etape_ligne = 0;
    
    // reference de vitesse de voiture
    float ref1 = 0.5; 
    float ref2 = 0.5;
    OC1RS = 1;
    OC2RS = 1;
    
    // Coefficient de correction (proportionnelle)
    float coef = 50;
    
    // Ordre de distance 
    float dist_cm = 10;
    int16_t dist_pos = mettre_to_pos(dist_cm);;
    
    //Variable pour les pos de roues
    int16_t x = 0; 
    int16_t y = 0;
    float err = 0;
    
    // Partie acceleration :
    float acc = 0.01;
    float start_pwm = 0.2;
    int coef_corr_acc = 100;
    //T4CONbits.TCKPS = 0b10; // set le prescaler a 1:64
    int16_t timer = 20000; // Timer de 0.004s * 64 = 0.032
    //PR4 = timer; // demarage timer d'acc
    //T4CONbits.TON = 1;
    
    // Timer de la correction
    // taille de PR = 15bits => valeur max = 32 768
    T5CONbits.TCKPS = 01;
    PR5 = 20000; //timer de t = 0.0005s * 8 
    T5CONbits.TON = 1;  // starts timer 1
    
    // Constantes de rotation
    float nb_degre = 0;
    float seuil =0;
    
    
    // terme intégrale
    float integr = 0;
    signed old_err = 0;
    int rep = 0;    
    float coef_int = 1;
    int16_t send = 0;
//  int16_t  send= 0 ;
    int n = 0;
    // Boucle infinie
    int r = 0;
    //T1CONbits.TON = 1;
    
    
    while(1==1) {
      
        //int8_t y =(int8_t) etape ;
        //while (U1STAbits.UTXBF) {}  // on attend que le buffer d'émission soit libre
        //U1TXREG = y; // Besoin que du LSB car info = 0 1 ou 2
        
        
        
        
        if (etape ==0){ // Recherche le start bit
            ech_signal(n_ech, t_ech , &liste);
            r = correl(liste, (int)n_ech,&temps,rif); //fct
            
            // if verif ajoute 1 au compteur et etape = 1 si compteur == 2
            
            
            if (r == 0){
                cnt = cnt+1;
                
                if (cnt == 2){ // On veut deux détections de suite
                    etape = 1;
                    ech_signal(n_ech, t_ech , &liste); // Attend 3*2.6
                    ech_signal(n_ech, t_ech , &liste);
                    ech_signal(n_ech, 0.5*t_ech , &liste);
                    i = 1; // reset i
                    cnt = 0;}} // Reset cnt pour prochaine couple
            else{cnt =0;}
        }


        if (etape == 1){
            // fait une mesure apres chaque timer    
            if (i == 1) {_T1IF = 1;}
            if (_T1IF == 1){
               _T1IF = 0;
               _LATB2 = 0;
                
               if (i == 1){
                   T1CONbits.TON = 1;
                   ech_signal(2*n_ech, 2*t_ech, &L1);
                //_T1IF = 0; // ca ne reset pas le temps
                //_LATB2 = 1;
                //while (_T1IF == 0){}
                //i = 2;
               _LATB2 = 1;
                    //ech_signal(n_ech, t_ech , &liste);
                    //ech_signal(n_ech, t_ech , &liste); // Probleme de temps donc retard artificiel
               }
                
                else if (i == 2){
                    ech_signal(2*n_ech, 2*t_ech, &L2); }
                else if (i == 3){ech_signal(2*n_ech, 2*t_ech, &L3); }
                else if (i == 4){ech_signal(2*n_ech, 2*t_ech, &L4); }
                else if (i == 5){ech_signal(2*n_ech, 2*t_ech, &L5); }
                else if (i == 6){ech_signal(2*n_ech, 2*t_ech, &L6); }
                else if (i == 7){ech_signal(2*n_ech, 2*t_ech, &L7); }
                else if (i == 8){ech_signal(2*n_ech, 2*t_ech, &L8); }
                else if (i == 9){ech_signal(2*n_ech, 2*t_ech, &L9); }
                else if (i == 10){ech_signal(2*n_ech, 2*t_ech,&L10);}
                else if (i == 11){ech_signal(2*n_ech, 2*t_ech,&L11);}
               _LATB2 = 1;
                i = i+1;
                if (i == 12){etape = 2;
                T1CONbits.TON = 0;
                TMR1 = 0;
                }
            }


            //_LATB2 = 0;
            } // passe


        if (etape == 2){
            
            // Fait la correlation sur chaque info
            n_ech = n_ech *2;
            for (int i = 0; i < 11; i++){
                if (i == 0){ordre = correl(L1, n_ech,&temps,0);sum = sum + ordre;}
                else if (i == 1){sens = correl(L2, n_ech,&temps,0);sum = sum + sens;}
                else if (i == 2){lst[0] = correl(L3, n_ech,&temps,0);sum = sum + lst[0];}
                else if (i == 3){lst[1] = correl(L4, n_ech,&temps,0);sum = sum + lst[1];}
                else if (i == 4){lst[2] = correl(L5, n_ech,&temps,0);sum = sum + lst[2];}
                else if (i == 5){lst[3] = correl(L6, n_ech,&temps,0);sum = sum + lst[3];}
                else if (i == 6){lst[4] = correl(L7, n_ech,&temps,0);sum = sum + lst[4];}
                else if (i == 7){lst[5] = correl(L8, n_ech,&temps,0);sum = sum + lst[5];}
                else if (i == 8){lst[6] = correl(L9, n_ech,&temps,0);sum = sum + lst[6];}
                else if (i == 9){lst[7] = correl(L10, n_ech,&temps,0);sum = sum + lst[7];}
                else if (i == 10){parite = correl(L11, n_ech,&temps,0);sum = sum + parite;}   
                
            }
            int i=0;
            n_ech = n_ech /2;
            /*while(1==1){
    // Envoye l'info via UART en boucle
        int8_t y = octet_to_int(lst) ;
        //int8_t msb = (y >>8) & 0xFF; // Donne les 8 premiers bits
        //int8_t lsb = y;
        while (U1STAbits.UTXBF) {}  // on attend que le buffer d'émission soit libre
        U1TXREG = y; // Besoin que du LSB car info = 0 1 ou 2
        while (U1STAbits.UTXBF) {}  // on attend que le buffer d'émission soit libre
        U1TXREG = 0; // Besoin que du LSB car info = 0 1 ou 2
        i = i + 1;
        if (i>8){i = 0;}
        
        }*/
            
            // Verification de la parité
            i = 0;
            int v =ispair(sum);
            sum = 0;
            enc_reset(); // reset les pos
            float integr = 0;
            signed old_err = 0;
            int rep = 0;
            
            if (v == 0){ // verif de parité
                if (ordre == 1) { // Partie ligne droite
                    
                    etape_ligne = 1; // Si = 1 => passe l'accéleration
                    dist_cm = octet_to_int(lst);
                    
                    dist_pos = mettre_to_pos((float)dist_cm);
                    if (sens == 0){
                        _LATB8 = 1;
                        _LATB11 = 1;
                        QEI1CONbits.SWPAB = 1;
                        QEI2CONbits.SWPAB = 0; 
                        etape = 3; // recule  
                    } 
                    if (sens == 1) {  
                        _LATB8 = 0;
                        _LATB11 = 0;
                        QEI1CONbits.SWPAB = 0;
                        QEI2CONbits.SWPAB = 1; 
                        etape = 3;
                    }} // avance
                if (ordre == 0) {  // Tourne 
                    float nb_degre = octet_to_int(lst);
                    float seuil = 9.95  *nb_degre;
                    if (sens == 0){etape = 4;}
                    if (sens == 1){etape = 5;}
                    
                }}
            else {etape = 0;} //retour au debut si autre chose
            enc_reset(); 
                    
            
            }
        
        
        
        if (etape == 3){ // Ligne droite
            
            // Condition d'arret 
            //if(dist_pos< enc_getPos1()){  
            //OC1RS = 1;
            //_LATB2 = 1;
            if (dist_pos< enc_getPos2()){etape = 0; OC1RS = 1;OC2RS = 1;}
            //}
            //if(dist_pos < enc_getPos2()){
            //OC2RS = 1;
            //if (dist_pos< enc_getPos1()){etape = 0;}
            //} 
            
            //if (etape_ligne == 0){ etape_ligne = acceleration(acc,start_pwm, coef);}
            etape_ligne = 1;
            if (etape_ligne == 1){ // Correction de la trjaectoire 
                
                if (_T5IF){
                    _T5IF = 0;
                    //_LATB2 = 0;
                    //float corr =0;// 16*dist_cm/120;
                    x = enc_getPos1();
                    y = enc_getPos2();
                    correction(ref1,ref2,coef);
                }
               
            }}
        
        if (etape == 4){
            int16_t r = (int16_t) seuil; 
            v_avant(r);
            etape = 0;
           
            }
        if (etape == 5){
            int16_t r = (int16_t) seuil; 
            v_arriere(r);
            etape = 0;   
        }
        
        } 
    return 0;
}
