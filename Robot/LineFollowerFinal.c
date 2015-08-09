/*=========================================================================\
|
|   Copyright (C) 2013 by Billy Park & Ethan Grant.
|
|   File:        main.c
|   Project:     MARCO Line Follower
|   Created:     11/04/2013
|   Description: This program is designed for the MARCO robot to make it
|                follow the edge of a line that has been set out on the
|                floor and if it detects an obstruction in its path the 
|                MARCO will stop until the obstruction is removed. 
|
\=========================================================================*/

#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "alt_types.h"
/* Standard Altera include files to enable the mapping of names
To hardware addresses etc. */
int main (void) __attribute__ ((weak, alias ("alt_main")));

/*Hex reference table*/
#define LEFT_MOTOR 0x1
#define RIGHT_MOTOR 0X2
#define LEFT_MOTORDIR 0x4
#define RIGHT_MOTORDIR 0x8
#define RIGHT_ENCODER 0x100
#define LEFT_ENCODER 0x200
#define RIGHT_REARBUMPER 0x400
#define RIGHT_FRONTBUMPER 0x800
#define LEFT_REARBUMPER 0x1000
#define RIGHT_LINEFOLLOWER 0x2000
#define LEFT_LINEFOLLOWER 0x4000
#define LEFT_FRONTBUMPER 0x8000
#define RIGHT_EYESWITCH 0x10000
#define LEFT_EYESWITCH 0x20000
#define EYEMOTOR_SM01 0x1000000
#define EYEMOTOR_SM02 0x2000000
#define EYEMOTOR_SM03 0x4000000
#define EYEMOTOR_SM04 0X8000000

void pwm(alt_u32);
void fpwm(alt_u32);
void bumpercheck(alt_u32, alt_u32);

alt_main()
{
    alt_u32 sensors;
    alt_u32 bumpers = 0;
    alt_u32 motor;
    int i = 0;

    IOWR_ALTERA_AVALON_PIO_DIRECTION(EXPANSION_JP1_BASE,0xF000000F);

    while(1)
    {
            bumpers = IORD_ALTERA_AVALON_PIO_DATA(EXPANSION_JP1_BASE);
            bumpers &= (LEFT_FRONTBUMPER | RIGHT_FRONTBUMPER);
            bumpers >>= 11;
            
            sensors = IORD_ALTERA_AVALON_PIO_DATA(EXPANSION_JP1_BASE);
            sensors = sensors & (LEFT_LINEFOLLOWER | RIGHT_LINEFOLLOWER);
            sensors >>= 13;
                      
            bumpercheck(bumpers, motor);

            switch(sensors)
            {
                case 0x0      :for(i = 0; i < 40; i++)  /*Turn right*/
                               {
                                    motor = 0x7;        /*Sharp turn right*/
                                    pwm(motor);       
                                    motor = 0xF;        /*Move forward*/  
                                    fpwm(motor);                  
                               }
                               break;
                               
                case 0x1      :for(i = 0; i < 40; i++)  /*Just need to keep moving forward*/
                               {
                                    motor = 0xF;        /*Move forward*/
                                    pwm(motor);                             
                               }
                               break; 
                                 
                case 0x2      :for(i = 0; i < 40; i++)  /*Turn right*/
                               {
                                    motor = 0x7;        /*Sharp turn right*/
                                    pwm(motor);
                                    motor = 0xF;        /*Move forward*/
                                    fpwm(motor);
                               }
                               break;
                               
                case 0x3      :for(i = 0; i < 40; i++)  /*Turn left*/
                               {
                                    motor = 0xB;        /*Sharp turn left*/
                                    pwm(motor);    
                                    motor = 0xF;        /*Move forward*/   
                                    fpwm(motor);                         
                               }
                               break; 
                                                                 
                default       :break;
            }
    }      
}

/*=========================================================================\
|
|   Function Name:  pwm
|   Description:    This function takes the motor value, and rapidly turns
|                   the motor on and off to slow down the MARCO and make its
|                   line following more precise.
|
\=========================================================================*/

void pwm(alt_u32 motor)
{
    int x = 0;
    int y = 0;
    
    for(x = 0; x < 70; x++)
    {
        IOWR_ALTERA_AVALON_PIO_DATA(EXPANSION_JP1_BASE, motor);
    }
    
    for(y = 0; y < 30; y++)
    {
        motor = 0x0;
        IOWR_ALTERA_AVALON_PIO_DATA(EXPANSION_JP1_BASE, motor);
    }    
}

/*=========================================================================\
|
|   Function Name:  fpwm
|   Description:    This function takes the forward motor value after every
|                   turn, and rapidly turns the motor on and off to slow
|                   down the MARCO to stop it leaving the line.
|
\=========================================================================*/

void fpwm(alt_u32 motor)
{
    int x = 0;
    int y = 0;
    
    for(x = 0; x < 60; x++)
    {
        IOWR_ALTERA_AVALON_PIO_DATA(EXPANSION_JP1_BASE, motor);
    }
    
    for(y = 0; y < 10; y++)
    {
        motor = 0x0;
        IOWR_ALTERA_AVALON_PIO_DATA(EXPANSION_JP1_BASE, motor);
    }    
}

/*=========================================================================\
|
|   Function Name:  bumpercheck
|   Description:    Bumpercheck is ran every time the main while loop
|                   iterates itself, which checks if the front bumpers
|                   are obstructed, if they are it stops and starts again
|                   when they arent obstructed.
|
\=========================================================================*/

void bumpercheck(alt_u32 bumpers, alt_u32 motor)
{
    while (bumpers != 17)
            {
                motor = 0x0;
                IOWR_ALTERA_AVALON_PIO_DATA(EXPANSION_JP1_BASE, motor);
                bumpers = IORD_ALTERA_AVALON_PIO_DATA(EXPANSION_JP1_BASE);
                bumpers &= (LEFT_FRONTBUMPER | RIGHT_FRONTBUMPER);
                bumpers >>= 11;
            }
    return;
}

