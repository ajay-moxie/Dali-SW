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
* File Name : r_led.c
* Device(s) : 
* Description : This file implements to control the LED 
******************************************************************************/

#pragma	SFR
#pragma	EI
#pragma	DI
#pragma	NOP

/******************************************************************************
Includes <System Includes> , ÅgProject IncludesÅh
******************************************************************************/
#ifndef __IAR_SYSTEMS_ICC__
#pragma	interrupt	INTTM01		LED_FeedbackOperation	/* FeedbackProcess/100us interrupt */
#endif

#include "r_macrodriver.h"
#include "r_user.h"
#include "r_led.h"

#ifdef __IAR_SYSTEMS_ICC__
#pragma	interrupt	INTTM01		LED_FeedbackOperation	/* FeedbackProcess/100us interrupt */
#endif

/******************************************************************************
Macro definitions
******************************************************************************/
#define ON 		1
#define OFF 		0

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
sreg signed short	temp11,temp12,temp21,temp22,temp31,temp32;
/* Duty cycle for LEDx (used for PI feedback method) */
sreg unsigned long	Duty_LED1,Duty_LED2,Duty_LED3;

/* Duty cycle error for LED1 (used for PI feedback method) */
sreg signed long	ErrLED1,ErrLED2,ErrLED3;

/* Parameter A1/A2 used for PI feedback method */
// const signed long	A1 = 185,A2 = 32;
#define A1	(long)185
#define A2	(long)32

unsigned char
	feedback_offset,		/* Feedback counter (used to calculate feedback 
							offset values on the first ever feedback) */
	feedback;			/* Feedback counter used to switch between
							ADC feedback channels */

unsigned char 	feedback1_off = 0,	/* Feedback processing enable/disable flags */
		feedback2_off = 0,
		feedback3_off = 0;

sreg unsigned short
	getvalue,		/* Save the latest A/D result */
	VR1,			/* Last A/D result for LED1 volume  */
	VR2,			/* Last A/D result for LED2 volume */
	VR3,			/* Last A/D result for LED3 volume */
	FB_LEDAD1,		/* Last A/D result for LED1 feedback */
	FB_LEDAD1_old,	/* Previous A/D result for LED1 feedback */
	FB_LEDAD2,		/* Last A/D result for LED2 feedback */
	FB_LEDAD2_old,	/* Previous A/D result for LED2 feedback */
	FB_LEDAD3,		/* Last A/D result for LED3 feedback */
	FB_LEDAD3_old,	/* Previous A/D result for LED3 feedback */
	offsetLED1,		/* Save feedback offset value for LED1 when off */
	offsetLED2,		/* Save feedback offset value for LED2 when off */
	offsetLED3;		/* Save feedback offset value for LED3 when off */

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/


/*
**-----------------------------------------------------------------------------
**
**  function name:
**  	LED_init
**
**  Parameters:
**  	None
**
**  Returns:
**  	None
**
**-----------------------------------------------------------------------------
*/ void LED_init(void) 
{
	#define FREQUENCY 		255

	ADPC	= 0b00000000;	/* A/D Port Configuration Register */
	PMC0	= 0b11110011;	/* RXD1/P03, TXD1/P02 used as digital I/O */
	PMC12	= 0b11111111;	/* ANI19/P120 used as analog input */
	PMC14	= 0b01111111;	/* ANI18/P147 used as digital I/O */
	P0		= 0b00000000;
	PM0		= 0b10100000;	/* Modify P05, P06 to change the output setting */
	P1		= 0b00000000;
	PM1		= 0b00000000;	/* Modify P00, P01, P02 to change the output setting */
	P2		= 0b00000000;
	PM2		= 0b11111111;	/* ANI3 is always set as analog input */
	P3		= 0b00000000;
	PM3		= 0b11111100;	/* P30, 31 set as outputs */
	P4		= 0b00000000;
	PM4		= 0b11110001;	/* P40 set as input */
	P7		= 0b00000000;
	PM7		= 0b00000000;	/* P77, 76, 75 set as outputs */
	P12		= 0b00000000;
	PM12	= 0b11111111;	/* P120 set as analog input */
	P14		= 0b00000000;
	PM14	= 0b00111100;	/* P147 set as output */
	P20		= 0b00000000;
	PM20	= 0b10000000;	/* P200-206 set as outputs */

	TPS0	= 0b0000000000000000;
	TMR00	= 0b0000000000000000;
	TMR01	= 0b0000000000000000;
	TDR00	= 31999;	/* The initial value of the period is set to 1ms @ 32MHz */
	TMMK00	= 1;		/* Mask interrupt */
	TDR01	= 3199;		/* The initial value of the period is set to 100us @ 32MHz */
	TMMK01	= 1;		/* Mask interrupt */

	ADM0	= 0b00101000;
	ADM1	= 0b00000000;
	ADM2	= 0b01100000;
	ADUL	= 0xFF;		/* Conversion result comparison upper limit setting register */
	ADLL	= 0x00;		/* Conversion result comparison lower limit setting register */
	ADS		= 0b00000000;
	ADCE	= 1;		/* Enables A/D voltage comparator operation */
	PGACTL	= 0b00000001;
	PGAINS	= 0b00000000;

	TPS2	= 0b00000000;/* CK20, CK21, both set to fCLK */

	TKBCTL00= 0x1100;	/* TKBO00, TKB01 PWM with dithering function ON */
	TKBCTL10= 0x0100;	/* TKBO10 PWM with dithering function ON */
	TKBCTL20= 0x0000;	/* TKB2 output gate function, PWM soft start,
							 PWM dither, maximum frequency ALL OFF */

	TKBIOC00= 0b00000101;/* TKB0 active high, Low level (normal output) */
	TKBIOC01= 0b00000011;/* TKB0 timer output enabled */
	TKBIOC10= 0b00000000;/* TKB1 active high, Low level (normal output) */
	TKBIOC11= 0b00000001;/* TKB1 timer output enabled */

	TKBCR01	= 000;		/* TKBO00 output LOW */
	TKBCR02	= 0;		/* TKBO01 output HIGH */
	TKBCR03	= 0;		/* TKBO01 output LOW */
	TKBCR00	= FREQUENCY;/* TKBO00, 01 cycle period */
	TKBCR11	= 0;		/* TKBO10 output LOW */
	TKBCR10	= FREQUENCY;/* TKBO10, 11 cycle period */

	TKBCE0	= 1;		/* Starts TKB0 */
	TKBCE1	= 1;		/* Starts TKB1 */

	TMIF01	= 0;		/* Clear TM01 interrupt flag */
	TMMK01	= 0;		/* Enables interrupt INTMM01 */

	EI();				/* Enable interrupt */

	TS0		= 0x02;		/* Start TAU Channel 1 */
	NOP();
	NOP();

	feedback_offset = 0;/* Initialize feedback counter for feedback 
							offset calculations */
	feedback		= 0;/* Initialize feedback counter */
}

/*
**-----------------------------------------------------------------------------
**
**  function name:
**  	fbp_interrupt
**
**  Parameters:
**  	None
**
**  Returns:
**  	void
**
**
**	Note	: when 100us interrupt occurs
**	INTTM01 occurs every 100us for feedback process
**	Change the PWM value to the new target value at each feecback processing 
**	cycle using dithering function 
**	CR0 = 255, OPamp x 8, Limit = 850 (400mA)
**
**	Feedback Operation using PI Control Method
**	"D(n) = D(n-1) + A1 x E(n) + A2 x E(n-1)"
**		D(n): Latest PWM output duty
**		D(n-1): Previous PWM output duty
**		E(n): Latest error value =
**			(A/D conversion target value) - (Latest A/D conversion measurement value)
**		E(n-1): Previous error value = 
**			(A/D conversion target value) - (Previous A/D conversion measurement value)
**		A1, A2: Coefficients
**
**-----------------------------------------------------------------------------
*/
__interrupt void LED_FeedbackOperation(void)
{
	#define MAXCHECK 		850
	#define FEEDBACK_LED1 	0
	#define FEEDBACK_LED2 	1
	#define FEEDBACK_LED3 	2
	#define PGA_OUTPUT 		3

	unsigned short ucCount;
	unsigned int adcvalue[3];

	feedback += 1;		// Increment feedback counter
	switch (feedback)
	{
		/* LED1 Feedback Processing */
		case 1:
			if (feedback1_off == 0) 
			{
				PGAINS = FEEDBACK_LED1;			// Select LED1 feedback channel
				PGAEN = ON;				// Start PGA
	
				for( ucCount=0; ucCount<20; ucCount++ )
				{
					NOP();
 				}
	
				ADS = PGA_OUTPUT;			// Select PGA output as ADC input channel
				ADIF = 0;				// Clear ADC interrupt flag
				ADCS = ON;				// start ADC

				while (!ADIF){}
				adcvalue[0] = ADCR;			// Read A/D result
				//ADCS = OFF;				// Stop ADC
				ADIF = 0;				// Clear ADC interrupt flag

				while (!ADIF){}
				adcvalue[1] = ADCR;			// Read A/D result
				//ADCS = OFF;				// Stop ADC
				ADIF = 0;				// Clear ADC interrupt flag

				while (!ADIF){}
				adcvalue[2] = ADCR;			// Read A/D result
				ADCS = OFF;				// Stop ADC
				ADIF = 0;				// Clear ADC interrupt flag

				getvalue = (adcvalue[0] + adcvalue[1] + adcvalue[2])/3;
				temp11 = VR1 - (FB_LEDAD1_old - offsetLED1);
				ErrLED1 = A2 * temp11;			// Calculate "A2 x E(n-1)" of PI control
				FB_LEDAD1 = (getvalue >> 6) & 0x03FE;
				if (feedback_offset == 0)
				{
					offsetLED1 = FB_LEDAD1;		// Calculate offset value for LED1 feedback when LED is off (only 1 time at start-up)
				}
				/* Check if LED1 feedback current is over max */
				if((FB_LEDAD1 - offsetLED1) >= MAXCHECK)
				{
					TKBCRLD00 = 0;			// Set PWM Duty to 0 when current is over MAX 400mA
				}
				else
				{
					temp12 = VR1 - (FB_LEDAD1 - offsetLED1);
					ErrLED1 += A1 * temp12;		// Calculate "A1 x E(n) + A2 x E(n-1)" of PI control
					if (ErrLED1 > 0)
					{
						if (0xFFF000 - Duty_LED1 >= ErrLED1)
						{
							Duty_LED1 += ErrLED1;		// Increase duty for LED1
						}
						else
						{
							Duty_LED1 = 0xFFF000;		// Set duty for LED1 to maximum
						}
					}
					else if (ErrLED1 < 0)
					{
						if (Duty_LED1 > - ErrLED1)
						{
							Duty_LED1 += ErrLED1;		// Decrease duty for LED1
						}
						else
						{
							Duty_LED1 = 0;			// Set duty for LED1 to 0
						}
					}
					else if ((ErrLED1 == 0) && (VR1 == 0))
					{
						Duty_LED1 = 0;			// Set duty for LED1 to 0
					}
					TKBCRLD00 = (unsigned short)(Duty_LED1 >> 8);	// Upload duty in the dithering count register
				}
				FB_LEDAD1_old = FB_LEDAD1;		// Save the LED1 feedback A/D result
				TKBTRG0 = 1;
				NOP();

				PGAEN = 0;				// Stop PGA
			}
			break;

		/* LED2 Feedback Processing */
		case 2:
			if (feedback2_off == 0) 
			{
				PGAINS = FEEDBACK_LED2;			// Select LED2 feedback channel
				PGAEN = ON;				// Start PGA
				for( ucCount=0; ucCount<20; ucCount++ )
				{
					NOP();
 				}
				ADS = PGA_OUTPUT;			// Select PGA output as ADC input channel
				ADIF = 0;				// Clear ADC interrupt flag
				ADCS = ON;				// start ADC

				while (!ADIF){}
				getvalue = ADCR;			// Read A/D result
				ADCS = OFF;				// Stop ADC
				ADIF = 0;				// Clear ADC interrupt flag
	
				temp21 = VR2 - (FB_LEDAD2_old - offsetLED2);
				ErrLED2 = A2 * temp21;			// Calculate "A2 x E(n-1)" of PI control
				FB_LEDAD2 = (getvalue >> 6) & 0x03FE;
				if (feedback_offset == 0)
				{
					offsetLED2 = FB_LEDAD2;		// Calculate offset value for LED2 feedback when LED is off (only 1 time at start-up)
				}
				/* Check if LED2 feedback current is over max */
				if((FB_LEDAD2 - offsetLED2) >= MAXCHECK)
				{
					TKBCRLD01 = 0;			// Set PWM Duty to 0 when current is over MAX 400mA
				}
				else
				{
					temp22 = VR2 - (FB_LEDAD2 - offsetLED2);
					ErrLED2 += A1 * temp22;		// Calculate "A1 x E(n) + A2 x E(n-1)" of PI control
					if (ErrLED2 > 0)
					{
						if (0xFFF000 - Duty_LED2 >= ErrLED2)
						{
							Duty_LED2 += ErrLED2;		// Increase duty for LED2
						}
						else
						{
							Duty_LED2 = 0xFFF000;		// Set duty for LED2 to maximum
						}
					}
					else if (ErrLED2 < 0)
					{
						if (Duty_LED2 > - ErrLED2)
						{
							Duty_LED2 += ErrLED2;		// Decrease duty for LED2
						}
						else
						{
							Duty_LED2 = 0;			// Set duty for LED2 to 0
						}
					}
					else if ((ErrLED2 == 0) && (VR2 == 0))
					{
						Duty_LED2 = 0;			// Set duty for LED2 to 0
					}
					TKBCRLD01 = (unsigned short)(Duty_LED2 >> 8);	// Upload duty in the dithering count register
				}		
				FB_LEDAD2_old = FB_LEDAD2;		// Save the LED2 feedback A/D result
				TKBTRG0 = 1;
				NOP();
	
				PGAEN = 0;				// Stop PGA
			}
			break;

		/* LED3 Feedback Processing */
		case 3:
			if (feedback3_off == 0) 
			{
				PGAINS = FEEDBACK_LED3;			// Select LED3 feedback channel
				PGAEN = ON;				// Start PGA
				for( ucCount=0; ucCount<20; ucCount++ )
				{
					NOP();
 				}
				ADS = PGA_OUTPUT;			// Select PGA output as ADC input channel
				ADIF = 0;				// Clear ADC interrupt flag
				ADCS = ON;				// start ADC
	
				while (!ADIF){}
				getvalue = ADCR;			// Read A/D result
				ADCS = OFF;				// Stop ADC
				ADIF = 0;				// Clear ADC interrupt flag
						
				temp31 = VR3 - (FB_LEDAD3_old - offsetLED3);
				ErrLED3 = A2 * temp31;			// Calculate "A2 x E(n-1)" of PI control
				FB_LEDAD3 = (getvalue >> 6) & 0x03FE;
				if (feedback_offset == 0)
				{
					offsetLED3 = FB_LEDAD3;		// Calculate offset value for LED3 feedback when LED is off (only 1 time at start-up)
				}
				/* Check if LED3 feedback current is over max */
				if((FB_LEDAD3 - offsetLED3) >= MAXCHECK)
				{
					TKBCRLD10 = 0;		// Set PWM Duty to 0 when current is over MAX 400mA
				}
				else
				{
					temp32 = VR3 - (FB_LEDAD3 - offsetLED3);
					ErrLED3 += A1 * temp32;			// Calculate "A1 x E(n) + A2 x E(n-1)" of PI control
					if (ErrLED3 > 0)
					{
						if (0xFFF000 - Duty_LED3 >= ErrLED3)
						{
							Duty_LED3 += ErrLED3;		// Increase duty for LED3
						}
						else	
						{
							Duty_LED3 = 0xFFF000;		// Set duty for LED3 to maximum
						}
					}
					else if (ErrLED3 < 0)
					{
						if (Duty_LED3 > - ErrLED3)
						{
							Duty_LED3 += ErrLED3;		// Decrease duty for LED3
						}
						else
						{
							Duty_LED3 = 0;			// Set duty for LED3 to 0
						}
					}
					else if ((ErrLED3 == 0) && (VR3 == 0))
					{
						Duty_LED3 = 0;			// Set duty for LED3 to 0
					}
					TKBCRLD10 = (unsigned short)(Duty_LED3 >> 8);	// Upload duty in the dithering count register
				}
				FB_LEDAD3_old = FB_LEDAD3;	// Save the LED3 feedback A/D result
				TKBTRG1 = 1;
				NOP();
	
				PGAEN = 0;			// Stop PGA
			}
			feedback = 0;			// Clear feedback counter
			if (feedback_offset == 0xFF)
			{
				feedback_offset = 1;
			}
			else
			{
				feedback_offset++;
			}
	}
	TMIF01 = 0;					// Clear TM00 interrupt flag
}
