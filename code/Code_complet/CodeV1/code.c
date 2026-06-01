#define FCY 40000000    
#include "libpic30.h"   
#include <xc.h> 
#include"configuration.h"
#include"Encoders.h"

/*
Code divisé en 4 parties pour lreception du essage et transfo en bit :
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
    PR1= 0.01*FCY /8; // On veut 4 ms pour apres la detection du start bit
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
    float coef = 200;
    
    // Ordre de distance 
    float dist_cm = 222;
    int16_t dist_pos = mettre_to_pos(dist_cm);;
    
    //Variable pour les pos de roues
    int16_t x = 0; 
    int16_t y = 0;
    float err = 0;
    
    // Partie acceleration :
    float acc = 0.01;
    float start_pwm = 0.2;
    int coef_corr_acc = 100;
    T4CONbits.TCKPS = 0b10; // set le prescaler a 1:64
    int16_t timer = 20000; // Timer de 0.004s * 64 = 0.032
    PR4 = timer; // demarage timer d'acc
    T4CONbits.TON = 1;
    
    // Timer de la correction
    // taille de PR1 = 15bits => valeur max = 32 768
    PR5 = 20000; //timer de t = 0.0005s 
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
    T1CONbits.TON = 1;
    
    
    while(1) {
        if (etape ==0){
            
            ech_signal(n_ech, t_ech , &liste);
            r = correl(liste, (int)n_ech,&temps,rif); //fct
            
            // if verif ajoute 1 au compteur et etape = 1 si compteur == 2
            int16_t send = r;
            int8_t msb = (send >>8) & 0xFF; // Donne les 8 premiers bits
            int8_t lsb = send;
            while (U1STAbits.UTXBF) {}  // on attend que le buffer d'émission soit libre
            U1TXREG = lsb; // Besoin que du LSB car info = 0 1 ou 2
            
            if (r == 0){
                cnt = cnt+1;
                if (cnt == 2){
                    etape = 1; 
                     ech_signal(n_ech, t_ech , &liste);
                     ech_signal(n_ech, t_ech , &liste);
                    i = 1; // reset i
                    cnt = 0;
                }    
            }
            else{cnt =0;}
        }


        if (etape == 1){
            // fait une mesure apres chaque timer
            //_LATB2 = 0; 
            if (_T1IF){
               _T1IF = 0;
               _LATB2 = 1; 
                if (i == 1){ ech_signal(n_ech, t_ech, &L1);}
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
                //_LATB2 = 0;
            }}


        if (etape == 2){
            
            // Fait la correlation sur chaque info
            n_ech = (int)n_ech;
            for (int i = 0; i < 11; i++){
                if (i == 0){ordre = correl(L1, n_ech,&temps,0);sum = sum + ordre;}
                if (i == 1){sens = correl(L2, n_ech,&temps,0);sum = sum + sens;}
                if (i == 2){lst[0] = correl(L3, n_ech,&temps,0);sum = sum + lst[0];}
                if (i == 3){lst[1] = correl(L4, n_ech,&temps,0);sum = sum + lst[1];}
                if (i == 4){lst[2] = correl(L5, n_ech,&temps,0);sum = sum + lst[2];}
                if (i == 5){lst[3] = correl(L6, n_ech,&temps,0);sum = sum + lst[3];}
                if (i == 6){lst[4] = correl(L7, n_ech,&temps,0);sum = sum + lst[4];}
                if (i == 7){lst[5] = correl(L8, n_ech,&temps,0);sum = sum + lst[5];}
                if (i == 8){lst[6] = correl(L9, n_ech,&temps,0);sum = sum + lst[6];}
                if (i == 9){lst[7] = correl(L10, n_ech,&temps,0);sum = sum + lst[7];}
                if (i == 10){parite = correl(L11, n_ech,&temps,0);sum = sum + parite;}   
                
            }
            // Verification de la parité
            i = 0;
            int v =ispair(sum);
            sum = 0;
            enc_reset(); // reset les pos
            float integr = 0;
            signed old_err = 0;
            int rep = 0;
            
             if (0 == 0){ // Pas encore de verif
                if (ordre == 1) {
                    etape_ligne = 1; // Si = 1 => passe l'accéleration
                    int dist_cm = octet_to_int(lst);
                    int16_t dist_pos = mettre_to_pos((float)dist_cm);
                    if (sens == 0){
                        _LATB8 = 1;
                        //ordre = 10;
                        _LATB11 = 1;
                        QEI1CONbits.SWPAB = 1;
                        QEI2CONbits.SWPAB = 0; 
                         etape = 3; // Avance  
                    } 
                    if (sens == 1) {
                        //_LATB8 = 0;
                        //_LATB11 = 0;
                        //QEI1CONbits.SWPAB = 0;
                        //QEI2CONbits.SWPAB = 1; 
                        etape = 0;}} // Recule
                if (ordre == 0) {  // Tourne 
                    float nb_degre = octet_to_int(lst);
                    float seuil = 9.95  *nb_degre;
                    //if (sens == 0){etape = 4;}
                    //if (sens == 1){etape = 5;}
                    etape = 0;
                    
                }}
            else {etape = 0;} //retour au debut si autre chose
                    
            
            }
        
        
        if (etape == 3){
              
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
            
            if (etape_ligne == 0){ etape_ligne = acceleration(acc,start_pwm, coef);}
            
            if (etape_ligne == 1){
                
                if (_T5IF){
                    _T5IF = 0;
                    _LATB2 = 0;
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

                    float pwm2 = ref2*PR2 + coef*1.5*err + 0.5*integr;
                    float pwm1 = ref1*PR2 - coef*1.5*err - 0.5*integr;    
                    if (pwm1 > 1199){pwm1 = 1199;}
                    if (pwm1<1){pwm1 = 1;}
                    if (pwm2 > 1199){pwm2 = 1199;}
                    if (pwm2<1){pwm2 = 1;}
                    OC1RS = pwm1;
                    OC2RS = pwm2;
                    //correction(ref1,ref2,coef,corr);}
                }
                old_err = err;
            }}
        
        if (etape == 4){
            int16_t r = (int16_t) seuil; 
            //v_avant(r);
            //etape = 0;
           
            }
        if (etape == 5){
            int16_t r = (int16_t) seuil; 
            //v_arriere(r);
            //etape = 0;
            
        }
        
        } 
    return 0;
}
