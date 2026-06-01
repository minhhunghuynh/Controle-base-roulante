#include"ADC.h"
#define FCY 40000000  

void initAdc1(void)
{
 //AD1CON1 control register 1 of ADC1
            
        AD1CON1bits.FORM = 0b00; // choix de la division (de 0 a 1024)
        AD1CON1bits.ASAM = 1; // auto sample active
        AD1CON1bits.AD12B = 0; // Convertisseur sur 10 bits
                 
        AD1CON3bits.ADRC = 0; // choix de l'horloge (ici celle principale)
        AD1CON3bits.ADCS = 0; // sorte de prescaler sur l'horloge pour Tad

        AD1CON2bits.CSCNA = 0; // ?
        
        AD1CON2bits.CHPS = 0; // utilise que CH0
        AD1CSSLbits.CSS0 = 1; // Choix de l'entrée (AN0)
        AD1PCFGLbits.PCFG0 = 0; // AN0 en mode analogique
        
        AD1CON1bits.ADON = 1; // Activation de l'ADC
        // choix du mode d'activation, 
        // 0 = manuel en remettant le bit AD1CON1bits.DONE à 0
        // 2 = défini par le timer3 pour sample à chaque fois
        AD1CON1bits.SSRC = 2;
}


int correl(signed int *list, int nb_ech, int *temps,float ref)
{
    float cos_800[66] = 
     {1.0000, 0.8763, 0.5358, 0.0628, -0.4258, -0.8090, -0.9921, -0.9298, -0.6374, -0.1874, 0.3090, 0.7290, 0.9686, 0.9686, 0.7290, 0.3090, -0.1874, -0.6374, -0.9298, -0.9921, -0.8090, -0.4258, 0.0628, 0.5358, 0.8763, 1.0000, 0.8763, 0.5358, 0.0628, -0.4258, -0.8090, -0.9921, -0.9298, -0.6374, -0.1874, 0.3090, 0.7290, 0.9686, 0.9686, 0.7290, 0.3090, -0.1874, -0.6374, -0.9298, -0.9921, -0.8090, -0.4258, 0.0628, 0.5358, 0.8763, 1.0000, 0.8763, 0.5358, 0.0628, -0.4258, -0.8090, -0.9921, -0.9298, -0.6374, -0.1874, 0.3090, 0.7290, 0.9686, 0.9686, 0.7290, 0.3090};
    float sin_800[66] =
        {0.0000, 0.4818, 0.8443, 0.9980, 0.9048, 0.5878, 0.1253, -0.3681, -0.7705, -0.9823, -0.9511, -0.6845, -0.2487, 0.2487, 0.6845, 0.9511, 0.9823, 0.7705, 0.3681, -0.1253, -0.5878, -0.9048, -0.9980, -0.8443, -0.4818, -0.0000, 0.4818, 0.8443, 0.9980, 0.9048, 0.5878, 0.1253, -0.3681, -0.7705, -0.9823, -0.9511, -0.6845, -0.2487, 0.2487, 0.6845, 0.9511, 0.9823, 0.7705, 0.3681, -0.1253, -0.5878, -0.9048, -0.9980, -0.8443, -0.4818, -0.0000, 0.4818, 0.8443, 0.9980, 0.9048, 0.5878, 0.1253, -0.3681, -0.7705, -0.9823, -0.9511, -0.6845, -0.2487, 0.2487, 0.6845, 0.9511};
    float cos_1200[66] = 
        {1.0000, 0.7290, 0.0628, -0.6374, -0.9921, -0.8090, -0.1874, 0.5358, 0.9686, 0.8763, 0.3090, -0.4258, -0.9298, -0.9298, -0.4258, 0.3090, 0.8763, 0.9686, 0.5358, -0.1874, -0.8090, -0.9921, -0.6374, 0.0628, 0.7290, 1.0000, 0.7290, 0.0628, -0.6374, -0.9921, -0.8090, -0.1874, 0.5358, 0.9686, 0.8763, 0.3090, -0.4258, -0.9298, -0.9298, -0.4258, 0.3090, 0.8763, 0.9686, 0.5358, -0.1874, -0.8090, -0.9921, -0.6374, 0.0628, 0.7290, 1.0000, 0.7290, 0.0628, -0.6374, -0.9921, -0.8090, -0.1874, 0.5358, 0.9686, 0.8763, 0.3090, -0.4258, -0.9298, -0.9298, -0.4258, 0.3090};
    float sin_1200[66] =
        {0.0000, 0.6845, 0.9980, 0.7705, 0.1253, -0.5878, -0.9823, -0.8443, -0.2487, 0.4818, 0.9511, 0.9048, 0.3681, -0.3681, -0.9048, -0.9511, -0.4818, 0.2487, 0.8443, 0.9823, 0.5878, -0.1253, -0.7705, -0.9980, -0.6845, -0.0000, 0.6845, 0.9980, 0.7705, 0.1253, -0.5878, -0.9823, -0.8443, -0.2487, 0.4818, 0.9511, 0.9048, 0.3681, -0.3681, -0.9048, -0.9511, -0.4818, 0.2487, 0.8443, 0.9823, 0.5878, -0.1253, -0.7705, -0.9980, -0.6845, -0.0000, 0.6845, 0.9980, 0.7705, 0.1253, -0.5878, -0.9823, -0.8443, -0.2487, 0.4818, 0.9511, 0.9048, 0.3681, -0.3681, -0.9048, -0.9511};

    float cor_f1 = 0;float sor_f1 = 0;
    float cor_f2 = 0;float sor_f2 = 0;
    float x =0;
    
    for(int i = 0; i < nb_ech; i++) // ici
    {
        x = list[i]-512;   // enlever offset ADC
        
        
        cor_f1 = cor_f1 +  (x *cos_800[i]);
        sor_f1 = sor_f1 +  (x *sin_800[i]);

        cor_f2 = cor_f2 +  (x*cos_1200[i]);
        sor_f2 = cor_f2 +  (x*sin_1200[i]);
    }

    float corel_f1 = (cor_f1*cor_f1) + (sor_f1*sor_f1);
    float corel_f2 = (cor_f2*cor_f2) + (sor_f2*sor_f2);
    
    // Détection
    if(corel_f1 > corel_f2 && corel_f1 > 0.1 * ref)
    {return 0;}   // 800 Hz détecté
    if(corel_f2 > corel_f1 && corel_f2 > 0.1* ref)
    { return 1;}   // 1200 Hz détecté

    return 2;       // rien détecté
}


void ech_signal(float nb_ech, float DT, int *liste){
    int16_t e = 0;
    DT = DT * FCY * 0.001; // Transformation en cycle d'horloge
    float dt = DT/nb_ech; // durée entre chaque echantillons
    
    T3CONbits.TCKPS = 0; // Prescaler 1:8
    PR3 = (int16_t) dt; // 500
    T3CONbits.TON = 1; // Démarage du timer
    

    while(e < (int16_t)nb_ech){
        if (AD1CON1bits.DONE) {
        AD1CON1bits.DONE = 0;
        liste[e] =(signed int) ADC1BUF0;
        e = e+1;
        }
    }
    T3CONbits.TON = 0;}