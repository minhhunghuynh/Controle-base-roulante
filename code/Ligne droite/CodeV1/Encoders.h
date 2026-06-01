/* 
 * File:   Encoders.h
 * Author: M. Osee
 * Created on 13/01/2013
 * Last update 13/02/2026
 * 
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
    void enc_config(uint8_t rpA, uint8_t rpB, uint8_t mode);
    
    /* Return the actual value of the QEI counter.  This value is the actual
     * encoder angular position (relative to its initial position),
     * in encoder's tick.
     * Parameters:
     *  - none
     * Return value:
     *   - QEI counter value */
    int16_t enc_getPos(void);
    
    /* Reset the QEI counter.  This sets the actual angular position
     * as new initial position
     * Parameters:
     *  - none
     * Return value:
     *   - none */
    void enc_reset(void);


#endif	/* ENCODERS_H */

