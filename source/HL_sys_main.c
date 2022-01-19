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
// ================================================================================
// Section: Header File (Added by HALCOGEN)
// Purpose: General Definitions relevant for all drivers (HALCOGEN-created)
// ================================================================================
/* USER CODE END */

/* Include Files */

#include "HL_sys_common.h"

/* USER CODE BEGIN (1) */
// ================================================================================
// Section: Header Files (Added by user)
// Purpose: Provides functions and capabilities needed for program execution
// ================================================================================
#include <stdlib.h>         // Memory Allocation/Freeing functions
#include "HL_gio.h"         // General Input/Output capabilities (HALCOGEN-created)
#include "HL_sys_core.h"    // System Core Interface functions (HALCOGEN-created)
#include "HL_rti.h"         // Real-Time Interrupt (HALCOGEN-created)
#include "HL_adc.h"         // ADC Conversion capabilities (HALCOGEN-created)
#include "HL_het.h"         // Provides PWM capabilities (HALCOGEN-created)
#include "HL_can.h"         // CAN communication capabilities (HALCOGEN-created)
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
// ================================================================================
// Section: Pin Assignments
// Purpose: Defines pin values associated with the microcontroller
// ================================================================================
// GIOA (General Purpose Pins)
#define BMSFault 0              // BMS Fault Indicator
#define BSPDFault 1             // BSPD Fault Indicator
#define IMDFault 2              // IMD Fault Indicator
#define StartButton 3           // From Cockpit (Low Voltage Master Switch)
#define TVToggle 4              // Torque Vector Switch (From DASH)
#define RegenToggle 5           //
#define TractionToggle 6        // Traction Switch (From DASH)

// N2HET2
#define RTDS 23                 // Speaker Output
#define BrakeLight 11           // Brake Light Output
#define TimeDelay 10            //
#define BMSLED 9                // LED Fault Output
#define BSPDLED 14              // LED Fault Output
#define IMDLED 22               // LED Fault Output

// ================================================================================
// Section: Global Variables
// Purpose: Variables that will be passed to other functions for processing
// ================================================================================
int bseFlag = 0;

/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */
// ================================================================================
// FUNCTION: main
// Purpose: Starting point of program
// ================================================================================
// Section: Startup Initializations
// Purpose: Start up microcontroller and initiate modules needed
// ================================================================================
    // Module initializations
    gioInit();
    rtiInit();
    adcInit();
    hetInit();
    canInit();

    // Enable IRQ
    _enable_IRQ_interrupt_();

    // Setup RTI
    rtiEnableNotification(rtiREG1, rtiNOTIFICATION_COMPARE0);

    //Set Initial state of User LEDs
    gioSetBit(gioPORTB, 6, 1);
    gioSetBit(gioPORTB, 7, 0);

    // Start RTI counter
    rtiStartCounter(rtiREG1, rtiCOUNTER_BLOCK0);

    // Run forever on standby (Will be interrupted to change states)
    while (1);

/* USER CODE END */

    return 0;
}

/* USER CODE BEGIN (4) */
// ================================================================================
// Section: Secondary Functions
// Purpose: Used during execution of main program. Executed via interrupt signals
// ================================================================================
// FUNCTION: fault
// Purpose: Zeros all outputs when there is a fault detected. Specific behavior
//          is generated depending on the caller value (which fault detected)
// Caller Value      Fault Type
//      0               BMS
//      1               BSPD
//      2               IMD
//      3          Redundant ADC fail
// ================================================================================
void fault(int caller)
{
    // Set Dashboard (DASH) LEDs
    if(caller == 0)
        gioSetBit(hetPORT2, BMSLED, 1);
    if(caller == 1)
        gioSetBit(hetPORT2, BSPDLED, 1);
    if(caller == 2)
        gioSetBit(hetPORT2, IMDLED, 1);
    // Disable Notifications if necessary
    if(caller == 3)
        rtiStopCounter(rtiREG1, rtiCOUNTER_BLOCK0);
    // Wait forever (MCU must be reset via Master Switch to restart execution)
    while(1);
}
// ================================================================================
// Function: rtiNotification
// Purpose: Implements Real Time Interrupt notifications
// ================================================================================
void rtiNotification(rtiBASE_t *rtiREG, uint32 notification)
{
gioToggleBit(gioPORTB, 6);
gioToggleBit(gioPORTB, 7);
}
// ================================================================================
// Function: checkPackets
// Purpose: Verify that packets sent and received over CAN are intended values
// ================================================================================
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
// ================================================================================
// Functions: Extra CAN Functions
// Purpose: All functions below need to be declared for CAN to function, but are
// only used when using interrupt vectors for CAN. Still need to verify that packets
// sent and received over CAN are intended values
// ================================================================================

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

/* USER CODE END */
