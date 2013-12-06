/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

/*******************************************************************************
* File Name : r_led1.c
* Device(s) : 
* Description : This file implements to control the LED(channel-1). 
******************************************************************************/

#pragma	SFR
#pragma	DI
#pragma	EI

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
#include "r_macrodriver.h"
#include "r_led.h"

/******************************************************************************
Private global variables and functions
******************************************************************************/
static uint8_t	led_current_level = 0;
sreg uint16_t	VR1_old;		/* Previous A/D result for LED1 volume */


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
extern sreg uint16_t
	getvalue,		// Save the latest A/D result
	VR1,			// Last A/D result for LED1 volume 
	VR2,			// Last A/D result for LED2 volume
	VR3,			// Last A/D result for LED3 volume
	FB_LEDAD1,		// Last A/D result for LED1 feedback
	FB_LEDAD1_old,		// Previous A/D result for LED1 feedback
	FB_LEDAD2,		// Last A/D result for LED2 feedback
	FB_LEDAD2_old,		// Previous A/D result for LED2 feedback
	FB_LEDAD3,		// Last A/D result for LED3 feedback
	FB_LEDAD3_old,		// Previous A/D result for LED3 feedback
	offsetLED1,		// Save feedback offset value for LED1 when off
	offsetLED2,		// Save feedback offset value for LED2 when off
	offsetLED3;		// Save feedback offset value for LED3 when off

extern sreg unsigned long Duty_LED1;

extern sreg signed long	ErrLED1;

extern unsigned char feedback1_off;	/* LED1 Feedback processing enable/disable flag */

/******************************************************************************
* Function Name : LED1_init
* Description : Initialize.
* Argument : none
* Return Value : none
******************************************************************************/
void LED1_init(void) 
{
	/* No Operation */
}

/******************************************************************************
* Function Name : LED1_set
* Description : Set lighting level.
* Argument : level
* Return Value : none
******************************************************************************/
void LED1_set(uint8_t level) 
{
	if (level == led_current_level) {
		/* lighting level is not change, then skip */
		return;
	}

	led_current_level = level;

	DI();
	VR1 = ((uint16_t)level << 8) & 0xFF00;	/* 8bit -> 16bit(upper 10bit be used for A/D) */
	/* A/D value of VR1 is 0 */
	if(VR1 <= 0)
	{
		TKBCRLD00 = 0;			/* Set PWM Output to 0 */
		TKBTRG0 = 1;
		feedback1_off = 1;		/* Disable LED1 current feedback processing */
		Duty_LED1 = 0;			/* Reinitialize PI variables */
		ErrLED1 = 0;
	}
	else
	{
		/*-------------------------------------------------------
		Calculate dimming level according to the 10bit A/D result
		-------------------------------------------------------*/
		/* if A/D result of VR1 is considered to have changed */
		if((VR1 > (VR1_old + 1))
		|| (VR1 < (VR1_old - 1)))
		{
			VR1 = (VR1 >> 3) / 11;	/* Convert VR1 target value to be in the feedback range */
			VR1_old = VR1;		/* Save VR1 A/D result */
			feedback1_off = 0;	/* Enable LED1 current feedback processing */
		}
	}
	EI();
}

/******************************************************************************
* Function Name : LED1_get
* Description : Get current level.
* Argument : none
* Return Value : level
******************************************************************************/
uint8_t LED1_get()
{
	return led_current_level;
}
