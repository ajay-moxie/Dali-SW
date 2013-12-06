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
#include "r_dali_command.h"
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


/******************************************************************************
* Function Name : DALI_InitTimer
* Description : Initialize the interval timer.
* Argument : none
* Return Value : none
******************************************************************************/
void DALI_InitTimer( void )
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

	Timer_StartInterval_1ms(DALI_Interval);
	
}

/******************************************************************************
* Function Name : DALI_Interval
* Description : Interval timer process.
* Argument : none
* Return Value : none
******************************************************************************/
void DALI_Interval( void )
{
	uint8_t i;

	/* ----- 1ms interval -----*/
	timecount_totaltime++;

	/* Check timer for send answer. */
	if ( timecount_answer != 0 ){
		timecount_answer--;
		if ( timecount_answer == 0 ){
			/* Send answer */
			SDTL4 = ((uint16_t)dali_answer) & 0x00FF;
			CLR_ANSWER;
		}
	}

	if ( timecount_prohibit_reception !=0 ){
		timecount_prohibit_reception--;
		if (timecount_prohibit_reception == 0){
			SS4 = 0x02;	 /* start reception */
		}
	}

	timecount_10ms--;
	if ( timecount_10ms == 0 ){
		timecount_10ms = 10;
		
		/* ----- 10ms interval -----*/
		for (i=0; i<NUMBER_OF_CHANNEL; i++){
			if (fade_count[i]>0){
				fade_steps[i] += fade_steps_per_10ms[i];
				while(fade_steps[i]>=0x10000){
					DALI_Fading(i);
					fade_steps[i] -= 0x10000;
				}
				fade_count[i]--;
				if (fade_count[i] == 0){
					STATUS_FADE_RUNNING(i) 	= BIT_CLR;
				}
			}

			/* Check command receive interval */
			if ( timecount_rcv_command[i] != 0 ){
				timecount_rcv_command[i]--;
			}

			/* Check for DAPC sequence */
			if ( timecount_dapc_sequence[i] != 0 ){
				timecount_dapc_sequence[i]--;
			}

			/* Check addressing period */
			if ( timecount_addressing[i] != 0 ){
				timecount_addressing[i]--;
				if (timecount_addressing[i]==0){
					dali_physical_selected = FALSE;
				}
			}

			/* Check to save value of actual level. */
			if ( timecount_actual_unchange[i] != 0 ){
				timecount_actual_unchange[i]--;
			}
		}

		/* check interface failure */
		if ( P1.1 == 0 ){
			if (timecount_interface_failure>0){
				timecount_interface_failure--;
				if (timecount_interface_failure==0){
					/* System failure at all channel */
					for (i=0; i<NUMBER_OF_CHANNEL; i++){
						DALI_SetSystemFailure(i);
					}
				}
			}
		}else{
			timecount_interface_failure = TIME_INTERFACE_FAILURE;
		}
		
	}

}

/******************************************************************************
* Function Name : DALI_StartTimer
* Description : Start timer count.
* Argument : type
* Return Value : none
******************************************************************************/
void DALI_StartTimer( uint8_t type )
{
	DI();

	switch ( type ){
		case TIMER_WAIT_ANSWER:
			if (timecount_answer==0){
				timecount_answer = TIME_WAIT_ANSWER;	/* 3-9ms */	
			}
			break;
		
		case TIMER_PROHIBIT_RECEPTION:
			timecount_prohibit_reception = TIME_PROHIBIT_RECEPTION;
			break;

		case TIMER_CMD:
			timecount_rcv_command[dali_led_number] = TIME_RECEIVE_INTERVAL;		/* 100ms */
			break;
		
		case TIMER_ADDRESSING_PERIOD:
			timecount_addressing[dali_led_number] = TIME_ADDRESSING_PERIOD;		/* 15 minuites */
			break;
		
		case TIMER_DAPC_INTERVAL:
			timecount_dapc_sequence[dali_led_number] = TIME_DAPCSEQUENCE_CMD;	/* 200ms */
			break;
			
		case TIMER_ACTUAL_SAVING:
			timecount_actual_unchange[dali_actualSaveChannel] = TIME_SAVE_ACTUAL;		/* 500ms set */
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
void DALI_StopTimer( uint8_t type )
{
	DI();

	switch ( type ){
		case TIMER_WAIT_ANSWER :
			timecount_answer = 0;
			break;

/** Not use
		case TIMER_PROHIBIT_RECEPTION:
			timecount_prohibit_reception = 0;
			break;
**/

		case TIMER_CMD:
			timecount_rcv_command[dali_led_number] = 0;
			break;
		
		case TIMER_ADDRESSING_PERIOD:
			timecount_addressing[dali_led_number] = 0;
			dali_physical_selected = FALSE;
			break;
		
		case TIMER_DAPC_INTERVAL:
			timecount_dapc_sequence[dali_led_number] = 0;
			break;
			
		case TIMER_ACTUAL_SAVING:
			timecount_actual_unchange[dali_actualSaveChannel] = 0;
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
uint8_t DALI_IsTimerRunning( uint8_t type )
{
	uint8_t ret = FALSE;
	
	switch ( type ){
		case TIMER_WAIT_ANSWER :
			if (timecount_answer!=0){
				ret = TRUE;
			}
			break;
		
/** Not use
		case TIMER_PROHIBIT_RECEPTION:
			if (timecount_prohibit_reception!=0){
				ret = TRUE;
			}
			break;
**/
		case TIMER_CMD:
			if (timecount_rcv_command[dali_led_number]!=0){
				ret = TRUE;
			}
			break;
		
		case TIMER_ADDRESSING_PERIOD:
			if (timecount_addressing[dali_led_number]!=0){
				ret = TRUE;
			}
			break;
		
		case TIMER_DAPC_INTERVAL:
			if (timecount_dapc_sequence[dali_led_number]!=0){
				ret = TRUE;
			}
			break;
			
		case TIMER_ACTUAL_SAVING:
			if (timecount_actual_unchange[dali_actualSaveChannel]!=0){
				ret = TRUE;
			}
			break;
			
		default:
			break;
	}

	return ret;
}

/******************************************************************************
* Function Name : DALI_StartFadeTimer
* Description : Start fading.
* Argument : fadestep :  fade step x 65536 (0x00010000-0x00FE0000)
*            fadetime :  fade time / 10ms (ex. 200ms Å® fadetime=20)
* Return Value : none
******************************************************************************/
void DALI_StartFadeTimer( uint32_t fadestep,uint16_t fadetime )
{
	DI();
	fade_steps_per_10ms[dali_led_number] = fadestep;
	fade_count[dali_led_number]		    = fadetime;
	STATUS_FADE_RUNNING(dali_led_number) = BIT_SET;
	EI();

}

/******************************************************************************
* Function Name : DALI_StopFadeTimer
* Description : Stop fading.
* Argument : channel
* Return Value : none
******************************************************************************/
void DALI_StopFadeTimer( uint8_t channel )
{
	DI();

	fade_count[channel] 			= 0;
	fade_steps_per_10ms[channel]	= 0;
	
	STATUS_FADE_RUNNING(channel)	= BIT_CLR;
	EI();

}

/******************************************************************************
* Function Name : DALI_IsFading
* Description : Check fading or not.
* Argument : channel
* Return Value :TRUE:fading  FALSE:not fading
******************************************************************************/
uint8_t DALI_IsFading( uint8_t channel )
{
	if (fade_count[channel] == 0){
		return FALSE;
	}
	
	return TRUE;
}

/******************************************************************************
* Function Name : DALI_GetRandomValue
* Description : Get random value for RANDOMIZE command.
* Argument : size
* Return Value : value
******************************************************************************/
uint16_t DALI_GetRandomValue( uint16_t size )
{
	/* Used for RANDOMISE command */
	return (timecount_totaltime & size);
}

