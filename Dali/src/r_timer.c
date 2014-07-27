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
* File Name : r_timer.c
* Device(s) : 
* Description : This file implements the Timer is controlled.
******************************************************************************/

#pragma	SFR
#pragma	NOP

/******************************************************************************
Includes <System Includes> , ÅgProject IncludesÅh
******************************************************************************/
#ifndef __IAR_SYSTEMS_ICC__
#pragma interrupt	INTTM00	Timer_INTTM000
#endif

#include "r_macrodriver.h"
#include "r_timer.h"

#ifdef __IAR_SYSTEMS_ICC__
#pragma vector = INTTM00_vect
__interrupt void Timer_INTTM000( void );
#endif

/******************************************************************************
Private global variables and functions
******************************************************************************/
static void (*interrupt_proc)() = 0;

/******************************************************************************
* Function Name : Timer_StartInterval_1ms
* Description : 
* Argument : none
* Return Value : none
******************************************************************************/
void Timer_Init( )
{
	TPS0	= 0b0000000000000000;
	TMR00	= 0b0000000000000000;
	TDR00	= 31999;	/* The initial value of the period is set to 1ms @ 32MHz */
	TMMK00	= 1;		/* Mask interrupt */
	TMMK00		   = 0;
}

/******************************************************************************
* Function Name : Timer_StartInterval_1ms
* Description : 
* Argument : none
* Return Value : none
******************************************************************************/
void Timer_StartInterval_1ms( void (*proc)(void) )
{
	interrupt_proc = proc;
	TS0		= 0x01;				/* Start TAU Channel 0 */
	TMMK00		   = 0;
}

/******************************************************************************
* Function Name : Timer_Interrupt
* Description : 
* Argument : none
* Return Value : none
******************************************************************************/
__interrupt void Timer_INTTM000( void )
{
	if (interrupt_proc!=0){
		interrupt_proc();
	}
}
