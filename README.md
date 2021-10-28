# CCMI_Test
 Embedded code to test the TI Hercules LAUNCHXL2-570LC43 / RM57Lx on the Central Control Module Interface (CCMI) PCB designed by UCI's Anteater Electric Racing (AER)'s Electronics Subteam

## Folder and File Contents
source: The location for the source code that executes functions.\
include: The location for the header files containing the declarations and definitions of the API.\
main program: HL_sys_main.c

## CCMI Overview
An interface PCB that provides a platform for the TI Hercules LAUNCHXL2-570LC43 / RM57Lx microcontroller unit (MCU). The CCMI is designed to be the central computing unit, as it receives input signals and outputs control, safety, and status signals.\
The CCMI controls various functions of the FSAE race car including motor speed, brake lights, accumulator fans, and the speaker that plays the ready to drive signal (RTDS). The CCMI receives human input from the steering, brake, and throttle components and receives sensor input from the Suspension Sensor Array (SSA) via the CAN bus. Also, the CCMI receives safety signals from the Battery Management System (BMS) and Insulation Monitoring Device (IMD) and then sends corresponding status signals to the dashboard (DASH) PCB and the Logic Power Stage (LPS) PCB. A Brake System Plausibility Device (BSPD) safety signal is also generated by the CCMI when the throttle and brake are both pressed hard for 0.5s; this status signal is also sent to the LPS board. Finally, the CCMI board includes indicator LEDs to display status signals for power, APPS, BSE, and INTERRUPT signals.


## Team Info
Code written by: Marcos Rincon\
CCMI Designed by: Electronics Subteam of Anteater Electric Racing (AER) at the University of California, Irvine

Advised by: Dr. J. Michael McCarthy

## Contributions/Dependencies
IDEs used to generate and edit hardware abstraction layer code:\
HALCOGEN\
Code Composer Studio

Thanks to the contributions from these GitHub libraries:


