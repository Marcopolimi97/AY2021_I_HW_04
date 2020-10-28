/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#ifndef __INTERRUPT_ROUTINES_H
    #define __INTERRUPT_ROUTINES_H
    
    #include "cytypes.h"
    #include "stdio.h"
    
    //4 byte per inviare: header, valore photoresistor, valore potenziometro, tail
    #define BYTE_TO_SEND 4
    
    #define TRANSMIT_BUFFER_SIZE 1+BYTE_TO_SEND+1
    
    CY_ISR_PROTO(Custom_ISR_ADC);
    CY_ISR_PROTO(Custom_ISR_RX);
    
    uint8 DataBuffer[TRANSMIT_BUFFER_SIZE];
    
    //il valore letto dal potenziomentro è messo come variabile globale
    //perchè è letto sia nell'ISR ADC sia nel main per settare la luminosità del LED
    int32 value_digit_PWM;
    
    //flag utili
    volatile uint8 SmartLampOnFlag;
    volatile uint8 PacketReadyFlag;
    volatile uint8 PhotoresistorThresholdFlag;
#endif
    