/* 
 * File:   rotation.h
 * Author: matthieudebruyne
 *
 * Created on February 27, 2026, 3:55 PM
 */

#ifndef Encoders_H
#define	Encoders_H

#ifdef	__cplusplus
extern "C" {
#endif



    #include <xc.h>
    #include <libpic30.h>

    #define ENC_X2_MODE  0b101
    #define ENC_X4_MODE  0b111

    /* configure the QEI in given mode and the PPS to connect its inputs to
     * the given pins.
     * Parameters:
     *   - rpA: ID of the remappable pin to connect the CHA input of QEI1
     *   - rpB: ID of the remappable pin to connect the CHB input of QEI1
     *   - mode: tick counting mode:
     *       ENC_X2_MODE: counter is incremented/decremented by 2 per period 
     *       ENC_X4_MODE: counter is incremented/decremented by 4 per period 
     * Return value:
     *   - none */
    void frcPll40MHzConfig(void);
    void extClockConfig(void);
    void enc_config(void);
    
    /* setup pour la position
     * Parameters:
     *  - none
     * Return value:
     *   - QEI counter value */
    int16_t enc_getPos1(void);
    int16_t enc_getPos2(void);
    
    /* Reset the QEI counter.  This sets the actual angular position
     * as new initial position
     * Parameters:
     *  - none
     * Return value:
     *   - none */
    void enc_reset(void);
    
    void setup_UART(void);
    
    void setup_PWM(void);
    
    void correction( float ref1, float ref2, float coef,float corr);
    /* Code permettant de garder la trajectoire
    Pas de msure de l'erreur resultante
    Entr?e : Position de la roue 1 et 2
    */
    int16_t mettre_to_pos(float cm);
    void correction_rot(int16_t x, int16_t y ,float ref1,float ref2, float coef);

    
    void v_avant(uint16_t seuil) ;
    int16_t acceleration(float acc, float ref_pwm, int coef);
    
    //void v_arriere(float seuil) ;
    
    float absolu(float value);
    
    //void correctionII(float err, float old_err,float coef,float coef_int, int *rep, float *integr);
   
    void initAdc1(void);
    
    
    void ech(int nb_ech,float DT, int16_t *liste);
    /* Fonction qui echantillonne un signale au pin RA0 
     * Sortie allant de 0 à 4095 (0V - 3.3V)
     * DT est le temps sur lequel on veut faire n échantillons.
     */
#ifdef	__cplusplus
}
#endif

#endif	/* ROTATION_H */

