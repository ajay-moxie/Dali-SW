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
* File Name : DALI_timer.c
* Device(s) : 
* Description : This file implement DALI protocol. 
******************************************************************************/
#pragma	SFR
#pragma DI
#pragma EI

/******************************************************************************
Includes <System Includes> , ÅgProject IncludesÅh
******************************************************************************/
#include "r_macrodriver.h"
#include "r_user.h"
#include "r_timer.h"
#include "r_dali.h"
#include "r_dali_analyze.h"
#include "r_dali_timer.h"


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern uint8_t dali_actualSaveChannel;

/******************************************************************************
Private global variables and functions
******************************************************************************/
static int32_t 	fade_steps[NUMBER_OF_CHANNEL];
//static int32_t	fade_time[NUMBER_OF_CHANNEL];
static int32_t	fade_steps_per_10ms[NUMBER_OF_CHANNEL];
static uint16_t fade_count[NUMBER_OF_CHANNEL];

static uint32_t timecount_addressing[NUMBER_OF_CHANNEL];
static uint8_t timecount_rcv_command[NUMBER_OF_CHANNEL];
static uint8_t timecount_answer;
static uint8_t timecount_dapc_sequence[NUMBER_OF_CHANNEL];
static uint8_t timecount_actual_unchange[NUMBER_OF_CHANNEL];
static uint8_t timecount_interface_failure;
static uint8_t timecount_prohibit_reception;
static uint8_t timecount_10ms;
static uint32_t timecount_totaltime;
static struct timer dali_timeout;
static void DALI_NameDeviceCounter( void );

/******************************************************************************
* Function Name : DALI_InitTimer
* Description : Initialize the interval timer.
* Argument : none
* Return Value : none
******************************************************************************/
void DALI_NameDeviceInitTimer( void )
{
	uint8_t i;

	/* initialize variable. */
	timecount_totaltime = 0;
	timecount_answer = 0;
	timecount_interface_failure = TIME_INTERFACE_FAILURE;
	timecount_10ms = 10;

	for (i=0; i<NUMBER_OF_CHANNEL; i++){
		timecount_addressing[i] = 0;
		timecount_rcv_command[i] = 0;
		timecount_actual_unchange[i] = 0;
		timecount_dapc_sequence[i] = 0;
		fade_steps[i] = 0;
		fade_count[i] = -1;
	}

	CLR_ANSWER;

	dali_timeout.timecount_4ms = 0;
	dali_timeout.timecount_8ms = 0;
	dali_timeout.timecount_10ms = 0;
	dali_timeout.timecount_50ms = 0;
	dali_timeout.timecount_100ms = 0;
	dali_timeout.timecount_15min = 0;
	dali_timeout.timecount_totaltime = 0;

	dali_timeout.func_timecount_4ms = (fn_t)0;
	dali_timeout.func_timecount_8ms = (fn_t)0;
	dali_timeout.func_timecount_10ms = (fn_t)0;
	dali_timeout.func_timecount_50ms = (fn_t)0;
	dali_timeout.func_timecount_100ms = (fn_t)0;
	dali_timeout.func_timecount_15min = (fn_t)0;
	Timer_StartInterval_1ms(DALI_NameDeviceCounter);
}


/******************************************************************************
* Function Name : DALI_ConfigCounter
* Description : Interval timer process.
* Argument : none
* Return Value : none
******************************************************************************/
static void DALI_NameDeviceCounter( void )
{
	dali_timeout.timecount_totaltime++;
	if(dali_timeout.timecount_4ms){
		dali_timeout.timecount_4ms--;
		if((dali_timeout.func_timecount_4ms) && (dali_timeout.timecount_4ms == 0))
			dali_timeout.func_timecount_4ms();
	}
	if(dali_timeout.timecount_8ms){
		dali_timeout.timecount_8ms--;
		if((dali_timeout.func_timecount_8ms) && (dali_timeout.timecount_8ms == 0))
			dali_timeout.func_timecount_8ms();
	}
	if(dali_timeout.timecount_10ms){
		dali_timeout.timecount_10ms--;
		if((dali_timeout.func_timecount_10ms) && (dali_timeout.timecount_10ms == 0))
			dali_timeout.func_timecount_10ms();
	}
	if(dali_timeout.timecount_50ms){
		dali_timeout.timecount_50ms--;
		if((dali_timeout.func_timecount_50ms) && (dali_timeout.timecount_50ms == 0))
			dali_timeout.func_timecount_50ms();
	}
	if(dali_timeout.timecount_100ms){
		dali_timeout.timecount_100ms--;
		if((dali_timeout.func_timecount_100ms) && (dali_timeout.timecount_100ms == 0))
			dali_timeout.func_timecount_100ms();
	}
	if(dali_timeout.timecount_15min){
		dali_timeout.timecount_15min--;
		if((dali_timeout.func_timecount_15min) && (dali_timeout.timecount_15min == 0))
			dali_timeout.func_timecount_15min();
	}

}

/******************************************************************************
* Function Name : DALI_StartTimer
* Description : Start timer count.
* Argument : type
* Return Value : none
******************************************************************************/
void DALI_NameDeviceStartTimer( uint16_t type )
{
	DI();

	switch ( type ){
		case MS_4:
			dali_timeout.timecount_4ms = MS_4 + FORWARD_FRAME_SIZE;
			break;
		case MS_8:
			dali_timeout.timecount_8ms = MS_8 + FORWARD_FRAME_SIZE;
			break;
		case MS_10:
			dali_timeout.timecount_10ms = MS_10 + FORWARD_FRAME_SIZE;
			break;
		case MS_50:
			dali_timeout.timecount_50ms = MS_50 + FORWARD_FRAME_SIZE;
			break;
		case MS_100:
			dali_timeout.timecount_100ms = MS_100 + FORWARD_FRAME_SIZE;
			break;
		case MIN_15:
			dali_timeout.timecount_15min = MIN_15 + FORWARD_FRAME_SIZE;
			break;
		default:
			break;
	}

	EI();
}


/******************************************************************************
* Function Name : DALI_StopTimer
* Description : Stop timer count.
* Argument : type
* Return Value : none
******************************************************************************/
void DALI_NameDeviceStopTimer( uint16_t type )
{
	DI();

	switch ( type ){
		case MS_4:
			dali_timeout.timecount_4ms = 0;
			break;
		case MS_8:
			dali_timeout.timecount_8ms = 0;
			break;
		case MS_10:
			dali_timeout.timecount_10ms = 0;
			break;
		case MS_50:
			dali_timeout.timecount_50ms = 0;
			break;
		case MS_100:
			dali_timeout.timecount_100ms = 0;
			break;
		case MIN_15:
			dali_timeout.timecount_15min = 0;
			break;
		default:
			break;
	}


	EI();
}

/******************************************************************************
* Function Name : DALI_IsTimerRunning
* Description : Check the timer count is running or not.
* Argument : type:timer mode
* Return Value : TRUE:running  FALSE:stopped
******************************************************************************/
uint8_t DALI_IsNameDeviceTimerRunning( uint16_t type )
{
	uint8_t ret = FALSE;

	switch ( type ){
		case MS_4:
			if(dali_timeout.timecount_4ms)
				ret = TRUE;
			break;
		case MS_8:
			if(dali_timeout.timecount_8ms)
				ret = TRUE;
			break;
		case MS_10:
			if(dali_timeout.timecount_10ms)
				ret = TRUE;
			break;
		case MS_50:
			if(dali_timeout.timecount_50ms)
				ret = TRUE;
			break;
		case MS_100:
			if(dali_timeout.timecount_100ms)
				ret = TRUE;
			break;
		case MIN_15:
			if(dali_timeout.timecount_15min)
				ret = TRUE;
			break;
		default:
			break;
	}
	return ret;
}


/******************************************************************************
* Function Name : DALI_RegisterConfigTimer
* Description : RegisterTimer.
* Argument : size
* Return Value : value
******************************************************************************/
void DALI_RegisterDeviceNameTimer(uint16_t type, fn_t func)
{
	switch ( type ){
		case MS_4:
			dali_timeout.func_timecount_4ms = func;
			break;
		case MS_8:
			dali_timeout.func_timecount_8ms = func;
			break;
		case MS_10:
			dali_timeout.func_timecount_10ms = func;
			break;
		case MS_50:
			dali_timeout.func_timecount_50ms = func;
			break;
		case MS_100:
			dali_timeout.func_timecount_100ms = func;
			break;
		case MIN_15:
			dali_timeout.func_timecount_15min = func;
			break;
		default:
			break;
	}
}
