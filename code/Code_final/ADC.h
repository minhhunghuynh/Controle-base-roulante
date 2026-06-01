
#ifndef ADC_H
#define	ADC_H

#include <xc.h> // include processor files - each processor file is guarded.  


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
    
    void initAdc1(void);
    /* Setup de l'ADC
     * Auto-sample via le timer 3
     * Donnée stockée sur 10 bits, 0-1023
     * Pin d'entrée : AN0
     */
    
    int correl(signed int *list, int nb_ech, int *temps,float ref);
    /*Effectue la correlation sur une liste d'echantillon et verifie si energie au dessus de la ref pour differencier entre vide,800Hz et 1200Hz
     * En entrée : liste des echantillons , nombre d'echantillons dans la liste, vecteur de remps, reference d'energie minimale
     * En sortie : 800Hz => 0 ; 1200Hz => 1 ; rien => 2
     */
    
    void ech_signal(float nb_ech, float DT, int *liste);
    /*Echantillonne une information sur un temps DT avec nb_ech d'echantillons
     * !! Utilise le timer 3!!
     */
    
#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

