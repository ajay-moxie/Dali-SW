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
* File Name : r_systeminit.c
* Device(s) : 
* Description : This file implements macro initialization.
******************************************************************************/

#pragma	SFR
#pragma	DI
#pragma	EI

/******************************************************************************
Includes <System Includes> , �gProject Includes�h
******************************************************************************/
#include "r_macrodriver.h"
#include "r_system.h"
#include "r_lvd.h"
#include "r_wdt.h"

/******************************************************************************
* Function Name : SystemInit
* Description : 
* Argument : none
* Return Value : none
******************************************************************************/
void SystemInit( void )
{
	/* clock generator initialize */
	Clock_Init();
	/* low-voltage detector initialize */
	Lvi_Init();
}

/******************************************************************************
* Function Name : hdwinit
* Description : 
* Argument : none
* Return Value : none
******************************************************************************/
void hdwinit( void )
{
	DI( );
	SystemInit( );
	EI( );
}
