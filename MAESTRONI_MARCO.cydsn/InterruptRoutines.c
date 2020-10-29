/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * MARCO MAESTRONI: - gestione interrupt UART per il segnale di accensione e spegnimento.
 *                  - gestione interrupt ADC per lettura del photoresistor e potenziometro
 *
 *
 * ========================================
*/

#include "project.h"
#include "InterruptRoutines.h"
#include "stdio.h"

#define LED_ON                      1
#define LED_OFF                     0

#define PHOTORESISTOR_MUX           0
#define POTENTIOMETER_MUX           1

#define MAX_PERIOD              65535

//il valore è stato ottenuto empiricamente 
//a questo determinato valore c'è abbastanza luminosità nella stanza
//quindi non è necessario accendere la smart lamp
#define PHOTORESISTORTHRESHOLD  45000

int32 value_digit_Photo;

uint8 ch_received;
uint8 SendBytesFlag=0;


CY_ISR(Custom_ISR_ADC)
{
    //rimetto la interrupt a 0
    Timer_ReadStatusRegister();
    
    //leggo il valore del photoresistor
    AMux_FastSelect(PHOTORESISTOR_MUX);
    
    value_digit_Photo= ADC_DelSig_Read32();
    
    //sistemo i valori letti
   if(value_digit_Photo<0)
    {
        value_digit_Photo=0;
    }
    
    if(value_digit_Photo>65535)
    {
        value_digit_Photo=65535;
    }
    
    //byte0 header, byte 1 e 2 salvo il dato del photoresistor, byte 5 tail
    DataBuffer[1]=value_digit_Photo>>8;
    DataBuffer[2]=value_digit_Photo & 0xFF;
    
    //se il photoresistor legge una bassa luminosità,
    //allora nel main attivo il led 
    if(value_digit_Photo < PHOTORESISTORTHRESHOLD)
    {
        PhotoresistorThresholdFlag=1;
    }   
    
    else
    {
        //spengo il LED
        PWM_LED_WriteCompare(MAX_PERIOD);
    }
    
    
    //se il photoresistor legge una luminosità inferiore alla soglia di luminosità minima
    //allora procede a leggere il valore del potenziometro
    if(PhotoresistorThresholdFlag)
    {
        AMux_FastSelect(POTENTIOMETER_MUX);
        
        //leggo il valore
        value_digit_PWM= ADC_DelSig_Read32();
    
        //sistemo i valori letti
        if(value_digit_PWM<0)
        {
             value_digit_PWM=0;
        }
    
        if(value_digit_PWM>65535)
        {
            value_digit_PWM=65535;
        }
    
        //byte0 header,byte 3 e 4 salvo il dato del potenziometro, byte 5 tail
        DataBuffer[3]=value_digit_PWM>>8;
        DataBuffer[4]=value_digit_PWM & 0xFF;
        
        // attivo la flag
        PacketReadyFlag=1;
    }  
}


CY_ISR(Custom_ISR_RX)
{
    //leggo carattere sulla UART 
    ch_received=UART_GetChar();
    
    switch(ch_received)
    {
        //start sampling sensors
        case 'B':
        case 'b':
            SmartLampOnFlag=1; 
            //accendo led integrato che rimane acceso sempre
            //mentre leggo i valori sull'ADC
            LED_UART_Write(LED_ON);
            //entro nell'ISR ADC ad ogni overflow del timer 
            Timer_Start();
            break;
            
        //stop, do not measure anything    
        case 'S':
        case 's':
            SmartLampOnFlag=0;
            //spengo led integrato
            LED_UART_Write(LED_OFF);
            //non entro mai nell'ISR ADC
            Timer_Stop();
            break;
            
        default:
            break;
    }
}


/* [] END OF FILE */
