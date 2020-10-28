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

#include "project.h"
#include "InterruptRoutines.h"
#include "stdio.h"

#define LED_ON                      1
#define LED_OFF                     0

#define PHOTORESISTOR_MUX           0
#define POTENTIOMETER_MUX           1

//il valore è stato ottenuto empiricamente oscurando il photoresistor e leggendo il valore tramite debug
#define PHOTORESISTORTHRESHOLD  20000

int32 value_mv_PWM;

int32 value_digit_Photo;
int32 value_mv_Photo;

uint8 ch_received;
uint8 SendBytesFlag=0;

uint8 chan=0;

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
    
    //byte0 header, byte 5 tail
    DataBuffer[1]=value_digit_Photo>>8;
    DataBuffer[2]=value_digit_Photo & 0xFF;
    
    //se il photoresistor legge una bassa luminosità,
    //allora nel main attivo il led 
    if(value_digit_Photo < PHOTORESISTORTHRESHOLD)
    {
        PhotoresistorThresholdFlag=1;
    }   
    
    
//----------------------------------------    
    PhotoresistorThresholdFlag=1;
//----------------------------------------
    
    //se il photoresistor legge una luminosità oltre la soglia
    //allora procedo a leggere il valore del potenziometro
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
    
        //value_mv_PWM= ADC_DelSig_CountsTo_mVolts(value_digit_PWM);
        
        //byte0 header, byte 5 tail
        DataBuffer[3]=value_digit_PWM>>8;
        DataBuffer[4]=value_digit_PWM & 0xFF;
        
        PacketReadyFlag=1;
    }  
}


CY_ISR(Custom_ISR_RX)
{
    ch_received=UART_GetChar();
    
    switch(ch_received)
    {
        //start sampling sensors
        case 'B':
        case 'b':
            SmartLampOnFlag=1;        
            LED_UART_Write(LED_ON);
            //entro nell'ISR ADC all'overflow del timer 
            Timer_Start();
            break;
            
        //stop, do not measure anything    
        case 'S':
        case 's':
            SmartLampOnFlag=0;        
            LED_UART_Write(LED_OFF);
            //non entro mai nell'ISR ADC
            Timer_Stop();
            break;
            
        default:
            break;
    }
}




/* [] END OF FILE */
