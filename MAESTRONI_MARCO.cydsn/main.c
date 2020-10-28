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


#define MAX_PERIOD            65535

#define LED_ON                    1
#define LED_OFF                   0


int main(void)
{
    CyGlobalIntEnable; 
    //attivo le varie funzioni e interrupts
    ADC_DelSig_Start();
    UART_Start();
    
    //inizializzo AMux
    AMux_Start();
    
    //attivo le varie interrupts
    //isr_ADC_StartEx(Custom_ISR_ADC);
    isr_RX_StartEx(Custom_ISR_RX);   
 
    //inizializzo le flags
    PacketReadyFlag=0;
    PhotoresistorThresholdFlag=0;
    SmartLampOnFlag=0;
    
    //inizializzo le variabili
    value_digit_PWM=0;
    
    //init
    DataBuffer[0]=0xA0;
    DataBuffer[TRANSMIT_BUFFER_SIZE-1]=0xC0;
    
// Start the ADC conversion----------
    //ADC_DelSig_StartConvert();
//-------------- forse non necessario
    
    //inializzo il LED_PWM e il LED_UART spenti
    PWM_LED_Start();
    
    PWM_LED_WriteCompare(MAX_PERIOD);
    LED_UART_Write(LED_OFF);
    
    //-------------
    //Timer_Start();
    //-----------
    
    isr_ADC_StartEx(Custom_ISR_ADC);
    
    for(;;)
    {   
        // se tramite UART leggo il comando di accensione della smart lamp
        //e il photoresistor supera una soglia di luminosità letta nell'isr
        //allora imposto la luminosità del LED tramite PWM con il valore letto sul potenziometro
        if(SmartLampOnFlag && PhotoresistorThresholdFlag)
        {
            PhotoresistorThresholdFlag=0;
            
            //avendo letto il valore del potenziometro nell'isr,
            //setto la luminosità del LED tramite PWM
            PWM_LED_WriteCompare(MAX_PERIOD - value_digit_PWM);
        }
        if(!SmartLampOnFlag)
        {
            //spengo il LED
            PWM_LED_WriteCompare(MAX_PERIOD);
        }
        
        
        if(PacketReadyFlag)
        {
            // invio dati
            UART_PutArray(DataBuffer,TRANSMIT_BUFFER_SIZE);
            PacketReadyFlag=0;
        }    
    }
}

/* [] END OF FILE */
