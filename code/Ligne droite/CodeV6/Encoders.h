/* 
 * File:   Encoders.h
 * Author: M. Osee
 * Created on 13/01/2013
 * Last update 13/02/2026
 * This library uses QEI1 peripheral
 */

#ifndef ENCODERS_H
#define	ENCODERS_H

    #include <xc.h>

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
    
    void correction(int16_t pos1, int16_t pos2, float ref1, float ref2, float coef);
    void v_avant(int16_t seuil,int16_t pos_avant_rot) ;
    
    void v_arriere(int16_t seuil,int16_t pos_avant_rot) ;
    
    int16_t mettre_to_pos(int16_t cm);

    


#endif	/* ENCODERS_H */

