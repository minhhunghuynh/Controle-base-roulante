
#ifndef mouvement_H
#define mouvement_H


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
    
    #include <xc.h> // include processor files - each processor file is guarded.  


    void correction( float ref1, float ref2, float coef,float corr,int16_t dist_r1,int16_t dist_r2);
    /* Code permettant de garder la trajectoire
    Pas de msure de l'erreur resultante
    Entr?e : Position de la roue 1 et 2
     */
    
    void correction_rot(int16_t x, int16_t y ,float ref1,float ref2, float coef,int16_t dist_r1, int16_t dist_r2);
    /*Correction proportionnelle utilisée pour la rotation de la voiture*/
    
    void v_arriere(int16_t seuil) ;
    void v_avant(uint16_t seuil) ;
    /* Fait tourner la voiture jusqu'au seuil(en position de roue)*/

    void correction2(float err, float old_err,float coef,float coef_int, int *rep, float *integr);
    /* Correction Proportionnelle et intégrale
     * Entrée :
     * err : erreur entre les roue 
     * old_err : ancienne erreur (pour le terme intégral)
     * integr : ancien terme intégral
     */
    
    void acceleration(float acc,int16_t timer,int16_t*etape,int dist_acc,int16_t dist_r1, int16_t dist_r2);
    /* Fait accelerer la voiture pour eviter le glissement
     */
    void deceleration(float dec, float ref_pwm, int coef, int dist_pos, float minimum,int16_t* etape,int16_t dist_r1, int16_t dist_r2);
    /* Decelere la voiture pour une meilleure precision sur la fin de la course
     */
    
#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

