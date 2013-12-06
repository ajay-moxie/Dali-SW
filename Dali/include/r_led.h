/*******************************************************************************
* File Name : r_led.h
* Description : This file implements LED control.
******************************************************************************/

#ifndef	_MDLED_
#define _MDLED_

/******************************************************************************
Macro definitions
******************************************************************************/
#define DIMMING_CURVE(level)	(dimming_table[level])

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern const uint8_t	dimming_table[256];

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
void LED_init( void );

void LED1_init( void );
void LED2_init( void );
void LED3_init( void );

void LED1_set( uint8_t level );
void LED2_set( uint8_t level );
void LED3_set( uint8_t level );

uint8_t LED1_get();
uint8_t LED2_get();
uint8_t LED3_get();

#endif
