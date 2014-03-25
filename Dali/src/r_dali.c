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
* File Name : r_dali.c
* Device(s) : 
* Description : This file implement DALI protocol. 
******************************************************************************/

#pragma	SFR
#pragma NOP
#pragma DI
#pragma EI

#define __DALI_C__

/******************************************************************************
Includes <System Includes> , ÅgProject IncludesÅh
******************************************************************************/
#include <string.h>
#include "r_macrodriver.h"
#include "r_user.h"
#include "r_led.h"
#include "r_dali.h"
#include "r_dali_analyze.h"
#include "r_dali_command.h"
#include "r_dali_timer.h"
#include "r_dali_variable.h"

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern const uint8_t enable_channel[NUMBER_OF_CHANNEL];

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
const uint8_t MASK_SAVE_REQUEST[4] = { 0x01,0x02,0x04,0x08 };

uint8_t dali_actual_level[NUMBER_OF_CHANNEL];		/* Actual Level */
uint8_t dali_actual_target[NUMBER_OF_CHANNEL];		/* for fading */
uint8_t dali_last_actual_level[NUMBER_OF_CHANNEL];
uint8_t dali_status_withdraw[NUMBER_OF_CHANNEL];	/* if true, exclude compare process */
uint8_t dali_current_dtr[NUMBER_OF_CHANNEL];		/* DTR.   */
uint8_t dali_current_dtr1[NUMBER_OF_CHANNEL];		/* DTR 1. */
uint8_t dali_current_dtr2[NUMBER_OF_CHANNEL];		/* DTR 2. */
uint8_t dali_fade_direction[NUMBER_OF_CHANNEL];
uint8_t dali_last_command[NUMBER_OF_CHANNEL];
uint8_t dali_search_address_h;						/* Search Address(H) */
uint8_t dali_search_address_l;						/* Search Address(M) */
uint8_t dali_search_address_m;						/* Search Address(L) */
uint8_t dali_physical_selected;
uint8_t dali_led_number;							/* LED number (0-3) */
uint8_t dali_answer_ready;
uint8_t dali_answer;
uint8_t dali_actualSaveChannel;

DALI_VARS_T	*dali_current_variable;					/* current variable */
STATUS_U	dali_status[NUMBER_OF_CHANNEL];			/* Status */

/******************************************************************************
Private global variables and functions
******************************************************************************/
static uint8_t dali_save_request = 0x00;

static void DALI_ProhibitReception(uint16_t received_data);

/******************************************************************************
* Function Name : DALI_init
* Description : Initialize DALI funciton.
* Argument : none
* Return Value : none
******************************************************************************/
void DALI_init( void )
{
	#define	DATA_EXIST	(0x55AA00FF)

	uint8_t i;
	uint8_t return_code;

	/* Initialize variable. */
	dali_physical_selected = FALSE;
	dali_search_address_h = DEFAULT_SEARCHADDRESSH;
	dali_search_address_m = DEFAULT_SEARCHADDRESSM;
	dali_search_address_l = DEFAULT_SEARCHADDRESSL;
	dali_save_request	  = 0x00;

	for (i=0; i<NUMBER_OF_CHANNEL; i++){
		dali_fade_direction[i] = FADE_UP;
		dali_status_withdraw[i] = BIT_CLR;
		dali_current_dtr[i] = DEFAULT_DTR;
		dali_current_dtr1[i] = DEFAULT_DTR;
		dali_current_dtr2[i] = DEFAULT_DTR;
		STATUS_ALL(i) = DEFAULT_STATUS_INFORMATION;		/* Status initialized. */
	}

	/* Initialize flash-memory for Self Programmingand and read variable. */
	if (DALI_InitEmulation() ==INIT_OK ){
		for (i=0; i<NUMBER_OF_CHANNEL; i++){
			dali_current_variable = &dali_variable[i];
			dali_led_number = i;

			return_code = DALI_ReadVariables( i+1 );
			if ( (return_code==READ_NODATA) ||
			    ((return_code==READ_OK    ) && (dali_current_variable->dataexist!=DATA_EXIST)) ) {
				dali_current_variable->dataexist = DATA_EXIST;
				DALI_SaveVariables(i+1);		/* write default parameter */
			}
			if ( (return_code==READ_NODATA) || (return_code==READ_OK) ) {
				uint8_t power_on_level= dali_current_variable->power_on_level;
				uint8_t max_level		= dali_current_variable->max_level;
				uint8_t min_level		= dali_current_variable->min_level;

				dali_last_actual_level[i] = dali_current_variable->actual_level;	/* set saved last level */
				if ( power_on_level == 0 ){
					dali_actual_level[i] = 0;
				}else if ( power_on_level == 255 ){
					dali_actual_level[i] = dali_last_actual_level[i];
				}else if ( (power_on_level >= min_level) && (power_on_level <= max_level) ){
					dali_actual_level[i] = power_on_level;

				}else if ( power_on_level < min_level ){
					dali_actual_level[i] = min_level;
					STATUS_LIMIT_ERROR(i) = BIT_SET;
				
				}else if ( power_on_level > max_level ){
					dali_actual_level[i] = max_level;
					STATUS_LIMIT_ERROR(i) = BIT_SET;
				}

				if ( dali_current_variable->short_address == DEFAULT_SHORTADDRESS){
					STATUS_CONTROL_GEAR(i) = BIT_CLR;	/* unavail channel */
					STATUS_MISSING_SHORT_ADDRESS(i) = BIT_SET;
				}else{
					STATUS_MISSING_SHORT_ADDRESS(i) = BIT_CLR;
				}
			} else {
				/* Use reset-value, because reading failed. */
				DALI_SetSystemFailure(i);
			}
		}

	}else{
		/* Use reset-value, because Self Programmingand can't work. */  
		for (i=0; i<NUMBER_OF_CHANNEL; i++){
			DALI_SetSystemFailure(i);
		}
	}
	
	/* initialize the memory bank */
	DALI_InitMemorybank();

	/* Initialize the timer to control the communication interval.  */
	DALI_InitTimer();

	/* Initialize the Port  */
	SPS4	= 0x0006;						/* CK40 select fCLK/2^6 = 62.5kHz	*/

	SOC4	= 0x0001;						/* set DALI mode						*/
	SUCTL	= 0x0000;						/* set 2 wire mode (DALIRxD4/DALITxD4)	*/

	SMR40	= 0x0022;						/* set DALI(Tx) mode				*/
	SMR41	= 0x0122;						/* set DALI(Rx) mode				*/

	SCR40	= 0x8026;						/* set DALI(Tx) mode				*/
	SCR41	= 0x4427; 						/* set DALI(Rx) mode				*/

	SDR40	= 0xCE00;						/* Tx 1202 bps */
	SDR41	= 0xCE00;						/* Rx 1202 bps */

	SOL4	= 0x0000;						/* set DALI(Tx) output mode			*/
	SO4		= 0x0F0F;
	SOE4	= 0x0001;

	SIR41	= 0x0087;						/* clear Rx error					*/

	NFEN3	= 0x01;							/* noise filter on					*/

	PIOR1	= 0x02;

	P1.0	= 1;							/* set port mode DALI(Tx)			*/
	PM1.0	= 0;
	POM1.0	= 1;
	PM1.1	= 1;							/* set port mode DALI(Rx)			*/
	PIM1.1	= 1;

	STDLIF4	= 0;							/* interrupt flg clear				*/
	SRDLIF4	= 0;							/* interrupt flg clear				*/
	SREDLIF4= 0;							/* interrupt flg clear				*/
	STDLMK4	= 1;							/* INTSTDL4 interrupt disable		*/
	SRDLMK4	= 1;							/* INTSRDL4 interrupt disable		*/
	SREDLMK4= 1;							/* INTSREDL4 interrupt disable		*/

	SS4		= 0x0003;						/* start UART4 ch.0 and 1			*/
}

/******************************************************************************
* Function Name : DALI_getValue
* Description : Get the value.
* Argument : channel
* Return Value : value (0...255)
******************************************************************************/
uint8_t DALI_getValue( uint8_t channel )
{
	uint8_t level = 0;

	DALI_ActualLevelChangecheck(channel-1);
	
	/* Save variable in idle time. */
	if (dali_save_request!=0x00 && !DALI_IsTimerRunning(TIMER_WAIT_ANSWER)){
		if (dali_save_request & MASK_SAVE_REQUEST[channel-1]){
			if ( DALI_SaveVariables(channel) != WRITE_OK ){
				DALI_SetSystemFailure(channel-1);
			}
			DI();
			dali_save_request &= ~MASK_SAVE_REQUEST[channel-1];		/* Clear flag */
			EI();
		}
	}

	level = dali_actual_level[channel - 1];
	return DIMMING_CURVE(level);
}

/******************************************************************************
* Function Name : DALI_ActualLevelChangecheck
* Description : Change actual level to write FLASH.
* Argument : channel
* Return Value : none
******************************************************************************/
void DALI_ActualLevelChangecheck( uint8_t channel )
{
	DALI_VARS_T *dali_var;	
	uint8_t actual_now;
	uint8_t actual_fix;
	
	actual_now = dali_actual_level[channel];
	dali_var = &dali_variable[channel];
	actual_fix = dali_var->actual_level;

	if ( actual_fix != actual_now ){
		dali_actualSaveChannel = channel;
		if ( dali_last_actual_level[channel] != actual_now ){
			DALI_StartTimer( TIMER_ACTUAL_SAVING );
			dali_last_actual_level[channel] = actual_now;
		}else if ( !DALI_IsTimerRunning( TIMER_ACTUAL_SAVING ) ){
			dali_var->actual_level = actual_now;
			DALI_UpdateVariable(channel);
		}
	}

}

/******************************************************************************
* Function Name : DALI_INT_Receive
* Description : Interrupt handler to receive DALI command.
* Argument : none
* Return Value : none
******************************************************************************/
void DALI_ReceiveCommand( void )
{
	uint16_t received_data;
	uint16_t received_status;

	if (SRDLIF4 || SREDLIF4) {
		received_status = SSR41;
		received_data	= SDCL4;
		if ( received_status & 0x87) {
			SIR41			= received_status;
		} else if ( received_status & 0x20) {
			DALI_ProhibitReception(received_data);

			for (dali_led_number=0; dali_led_number<NUMBER_OF_CHANNEL; dali_led_number++){
				if (enable_channel[dali_led_number]==0) continue;		/* This channel is not use. */
				dali_current_variable = &dali_variable[dali_led_number];
				DALI_AnalyzeCommand(received_data);
			}
			if (dali_answer_ready==TRUE){
				DALI_StartTimer( TIMER_WAIT_ANSWER );
			}
		}
		SRDLIF4	= 0;
		SREDLIF4= 0;
	}
}

/******************************************************************************
* Function Name : DALI_Fading
* Description : Fading operation.
* Argument : channel
* Return Value : none
******************************************************************************/
void DALI_Fading( uint8_t channel)
{
	DALI_VARS_T *dali_var;	
	uint8_t max_level;
	uint8_t min_level;
	uint8_t actual_level = dali_actual_level[channel];
	
	dali_var = &dali_variable[channel];
	max_level = dali_var->max_level;
	min_level = dali_var->min_level;

	/* If the actual arc power level reach to the target, stop timer. */
	if (dali_actual_target[channel]!=0 && actual_level==dali_actual_target[channel]){
		DALI_StopFadeTimer(channel);
		return;
	}

    /*Update the actual arc power level. */
	switch ( dali_fade_direction[channel] ){
		case FADE_UP:/* fade up */
			if ( actual_level >= min_level ){
				actual_level++;
			}else{
				actual_level = min_level;
			}
			
			if ( actual_level >= max_level ){
				DALI_StopFadeTimer(channel);
				actual_level = max_level;
			}
			break;

		case FADE_DOWN:/* fade down */
			if ( (actual_level <= min_level) || (actual_level == 255) ){
				DALI_StopFadeTimer(channel);
				if ( dali_last_command[channel] != COMMAND_DOWN ){
					actual_level = 0;
				}
			}else{
				actual_level--;
			}
			
			break;

		default:
			break;
	}
	dali_actual_level[channel] = actual_level;
}

/******************************************************************************
* Function Name : DALI_UpdateVariable
* Description : Change flag to write DALI values to FLASH.
* Argument : channel
* Return Value : none
******************************************************************************/
void DALI_UpdateVariable( uint8_t channel )
{
	dali_save_request |= MASK_SAVE_REQUEST[channel];
}

/******************************************************************************
* Function Name : DALI_SetSystemFailure
* Description : Changet to system failure level.
* Argument : channel
* Return Value : none
******************************************************************************/
void DALI_SetSystemFailure( uint8_t channel )
{
	DALI_VARS_T *dali_var;
	uint8_t failure_level;

	/* The timer that controls the fade is stopped. */
	DALI_StopFadeTimer( channel );
	
	dali_var = &dali_variable[channel];
	failure_level = dali_var->system_failure_level;
	if( failure_level == 255 ){
		/* When the VAR_SYSTEM_FAILURE_LEVEL is 255(MASK), Actual Level is not changed.  */
		return;
	}else if( (failure_level > 0x00) && (failure_level < dali_var->min_level) ){
		dali_actual_level[channel] = dali_var->min_level;
		STATUS_LIMIT_ERROR(channel) = BIT_SET;

	}else if ( failure_level > dali_var->max_level ){
		dali_actual_level[channel] = dali_var->max_level;
		STATUS_LIMIT_ERROR(channel) = BIT_SET;

	}else{	/* MIN LEVEL<=failure_level<=MAX LEVEL  or failure_level==0*/
		dali_actual_level[channel] = failure_level;
		STATUS_LIMIT_ERROR(channel) = BIT_CLR;
	}

}

/******************************************************************************
* Function Name : DALI_ResetValue
* Description : Set reset value.
* Argument : none
* Return Value : none
******************************************************************************/
void DALI_ResetValue( )
{
	uint8_t i;

	dali_current_variable->power_on_level = RESET_POWERONLEVEL;
	dali_current_variable->system_failure_level = RESET_SYSTEMFAILURELEVEL;
	dali_current_variable->max_level = RESET_MAXLEVEL;
	dali_current_variable->min_level = dali_current_variable->physical_min_level;
	dali_current_variable->fade_rate = RESET_FADERATE;
	dali_current_variable->fade_time = RESET_FADETIME;
	dali_current_variable->random_address_h = RESET_RANDOMADDRESSH;
	dali_current_variable->random_address_m = RESET_RANDOMADDRESSM;
	dali_current_variable->random_address_l = RESET_RANDOMADDRESSL;
	dali_current_variable->group_0_7 = RESET_GROUP_0_7;
	dali_current_variable->group_8_15 = RESET_GROUP_8_15;
	
	for (i=0;i<=15;i++){
		dali_current_variable->scene[i] = RESET_SCENE;
	}
}

/******************************************************************************
* Function Name : DALI_CheckReset
* Description : Check reset state.
* Argument : none
* Return Value : none
******************************************************************************/
void DALI_CheckReset( )
{
	uint8_t i;
	uint8_t status = TRUE;

	if (dali_current_variable->power_on_level != RESET_POWERONLEVEL){
		status = FALSE;
	}
	if (dali_current_variable->system_failure_level != RESET_SYSTEMFAILURELEVEL){
		status = FALSE;
	}
	if (dali_current_variable->max_level != RESET_MAXLEVEL){
		status = FALSE;
	}
	if (dali_current_variable->min_level != dali_current_variable->physical_min_level){
		status = FALSE;
	}
	if (dali_current_variable->fade_rate != RESET_FADERATE){
		status = FALSE;
	}
	if (dali_current_variable->fade_time != RESET_FADETIME){
		status = FALSE;
	}
	if (dali_current_variable->random_address_h != RESET_RANDOMADDRESSH){
		status = FALSE;
	}
	if (dali_current_variable->random_address_m != RESET_RANDOMADDRESSM){;
		status = FALSE;
	}
	if (dali_current_variable->random_address_l != RESET_RANDOMADDRESSL){
		status = FALSE;
	}
	if (dali_current_variable->group_0_7 != RESET_GROUP_0_7){;
		status = FALSE;
	}
	if (dali_current_variable->group_8_15 != RESET_GROUP_8_15){;
		status = FALSE;
	}
	
	for (i=0;i<=15;i++){
		if (dali_current_variable->scene[i] != RESET_SCENE){
			status = FALSE;
			break;
		}
	}
	
	if (status){
	    STATUS_RESET_STATE(dali_led_number) = BIT_SET;
	}else{
	    STATUS_RESET_STATE(dali_led_number) = BIT_CLR;
	}
	
}


/******************************************************************************
* Function Name : DALI_CheckReset
* Description : Check reset state.
* Argument : none
* Return Value : none
******************************************************************************/
void DALI_ProhibitReception(uint16_t received_data)
{
	uint8_t prohibit = FALSE;
	uint8_t cmd1 = (uint8_t)(received_data>>8);
	uint8_t cmd2 = (uint8_t)received_data;

	if ((cmd1&0x01)==0x00){
		return;
	}else if ((cmd2>=0x90) && (cmd2<0x9E)){
		prohibit = TRUE;
	}else if ((cmd2>=0xA0) && (cmd2<0xA6)){
		prohibit = TRUE;
	}else if ((cmd2>=0xB0) && (cmd2<0xC6)){
		prohibit = TRUE;
	}else if ((received_data & 0x01FF)==0x01FF){
		prohibit = TRUE;
	}else if (received_data==0xA900){
		prohibit = TRUE;
	}else if (cmd1==0xB9){
		prohibit = TRUE;
	}else if (cmd1==0xBB){
		prohibit = TRUE;
	}

	if (prohibit){
		ST4 = 0x02;
		DALI_StartTimer(TIMER_PROHIBIT_RECEPTION);
	}
}
