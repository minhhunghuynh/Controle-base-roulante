/* 
 * File:   Encoders.h
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
    float enc_getPos1(int16_t dist_r1);
    float enc_getPos2(int16_t dist_r2);
    /* En entrée : la distance a parcourir da la roue x
     * En sortie : une fraction de la distance parcourue (de 0 à 10000)
     */
    
    void enc_reset(void);
    /* Reset the QEI counter.  This sets the actual angular position
     * as new initial position
     * Parameters:
     *  - none
     * Return value:
     *   - none */
    
    void setup_UART(void);
    
    /* Setup une communication en UART via le pin RB4 (sortie du message)
     * Permet de fixer la fréquence d'horlage à 40 000 000 Hz
     * Format de l'UART : start bit,8bits f'info, stop bit
     */
    
    void setup_PWM(void);
    /* Setup la commande du moteur via OCxRS
     * !!!Utilise le timer 2!!!
     */
    
    int16_t mettre_to_pos(float cm);
    /* Transforme une distance en mettre vers l'unité utilisé pour la mesure de la postion de la roue
     */
    
    float absolu(float value);
    /* Retourne la valeur absolue*/
    
    int octet_to_int(int *liste);
    /* Transforme un octet stocké dans une liste vers un int */
    
    int ispair(int number);
    
    
#ifdef	__cplusplus
}
#endif

#endif	

