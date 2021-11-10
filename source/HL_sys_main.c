/** @file HL_sys_main.c 
*   @brief Application main file
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com  
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Include Files */

#include "HL_sys_common.h"

/* USER CODE BEGIN (1) */
// header files for GIO, RTI, and CAN services
#include "HL_gio.h"
#include "HL_rti.h"
#include "HL_sys_core.h"
#include "HL_can.h"
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
// Pin Values
/*
int APPS_PIN = ;
int BSE_PIN = ;
int INTERRUPT_PIN = ;
// Boolean Variables
bool APPS = 0;
bool BSE = 0;
bool INTERRUPT = 0;
*/
/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */
    // Initializations
    gioInit();                  // General Input Output Pins
    rtiInit();                  // ReTurn from Interrupt Functionality
    canInit();                  // //Initiate DCAN module


    // Enable IRQ
    _enable_IRQ_interrupt_();

    // Enable RTI Notification
    rtiEnableNotification(rtiREG1, rtiNOTIFICATION_COMPARE0);


    //Set Initial state of User LEDs
    gioSetBit(gioPORTB, 6, 1);
    gioSetBit(gioPORTB, 7, 0);

    // Start RTI counter
    rtiStartCounter(rtiREG1, rtiCOUNTER_BLOCK0);

    /*Transmit data within tx_data buffer
      canREG1 refers to DCAN module 1
      canMESSAGE_BOX1 refers to CAN ID 0x01
      tx_data is the data buffer being sent over CAN
    */
    canTransmit(canREG1,canMESSAGE_BOX1, tx_data);

    // Run Forever (Will be interrupted to change states)
    while (1);

    /* Intended for main execution but not sure where to place yet
    APPS = APPSINDICATOR();
    BSE = BSEINDICATOR();
    INTERRUPT = INTERRUPTINDICATOR();
    while(1) {
        if (APPS)  {
            gioToggleBit(gioPORTA, APPS_PIN;
        }
                         
        if (BSE)  {
            gioToggleBit(gioPORTA, BSE_PIN;
        }
        if (INTERRUPT))  {
            gioToggleBit(gioPORTA, INTERRUPT_PIN;
        }
    }
    */

/* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */
//RTI Notification implementation
void rtiNotification(rtiBASE_t *rtiREG, uint32 notification)
{
gioToggleBit(gioPORTB, 6);
gioToggleBit(gioPORTB, 7);
}

//Function to verify that packets sent and received are intended values
uint32_t checkPackets(uint8_t *src_packet,uint8_t *dst_packet,uint32_t psize){
    uint32_t err=0;
    uint32_t cnt=psize;

    while(cnt--){
        if((*src_packet++) != (*dst_packet++)){
            err++;
        }
    }
    return(err);
}

// All functions below need to be declared for CAN to function, but
// are only used when using intterupt vectors for CAN
void canMessageNotification(canBASE_t *node, uint32_t messageBox){
    return;
}

void canErrorNotification(canBASE_t *node, uint32_t messageBox){
    return;
}

void esmGroup1Notification(unsigned channel){
    return;
}

void esmGroup2Notification(unsigned channel)
{
    return;
}

/* All functions below are functions intended for operation
// Detect APPS
bool APPSINDICATOR() {
    // fill
}

bool BSEINDICATOR() {
    // fill
}

bool INTERRUPTINDICATOR() {
    // fill
}
*/
/* USER CODE END */
