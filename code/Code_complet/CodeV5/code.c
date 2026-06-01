#define FCY 40000000    
#include "libpic30.h"   
#include <xc.h> 
#include"configuration.h"
#include"Encoders.h"
#include"ADC.h"
#include"mouvement.h"

/*
Code divis� en 4 parties pour lreception du essage et transfo en bit :
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
    
    // Pour mesure de test
    _TRISB2 = 0;    // output pin
    _LATB2 =1;   
    _TRISB3 = 0;    // output pin
    _LATB3 = 1;  


    // Param�tre d'�chantillonnage :
    float t_ech = 2.6; // En milisencode
    float f_ech = 10000;
    int n_ech = (int)(t_ech * 0.001*f_ech);

    // Stockage des echantillonnnages du start bit
    float liste[(signed int)n_ech];
    // liste des donn�es 
    signed int L1[2*n_ech]; signed int L2[2*n_ech];signed int L3[2*n_ech];signed int L4[2*n_ech];signed int L5[2*n_ech];signed int L6[2*n_ech];signed int L7[2*n_ech];signed int L8[2*n_ech];signed int L9[2*n_ech];signed int L10[2*n_ech];signed int L11[2*n_ech];
    
    
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
    int16_t etape = 0;
    int16_t* p_etape = &etape;
    // Paramettre etape 1 :
    int i = 1;
    
    // Paramettre etape 2 : 
    int ordre = 2;
    int sens = 2;
    int parite = 2;
    int sum =0;
    
    // Parametre de d�placement de la voiture
    int16_t etape_ligne = 1;
    int16_t* p_etape_ligne = &etape_ligne;
    
    // reference de vitesse de voiture
    float ref1 = 0.5; 
    float ref2 = 0.5;
    OC1RS = 1;
    OC2RS = 1;
    
    // Coefficient de correction (proportionnelle)
    float coef = 200;
    
    // Ordre de distance 
    float dist_cm = 100;
    int16_t dist_pos = mettre_to_pos(dist_cm);;
    
    //Variable pour les pos de roues
    int16_t x = 0; 
    int16_t y = 0;
    float err = 0;
    
    // Partie acceleration :
    float acc = 0.005;
    float start_pwm = 0.2;
    int coef_corr_acc = 100;
    T4CONbits.TCKPS = 0b01; // set le prescaler a 1:8
    int16_t timer = 24000; // Timer de 0.004s * 64 = 0.032
    PR4 = timer; // demarage timer d'acc
    T4CONbits.TON = 1;
    
    //Partie deceleration 
    int16_t dist_decel = dist_pos*0.8 ; // distance de d�but de d�c�l�ration
    
    // Timer de la correction
    // taille de PR = 15bits => valeur max = 32 768
    T5CONbits.TCKPS = 00;
    PR5 = 3000; //timer de t = 0.0005s * 8 
    T5CONbits.TON = 1;  // starts timer 1
    
    // Constantes de rotation
    float nb_degre = 0;
    float seuil =0; // seuil = 9.95*nb_degre
    
    
    // terme int�grale
    float integr = 0;
    signed old_err = 0;
    int rep = 0;    
    float coef_int = 1;
    // Boucle infinie
    int r = 0;
    
                       
    while(1==1) {
        
        
        
        if (etape ==0){ // Recherche le start bit
            ech_signal(n_ech, t_ech , &liste);
            r = correl(liste, n_ech,&temps,rif); //fct
            
            // if verif ajoute 1 au compteur et etape = 1 si compteur == 2
            if (r == 0){
                cnt = cnt+1;
                
                if (cnt == 2){ // On veut deux d�tections de suite
                    etape = 1;
                    ech_signal(n_ech, t_ech , &liste); // Attend avant de prendre les prochaines mesures
                    ech_signal(n_ech, t_ech , &liste);
                    //ech_signal(2, 0.1*t_ech , &liste);
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
                   ech_signal(2*n_ech, 2*t_ech, &L1);}                
                else if (i == 2){ech_signal(2*n_ech, 2*t_ech, &L2);}
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
                TMR1 = 0;
                T1CONbits.TON = 0;
                
                }
            }//_LATB2 = 0;
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
            
            // Verification de la parit�
            i = 0;
            int v =ispair(sum);
            
            sum = 0;
            enc_reset(); // reset les pos
            integr = 0;
            old_err = 0;
            rep = 0;
            
            if (v == 0){ // verif de parit�
                if (ordre == 1) { // Partie ligne droite
                    
                    etape_ligne = 1; // Si = 1 => fait l'acc�leration
                    dist_cm = octet_to_int(lst);
                    while (U1STAbits.UTXBF) {}  // on attend que le buffer d'�mission soit libre
        U1TXREG = 50;
                    dist_pos = mettre_to_pos((float)dist_cm);
                    dist_decel = dist_pos*0.8 ;
                    if (sens == 1){
                        _LATB8 = 1;
                        _LATB11 = 1;
                        QEI1CONbits.SWPAB = 1;
                        QEI2CONbits.SWPAB = 0; 
                        etape = 3; // avance  
                    } 
                    if (sens == 0) {  
                        _LATB8 = 0;
                        _LATB11 = 0;
                        QEI1CONbits.SWPAB = 0;
                        QEI2CONbits.SWPAB = 1; 
                        etape = 3;
                    }} // recule
                if (ordre == 0) {  // Tourne 
                    nb_degre = octet_to_int(lst);
                    seuil = 9.95  *nb_degre;
                    if (sens == 0){etape = 4;}
                    if (sens == 1){etape = 5;}
                    
                }}
            else {etape = 0;} //retour au debut si autre chose
            enc_reset(); 
            
            
            }
        
        
        
        if (etape == 3){ // Ligne droite
            _LATB3 = 0;
            if (etape_ligne == 1){
               
                acceleration(acc,timer,p_etape_ligne,dist_pos);
        }
            if (etape_ligne == 2){
                
                if(dist_decel< enc_getPos1() && dist_decel< enc_getPos2()){
                    *p_etape_ligne = 3;}
                
                x = enc_getPos1();
                y = enc_getPos2();
                err = x-y;
                correction2(err, old_err, coef, coef_int,&rep,&integr);
                old_err = err;
                }
            

            if (*p_etape_ligne == 3){
                deceleration(0.005, 0.5, 20, dist_pos, 0.25, p_etape_ligne);
                etape = 0;
        }}

        
        if (etape == 4){
            enc_reset();
            int16_t r = (int16_t) seuil; 
            v_avant(r);
            etape = 0;
           
            }
        if (etape == 5){
            enc_reset();
            int16_t r = (int16_t) seuil; 
            v_arriere(r);
            etape = 0;   
        }
       
        } 
    return 0;
}
