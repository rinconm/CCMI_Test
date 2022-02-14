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
/*********************************************************************************/
/*********************************************************************************/
/* Program: HL_sys_main.c                                                        */
/* Purpose: Source code for the main microcontroller of the Ampeater EVO system  */
/* Authors: Anteater Electric Racing Team, Marcos Rincon                         */
/*                                                                               */
/* Version History                                                               */
/* 01/28/22 MR  Modified fault functions and commented out descriptions          */
/* 12/05/21 MR  Added initialization functions                                   */
/* 11/02/19 JM  Initial Version                                                  */
/*********************************************************************************/
/*********************************************************************************/

/*********************************************************************************/
/* Section: Header Files                                                         */
/* Purpose: These files define keywords, constants, and definitions for custom   */
/*          types, symbols, and more for program execution. Provides module      */
/*          abstraction to improve program readability.                          */
/* Notes  : HALCOGEN-generated header files start with HL_. These files were     */
/*          automatically generated when choosing desired modules in the HALCOGEN*/
/*          IDE.                                                                 */
/*          Each line of code gets replaced with the .h file during the pre-     */
/*          processing step of the compilation process.
/*********************************************************************************/
/* USER CODE END */

/* Include Files */

#include "HL_sys_common.h"

/* USER CODE BEGIN (1) */

/* System libraries */
#include <stdlib.h>         /* Static Memory Allocation/Freeing functions */
/* HALCOGEN-generated header files */
#include "HL_gio.h"         /* General Input/Output capabilities */
#include "HL_sys_core.h"    /* System Core Interface functions */
#include "HL_rti.h"         /* Real-Time Interrupt functions */
#include "HL_adc.h"         /* Analog to Digital (ADC) Conversion capabilities */
#include "HL_het.h"         /* Pulse Width Modulation (PWM) capabilities) */
#include "HL_can.h"         /* CAN communication capabilities */

/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
/*********************************************************************************/
/* Section: Global Constants/Parameters/Variables                                */
/* Purpose: Variables/values that will be passed to functions for processing     */
/*********************************************************************************/
/** Microcontroller Pin Assignments **/
/* GIOA (General Purpose Pins) */
#define BMSFault       0        /* BMS Fault Indicator */
#define BSPDFault      1        /* BSPD Fault Indicator */
#define IMDFault       2        /* IMD Fault Indicator */
#define StartButton    3        /* From Cockpit (Low Voltage Master Switch) */
#define TVToggle       4        /* Torque Vector Switch (From DASH) */
#define RegenToggle    5        /* */
#define TractionToggle 6        /* Traction Switch (From DASH) */
/* N2HET2 Pins */
#define BMSLED      9           /* LED Fault Output */
#define BrakeLight 11           /* Brake Light Output */
#define BSPDLED    14           /* LED Fault Output */
#define IMDLED     22           /* LED Fault Output */
#define RTDS       23           /* Speaker Output */

/* System Parameters */
#define BRAKE_APPLIED_CUTOFF 400
#define BATTERY_TEMP_FAN_TURNON 0xFFF
#define DEADZONE_LOW 1536
#define DEADZONE_HIGH 2560

/* N2HET2 Parameters */
#define TimeDelay 10

/* PWM Channel Parameters */
#define PWM_PERIOD 100
#define ThrottleL pwm0
#define ThrottleR pwm1
#define RegenL    pwm2
#define RegenR    pwm3
#define Fans      pwm4

/* Global scope variables */
int bseFlag = 0;                /* Brake Systen Encoder Fault Flag */
int timeDelayFlag = 0;          /* */
int compare2Counter = 0;        /* */

/*********************************************************************************/
/* Section: User-Defined Function Declarations                                   */
/* Purpose: Declares the functions/algorithms that our team created. (Declared   */
/*          here and defined below).                                             */
/*********************************************************************************/
/* Converts input analog signals from  to digital values in an array */
void adcConversion(unsigned int *adcArray);
/* Check if there's an Acceleration Pedal Position Sensor fault */
int APPSFault(int);
/* Check if brakes are calibrated correctly (must be under the defined cutoff value */
int brakeCheck();
/* Check if there's a Brake System Encoder fault */
int BSEFault(unsigned int accel1,
             unsigned int accel2,
             unsigned int brake);
/* Stalls the whole program if a BMS, BSPD, IMD, or Redundant ADC fault detected */
void fault(int caller);
/* Outputs the motor torque generated by the TVA to the motor controller board*/
void motorOutput(unsigned int *outputArray);
/* Initializes PWM values for motor operation to zero */
void pwmSetup();
/* Startup required modules */
void startup();
/* The torque vector algorithm used to determine motor torque output */
void TVA(unsigned int *outputArray,
         unsigned int *adcArray);
/* Receive data being transmitted over CAN bus  */
void getAllCANData(uint8_t *canData);
/* Broadcast data over CAN bus */
void sendAllDataOBD(uint8_t *canData);
/* Acknowledge packets received over CAN bus */
uint32_t checkPackets(uint8_t  *src_packet,
                      uint8_t  *dst_packet,
                      uint32_t psize);

/* USER CODE END */

/* USER CODE BEGIN (3) */
/*********************************************************************************/
/*********************************************************************************/
/* FUNCTION: main                                                                */
/* Purpose : Global starting point of entire program                             */
/*********************************************************************************/
/*********************************************************************************/
int main(void)
{
/*********************************************************************************/
/* Section: Initialization commands                                              */
/* Purpose: Prepare mcu by initiating modules needed for program execution       */
/*********************************************************************************/
    /* Pin Module initializations */
    gioInit();
    rtiInit();
    adcInit();
    hetInit();
    canInit();

    /** Setup RTI Functionality **/
    /* Enable ADC/Torque Interrupt */
    rtiEnableNotification(rtiREG1, rtiNOTIFICATION_COMPARE0);
    /* Enable Battery Management Interrupt */
    rtiEnableNotification(rtiREG1, rtiNOTIFICATION_COMPARE1);
    /* Enable utility/time delay notification */
    rtiEnableNotification(rtiREG1, rtiNOTIFICATION_COMPARE2);

    /* Enable IRQ and FIQ functionality/notifications */
    _enable_interrupt_();

    /* Calibrate ADC */
    adcCalibration(adcREG1);
    adcCalibration(adcREG2);

    //Setup PWM outputs
    pwmSetup();

    //Setup for time delay
        //After a set time delay we need to set time delay pin to high
        //Leave time delay pin unused after
        //Need to initially have start button disabled
        //Wait for time to pass and call rtinotification compare2 using rticounter 1
    gioDisableNotification(gioPORTA, StartButton);  //Make sure start button is disabled
    /* Start RTI counter (Timer) */
    rtiStartCounter(rtiREG1, rtiCOUNTER_BLOCK0);

/*********************************************************************************/
/* Section: Standby mode                                                         */
/* Purpose: Main state of the program. Keeps looping until an interrupt vector   */
/*          is detected from the interrupt pins.                                 */
/*********************************************************************************/
    /* Run forever (Will be interrupted to change states) */
    while (1);

/* USER CODE END */

    return 0;
}

/* USER CODE BEGIN (4) */
/*********************************************************************************/
/*********************************************************************************/
/* Section: Secondary Functions                                                  */
/* Purpose: Used during execution of main program. Called  via interrupt signals */
/*********************************************************************************/
/*********************************************************************************/

//gioNotification
//Function that is called every time a gio Interrupt is issued
//Function has different functionality based on which bit causes
//the interrupt
//Bits 0, 1, 2 -> BMS, IMD, and BSPD faults respectively
//Bit 3 -> Start button
void gioNotification(gioPORT_t *port, uint32 bit)
{
    if(bit==BMSFault || bit==IMDFault || bit==BSPDFault)  //Fault causing inputs
        fault(bit);

    if(bit==StartButton) //Start Button signal
    {
        //Check Brake
        if(brakeCheck() == 1)
        {
            //Disable Start button
            gioDisableNotification(gioPORTA, StartButton);
            //Run Start Sequence
            startup();
        }
        else
            return;
    }
}

/*********************************************************************************/
/* Function: rtiNotification                                                     */
/* Purpose: Implements Real Time Interrupt notifications                         */
/*********************************************************************************/
//rtiNotification
//Function that is called every time an RTI interrupt is issued
//Function has different functionality based on which RTI interrupt
//calls the function.
//rtiNOTIFICATION_COMPARE0 --> ADC translation and torque vectoring
//rtiNOTIFICATION_COMPARE1 --> Battery Fan management
void rtiNotification(rtiBASE_t *rtiREG, uint32 notification)
{
    if(notification == rtiNOTIFICATION_COMPARE0) //Torque Function
    {
        unsigned int adcArray[4];
        unsigned int outputArray[4];
        //Output array format
        //  [0]-ThrottleL  [1]-ThrottleR
        //  [2]-RegenL     [3]-RegenR

        //Get ADC Data
        adcConversion(adcArray);

        //TODO: Calibrate adc values once on vehicle (maybe do in adc conversion function)
        int adcDiff = adcArray[0] - adcArray[1];
        if (adcDiff < 0)
            adcDiff *= -1;
        //Check ADC Data
        if(APPSFault(adcDiff) || BSEFault(adcArray[0],adcArray[1],adcArray[2]))
        {
            //Set motor output = 0
            outputArray[0] = 0;
            outputArray[1] = 0;

            //Should we also zero Regen?
            outputArray[2] = 0;
            outputArray[3] = 0;
            //Do we want to set throttle/output values in an array and then output once?
        }
        else
        {
            //Run Torque or Regen Vectoring Algorithm
            TVA(outputArray, adcArray);
        }

        //Output to motors
        motorOutput(outputArray);

        //Misc output functions (brake light)
        if(adcArray[2] > BRAKE_APPLIED_CUTOFF)
            gioSetBit(hetPORT2, BrakeLight, 1);
        else
            gioSetBit(hetPORT2, BrakeLight, 0);

        //Input from CAN and output to OBD2
        //Converts data to percentages
        uint8_t throttleLPercent = outputArray[0];
        uint8_t throttleRPercent = outputArray[1];
        uint8_t regenLPercent = outputArray[2];
        uint8_t regenRPercent = outputArray[3];
        uint8_t brakePercent = (float)((adcArray[2]/4096) * 100);
        uint8_t steeringPercent = (float)((adcArray[3]/4096) * 100);

        uint8_t canData[94];
        //Populate canData buffer with all data from CANBus and throttle,brake,and steering % values
        getAllCANData(canData);
        canData[0] = (((adcArray [0] + adcArray[1])/2) * 100);
        canData[1] = brakePercent;
        canData[2] = steeringPercent;

        sendAllDataOBD(canData);

    }
    if(notification == rtiNOTIFICATION_COMPARE1) //Battery Management
    {
        //TODO: maybe move these variable elsewhere
        uint8_t bms1_temp[6];
        uint8_t bms2_temp[6];
        unsigned int internalTemp1;
        unsigned int internalTemp2;
//        unsigned int ext1Temp1;
//        unsigned int ext2Temp1;
//        unsigned int ext1Temp2;
//        unsigned int ext2Temp2;

        //Get data from CAN Bus
        canGetData(canREG1,canMESSAGE_BOX1,bms1_temp);
        canGetData(canREG1,canMESSAGE_BOX2,bms2_temp);

        //TODO: Internal temp may not be desired data for this calculation
        //Evaluate data and figure out new output
        internalTemp1 = bms1_temp[0];
        internalTemp1 = internalTemp1 << 8;
        internalTemp1 = internalTemp1 | bms1_temp[1];

        internalTemp2 = bms2_temp[0];
        internalTemp2 = internalTemp2 << 8;
        internalTemp2 = internalTemp2 | bms2_temp[1];

        //Output to PWM
        //TODO: Add "better" algorithm if necessary, and find "turnon value"
        if((internalTemp1 || internalTemp2) > BATTERY_TEMP_FAN_TURNON)
            pwmSetDuty(hetRAM1,Fans,100);
        else
            pwmSetDuty(hetRAM1, Fans, 0);
    }
    if(notification == rtiNOTIFICATION_COMPARE2 && timeDelayFlag == 0) //Time Delay
    {
        compare2Counter ++;
        if(compare2Counter == 20)
        {
            rtiStopCounter(rtiREG1, rtiCOUNTER_BLOCK1);     //Stop Counter 1
            rtiResetCounter(rtiREG1, rtiCOUNTER_BLOCK1);

            gioSetBit(hetPORT2, TimeDelay, 1);              //Set Time Delay pin to high

            timeDelayFlag = 1;                              //Set Time Delay flag to high
            compare2Counter = 0;

            gioEnableNotification(gioPORTA, StartButton);   //Enable Start Button
        }
    }
    if(notification == rtiNOTIFICATION_COMPARE2 &&timeDelayFlag == 1)//RTDS
    {
        compare2Counter ++;
        if(compare2Counter == 6)
        {
            rtiStopCounter(rtiREG1, rtiCOUNTER_BLOCK1);
            rtiResetCounter(rtiREG1, rtiCOUNTER_BLOCK1);

            gioSetBit(hetPORT2, RTDS, 0);   //Disable RTDS

            compare2Counter = 0;
            //Need to set all DASH LEDS low
            gioSetBit(hetPORT2, BMSLED, 0);
            gioSetBit(hetPORT2, IMDLED, 0);
            gioSetBit(hetPORT2, BSPDLED, 0);

            //Start RTI Counter 0
            rtiStartCounter(rtiREG1, rtiCOUNTER_BLOCK0);
        }
        else
        {
            gioToggleBit(hetPORT2, BMSLED);
            gioToggleBit(hetPORT2, IMDLED);
            gioToggleBit(hetPORT2, BSPDLED);
        }
    }
}

/*********************************************************************************/
/* FUNCTION: fault                                                               */
/* Purpose : Stalls program when there is a fault detected. Specific output LED  */
/*           is set on depending on the caller value (which fault is detected)   */
/*                                                                               */
/* Caller Value      Fault Type                                                  */
/*      0               BMS                                                      */
/*      1               BSPD                                                     */
/*      2               IMD                                                      */
/*      3          Redundant ADC fail                                            */
/*********************************************************************************/
void fault(int caller)
{
    /* Set Dashboard (DASH) LEDs */
    if(caller == 0)
        gioSetBit(hetPORT2, BMSLED, 1);
    if(caller == 1)
        gioSetBit(hetPORT2, BSPDLED, 1);
    if(caller == 2)
        gioSetBit(hetPORT2, IMDLED, 1);
    /* Disable Notifications if necessary */
    if(caller == 3)
        rtiStopCounter(rtiREG1, rtiCOUNTER_BLOCK0);
    /* Wait forever (MCU must be reset via Master Switch to restart execution) */
    while(1);
}

//startup
//Function for running all startup functions after the start
//button has been pressed
void startup()
{
    //Turn on RTDS for 1-3 seconds
    gioSetBit(hetPORT2, RTDS, 1);
    rtiStartCounter(rtiREG1, rtiCOUNTER_BLOCK1);    //Start RTI Counter

    //Blink dash LEDs
    //This happens on the RTI interrupt
}

//brakeCheck
//Checks if the brake is applied above BRAKE_APPLIED_CUTOFF
//returns 1 if brake is applied
//returns 0 if brake is not applied
int brakeCheck()
{
    unsigned int adcArray[4];
    unsigned int brakeValue;
    adcConversion(adcArray);
    brakeValue = adcArray[2];

    if(brakeValue > BRAKE_APPLIED_CUTOFF)
        return 1;
    else
        return 0;
}

//adcConversion
//Runs redundant ADC on all 4 analog inputs, checks for greater than 10% difference
//between redundant ADC inputs and returns the averaged ADC values in an output array
//
//Output array is ordered as follows
//[0]-Throttle1 [1]-Throttle2
//[2]-Brake     [3]-Angle
void adcConversion(unsigned int *adcArray)
{
    adcData_t adc1Array[4], adc2Array[4];
    unsigned int brake[2], throttle1[2], throttle2[2], angle[2];
    unsigned int tempValue1, tempValue2;
    int num1, num2, i, diff;

    //Start ADC Conversion
    adcStartConversion(adcREG1, adcGROUP1);
    adcStartConversion(adcREG2, adcGROUP1);

    //Wait for ADC to complete
    while(!adcIsConversionComplete(adcREG1, adcGROUP1) ||
            !adcIsConversionComplete(adcREG2, adcGROUP1) );

    //Get ADC Data
    num1 = adcGetData(adcREG1, 1U, adc1Array);
    num2 = adcGetData(adcREG2, 1U, adc2Array);

    if(num1 != num2)
        fault(3);

    for(i = 0; i<num1; i++)
    {
        tempValue1 = adc1Array[i].value;
        tempValue2 = adc2Array[i].value;

        switch(i)
        {
        case 0:
            brake[0] = tempValue1;
            throttle2[1] = tempValue2;
            break;
        case 1:
            angle[0] = tempValue1;
            brake[1] = tempValue2;
            break;
        case 2:
            throttle1[0] = tempValue1;
            angle[1] = tempValue2;
            break;
        case 3:
            throttle2[0] = tempValue1;
            throttle1[1] = tempValue2;
            break;
        }
    }

    //Check and average throttle 1
    diff = throttle1[0] - throttle1[1];
    if(diff < 0)
        diff = diff * -1;
    if(diff > 410)
        fault(3);
    adcArray[0] = (throttle1[0]+throttle1[1])/2;

    //Check and average throttle 2
    diff = throttle2[0] - throttle2[1];
    if(diff < 0)
        diff = diff * -1;
    if(diff > 410)
        fault(3);
    adcArray[1] = (throttle2[0]+throttle2[1])/2;

    //Check and average brakes
    diff = brake[0] - brake[1];
    if(diff < 0)
        diff = diff * -1;
    if(diff > 410)
        fault(3);
    adcArray[2] = (brake[0]+brake[1])/2;

    //Check and average steering angle
    diff = angle[0] - angle[1];
    if(diff < 0)
        diff = diff * -1;
    if(diff > 410)
        fault(3);
    adcArray[3] = (angle[0]+angle[1])/2;

    //Return array of averaged ADC values
    //return outputArray;
}

//APPSFault
//Used for checking the difference between two throttle inputs
//Returns 1 if greater than 10% diff => fault
//Returns 0 if no fault
int APPSFault(int adcDiff)
{
    if (adcDiff > 410)
        return 1;
    else
        return 0;
}

//BSEFault
//Used for checking if throttle and brake are pressed at the same time (indicating a short circuit)
//Return 1 if throttle is > 25% and brake is pressed, until brake is released and throttle is < 5%
//Return 0 if no fault
int BSEFault(unsigned int accel1, unsigned int accel2, unsigned int brake)
{
    if((bseFlag) && (accel1 > 205 || accel2 > 205))//205 is 5% of 4096
    {
        return 1;
    }
    else if((accel1 > 1024 || accel2 > 1024) && (brake > BRAKE_APPLIED_CUTOFF))//1024 is 25% of 4096
    {
        bseFlag = 1;
        return 1;
    }
    else if((bseFlag) && (accel1 < 205 && accel2 < 205))//205 is 5% of 4096
    {
        bseFlag = 0;
        return 0;
    }
    //NEED A DEFAULT RETURN SIGNAL
    return;
}

//pwmSetup
//Used for initial setup of pwm signals
//Only exists for the sake of making code look cleaner
void pwmSetup()
{
    pwmSetDuty(hetRAM1, ThrottleL, 0);

    pwmSetDuty(hetRAM1, ThrottleR, 0);

    pwmSetDuty(hetRAM1, RegenL, 0);

    pwmSetDuty(hetRAM1, RegenR, 0);

    pwmSetDuty(hetRAM1, Fans, 0);
}

//Motor Output
//Handles setting PWM values for motor output
void motorOutput(unsigned int *outputArray)
{
    pwmSetDuty(hetRAM1, ThrottleL, outputArray[0]);
    pwmSetDuty(hetRAM1, ThrottleR, outputArray[1]);
    pwmSetDuty(hetRAM1, RegenL, outputArray[2]);
    pwmSetDuty(hetRAM1, RegenR, outputArray[3]);
}

//TVA
//Output array format
//  [0]-ThrottleL  [1]-ThrottleR
//  [2]-RegenL     [3]-RegenR
void TVA(unsigned int *outputArray, unsigned int *adcArray)
{
    //Unpack and process input array
    unsigned int Angle = adcArray[3];
    float TReq = (adcArray[0] + adcArray[1]) / 2;    //Average two throttle inputs
    float lFactor, rFactor, m;

    //Do some math
    m = (0.6/DEADZONE_LOW);     //Factor used in TVA
    TReq = (TReq/4096) * 100;   //Throttle request as a percentage, represented 0-100

    //Run the Torque Vectoring Algorithm
    if(Angle <= DEADZONE_HIGH && Angle >= DEADZONE_LOW) //If Angle is within the deadzone
    {
        lFactor = 1;
        rFactor = 1;
    }
    else if(Angle < DEADZONE_LOW || Angle > DEADZONE_HIGH)//If Angle is outside of deadzone
    {
        if(Angle < DEADZONE_LOW)    //Turning Left
        {
            lFactor = (m * Angle) + 0.4;
        }
        else
        {
            lFactor = 1;
        }

        if(Angle > DEADZONE_HIGH)   //Turning Right
        {
            rFactor = ((-m) * Angle) + (0.4 - (4096 * -m));
        }
        else
        {
            rFactor = 1;
        }

    }
    //Populate output array
    outputArray[0] = TReq * lFactor;    //ThrottleL
    outputArray[1] = TReq * rFactor;    //ThrottleR
    outputArray[2] = 0;
    outputArray[3] = 0;
}

void getAllCANData(uint8_t *canData){
    canGetData(canREG1, canMESSAGE_BOX1,canData+6);
    canGetData(canREG1, canMESSAGE_BOX2,canData+12);
    canGetData(canREG1, canMESSAGE_BOX3,canData+18);
    canGetData(canREG1, canMESSAGE_BOX4,canData+21);
    canGetData(canREG1, canMESSAGE_BOX5,canData+24);
    canGetData(canREG1, canMESSAGE_BOX6,canData+27);
    canGetData(canREG1, canMESSAGE_BOX7,canData+30);
    canGetData(canREG1, canMESSAGE_BOX8,canData+36);
    canGetData(canREG1, canMESSAGE_BOX9,canData+42);
    canGetData(canREG1, canMESSAGE_BOX10,canData+48);
    canGetData(canREG1, canMESSAGE_BOX11,canData+54);
    canGetData(canREG1, canMESSAGE_BOX12,canData+60);
    canGetData(canREG1, canMESSAGE_BOX13,canData+66);
    canGetData(canREG1, canMESSAGE_BOX14,canData+72);
    canGetData(canREG1, canMESSAGE_BOX15,canData+78);
    canGetData(canREG1, canMESSAGE_BOX16,canData+82);
    canGetData(canREG1, canMESSAGE_BOX17,canData+86);
    canGetData(canREG1, canMESSAGE_BOX18,canData+90);
}

void sendAllDataOBD(uint8_t *Data){
    canTransmit(canREG2, canMESSAGE_BOX1,Data);
    canTransmit(canREG2, canMESSAGE_BOX2,Data+6);
    canTransmit(canREG2, canMESSAGE_BOX3,Data+12);
    canTransmit(canREG2, canMESSAGE_BOX4,Data+18);
    canTransmit(canREG2, canMESSAGE_BOX5,Data+21);
    canTransmit(canREG2, canMESSAGE_BOX6,Data+24);
    canTransmit(canREG2, canMESSAGE_BOX7,Data+27);
    canTransmit(canREG2, canMESSAGE_BOX8,Data+30);
    canTransmit(canREG2, canMESSAGE_BOX9,Data+36);
    canTransmit(canREG2, canMESSAGE_BOX10,Data+42);
    canTransmit(canREG2, canMESSAGE_BOX11,Data+48);
    canTransmit(canREG2, canMESSAGE_BOX12,Data+54);
    canTransmit(canREG2, canMESSAGE_BOX13,Data+60);
    canTransmit(canREG2, canMESSAGE_BOX14,Data+66);
    canTransmit(canREG2, canMESSAGE_BOX15,Data+72);
    canTransmit(canREG2, canMESSAGE_BOX16,Data+78);
    canTransmit(canREG2, canMESSAGE_BOX17,Data+82);
    canTransmit(canREG2, canMESSAGE_BOX18,Data+86);
    canTransmit(canREG2, canMESSAGE_BOX19,Data+90);
}

/*********************************************************************************/
/* Function: checkPackets                                                        */
/* Purpose: Verify that packets sent and received over CAN are intended values   */
/* Return : Unsigned 32-bit integer                                              */
/*********************************************************************************/
/* Function to verify that packets sent and received are intended values */
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
/*********************************************************************************/
/* Functions: Extra CAN Functions                                                */
/* Purpose: All functions below need to be declared for CAN to function, but are */
/* only used when using interrupt vectors for CAN. Still need to verify that     */
/* packets sent and received over CAN are intended values                        */
/*********************************************************************************/

/* All functions below need to be declared for CAN to function, but are only     */
/* used when intterupt vectors are broadcasted over the CAN network              */
void canMessageNotification(canBASE_t *node, uint32_t messageBox)
{
    /* Still need to implement */
    return;
}
void canErrorNotification(canBASE_t *node, uint32_t messageBox)
{
    /* Still need to implement */
    return;
}
void esmGroup1Notification(unsigned channel)
{
    /* Still need to implement */
    return;
}
void esmGroup2Notification(unsigned channel)
{
    /* Still need to implement */
    return;
}

/* End of HL_sys_main.c */
/* USER CODE END */
