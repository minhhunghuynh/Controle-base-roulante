
#ifndef ADC_H
#define	ADC_H

#include <xc.h> // include processor files - each processor file is guarded.  


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    void ech_message(int *tab, int n_ech, int t_ech);
    /* echantillonne la trame et la store dans tab*/
    
    int correl(signed int *list, int nb_ech, int *temps,float ref);
    /*Effectue la correlation sur une liste d'echantillon et verifie si energie au dessus de la ref pour differencier entre vide et message*/
    
    void ech_signal(float nb_ech, float DT, int *liste);
    /*Echantillonne une information sur un temps DT avec nb_ech d'echantillons*/
    
#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

