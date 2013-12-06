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
* File Name : system.c
* Device(s) : 
* Description : This file implements device driver for System module.
******************************************************************************/

#pragma	SFR
#pragma	NOP

/******************************************************************************
Includes <System Includes> , ÅgProject IncludesÅh
******************************************************************************/
#include "r_macrodriver.h"
#include "r_system.h"
#include "r_user.h"

/******************************************************************************
* Function Name : Clock_Init
* Description : 
* Argument : data
* Return Value : none
******************************************************************************/
void Clock_Init( void )
{
	int ucCount;

	CMC			= 0b00000000;					/* Clock Operation Mode */
	CSC			= 0b11000000;					/* Clock Operation Status Control */
	OSMC		= 0b10000000;					/* Operation Speed Mode Control */
	PLLCTL		= 0b00000000;					/* Clock divided by 2 (32 MHz) */
	PLLCTL		= 0b00000001;					/* Operating PLL function */
	for( ucCount = 0; ucCount < 255 ; ucCount++ ) {	/* Wait(must be at least 40us) */
		NOP();
	}
	PLLCTL		= 0b00000101;		/* Selects PLL output clock (32 MHz is supplied to fCLK) */

	do{
		NOP();
	}while( PLLS != 1 );

	PER0 		= 0b00100101;		/* Peripheral Enable Register 0 (PER0) */
	PER1 		= 0b00000001;		/* Peripheral Enable Register 1 (PER1) */
	PER2 		= 0b10000111;		/* Peripheral Enable Register 2 (PER2) */
}
