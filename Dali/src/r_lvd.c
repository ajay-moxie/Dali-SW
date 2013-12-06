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
* File Name : r_lvi.c
* Device(s) : 
* Description : This file implements the functions of Low-Voltage Detector.
******************************************************************************/

#pragma	SFR
#pragma	NOP

/******************************************************************************
Includes <System Includes> , gProject Includesh
******************************************************************************/
#include "r_macrodriver.h"
#include "r_lvd.h"

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
void wait10us( void );

/******************************************************************************
* Function Name : Lvi_Init
* Description : Initilaize for LVI.
* Argument : none
* Return Value : none
******************************************************************************/
void Lvi_Init( void )
{
	if (!(RESF & 0b00000001))	/* If the reset source is not LVI, set LVI */
	{
		LVIMK = 1;				/* Mask the LVI interrupt */
		LVISEN = 1;				/* LVIS register write enable */
		LVIS  = 0b10000001;		/* LVI Reset & Low Voltage detect */
		LVISEN = 0;				/* LVIS register write disable */
		while (LVIF){};			/* Wait until VDD >= VLVI */
	}
}

/******************************************************************************
* Function Name : wait10us
* Description : 
* Argument : none
* Return Value : none
******************************************************************************/
void wait10us( void )
{							/* CALL :  7 clocks */
	NOP();					/* NOP  :  2 clocks */
	NOP();					/* NOP  :  2 clocks */
	NOP();					/* NOP  :  2 clocks */
	NOP();					/* NOP  :  2 clocks */
							/* RET  :  6 clocks */
							/*        21 clocks in all			*/
							/* count for 0.5us x 21 = 10.5us	*/
}
