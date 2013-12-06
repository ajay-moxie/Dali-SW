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
* File Name : r_dali_command.c
* Device(s) : 
* Description : This file implement DALI protocol. 
******************************************************************************/

#pragma	SFR

/******************************************************************************
Includes <System Includes> , ÅgProject IncludesÅh
******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "r_macrodriver.h"
#include "r_user.h"
#include "r_dali.h"
#include "r_dali_analyze.h"
#include "r_dali_command.h"
#include "r_dali_timer.h"
#include "r_dali_variable.h"

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
const uint8_t MASK_GROUP[16] = 
{
	0b00000001,	/* Group0 */
	0b00000010,	/* Group1 */
	0b00000100,	/* Group2 */
	0b00001000,	/* Group3 */
	0b00010000,	/* Group4 */
	0b00100000,	/* Group5 */
	0b01000000,	/* Group6 */
	0b10000000,	/* Group7 */
	0b00000001,	/* Group8 */
	0b00000010,	/* Group9 */
	0b00000100,	/* Group10 */
	0b00001000,	/* Group11 */
	0b00010000,	/* Group12 */
	0b00100000,	/* Group13 */
	0b01000000,	/* Group14 */
	0b10000000	/* Group15 */
};

/* This array is used to fade using FADE_RATE. */
/* This array consists of the number of steps every 10ms */
const unsigned long FADE_RATE_STEP[] =
{
	0,
	((unsigned long)(358   * 65536 / 100)),
	((unsigned long)(253   * 65536 / 100)),
	((unsigned long)(179   * 65536 / 100)),
	((unsigned long)(127   * 65536 / 100)),
	((unsigned long)( 89.4 * 65536 / 100)),
	((unsigned long)( 63.3 * 65536 / 100)),
	((unsigned long)( 44.7 * 65536 / 100)),
	((unsigned long)( 31.6 * 65536 / 100)),
	((unsigned long)( 22.4 * 65536 / 100)),
	((unsigned long)( 15.8 * 65536 / 100)),
	((unsigned long)( 11.2 * 65536 / 100)),
	((unsigned long)(  7.9 * 65536 / 100)),
	((unsigned long)(  5.6 * 65536 / 100)),
	((unsigned long)(  4.0 * 65536 / 100)),
	((unsigned long)(  2.8 * 65536 / 100)),
};

/* This array is used to fade using FADE_TIME. */
/* This array consists of the number of fade time(x10ms). */
const uint16_t  FADE_TIME[16] = 
{
	0,
	((uint16_t)( 0.7 * 100.0)),
	((uint16_t)( 1.0 * 100.0)),
	((uint16_t)( 1.4 * 100.0)),
	((uint16_t)( 2.0 * 100.0)),
	((uint16_t)( 2.8 * 100.0)),
	((uint16_t)( 4.0 * 100.0)),
	((uint16_t)( 5.7 * 100)),
	((uint16_t)( 8.0 * 100)),
	((uint16_t)(11.3 * 100)),
	((uint16_t)(16.0 * 100)),
	((uint16_t)(22.6 * 100)),
	((uint16_t)(32.0 * 100)),
	((uint16_t)(45.3 * 100)),
	((uint16_t)(64.0 * 100)),
	((uint16_t)(90.5 * 100)),
};


/******************************************************************************
Private global variables and functions
******************************************************************************/
static void DALI_Fade200ms( uint8_t ucFadeRate,uint8_t ucFadeDir );

/******************************************************************************
* Function Name : DALI_LightingCommand
* Description : Operation lighting control commands.
* Argument : cmd
* Return Value : none
******************************************************************************/
void DALI_LightingCommand( uint8_t cmd )
{
	uint8_t  actual_level = dali_actual_level[dali_led_number];
	uint8_t  max_level = dali_current_variable->max_level;
	uint8_t  min_level = dali_current_variable->min_level;

	/* disalble to write memory bank */
	enable_memory_bank = FALSE;

	/* Stop fading. */
	DALI_StopFadeTimer( dali_led_number );

	/* Lighting command */
	switch( cmd ){
		case COMMAND_OFF:/* OFF */
			/* Extinguish the lamp without fading */
			dali_actual_level[dali_led_number] = 0;
			STATUS_LIMIT_ERROR(dali_led_number) = BIT_CLR;
			STATUS_POWER_FAILURE(dali_led_number) = BIT_CLR;
			break;

		case COMMAND_UP:/* UP */
			/* Dim up 200 ms using the selected 'FADE RATE'. */
			/* No change if the arc power output is already at the 'MAX LEVEL'. */
			/* This command shall only affect with burning lamps. */
			if ( (actual_level != 0) && (actual_level < max_level) ){
				DALI_Fade200ms(dali_current_variable->fade_rate, FADE_UP );
				STATUS_LIMIT_ERROR(dali_led_number) = BIT_CLR;
			}
			break;

		case COMMAND_DOWN:/* DOWN */
			/* Dim down 200 ms using the selected 'FADE RATE'. */
			/* No change if the arc power output is already at the 'MIN LEVEL' */
			/* Lamp shall not be switched off via this command. */
			if ( (actual_level != 0) && (actual_level > min_level) ){
				DALI_Fade200ms( dali_current_variable->fade_rate, FADE_DOWN );
				STATUS_LIMIT_ERROR(dali_led_number) = BIT_CLR;
			}
			break;

		case COMMAND_STEP_UP:/* STEP UP */
			/* Set the actual arc power level one step higher immediately without fading. */
			/* No change if the arc power output is already at the 'MAX LEVEL'. */
			/* This command shall only affect with burning lamps. */
			if ( (actual_level!= 0) && (actual_level < max_level) ){
				dali_actual_level[dali_led_number]++;
				STATUS_LIMIT_ERROR(dali_led_number) = BIT_CLR;
			}
			break;

		case COMMAND_STEP_DOWN:/* STEP DOWN */
			/* Set the actual arc power level one step lower immediately without fading. */
			/* No change if the arc power output is already at the 'MIN LEVEL' .*/
			/* Lamps shall not be switched off via this command. */
			if ( (actual_level!= 0) && (actual_level > min_level) ){
				dali_actual_level[dali_led_number]--;
				STATUS_LIMIT_ERROR(dali_led_number) = BIT_CLR;
			}
			break;

		case COMMAND_RECALL_MAX_LEVEL:/* RECALL MAX LEVEL */
			/* Set the actual arc power level to the 'MAX LEVEL' without fading. */
			/* If the lamp is off it shall be ignited with this command. */
			dali_actual_level[dali_led_number] = max_level;
			STATUS_LIMIT_ERROR(dali_led_number) = BIT_CLR;
			STATUS_POWER_FAILURE(dali_led_number) = BIT_CLR;
			break;

		case COMMAND_RECALL_MIN_LEVEL:/* RECALL MIN LEVEL */
			/* Set the actual arc power level to the 'MIN LEVEL' without fading. */
			/* If the lamp is off it shall be ignited with this command. */
			dali_actual_level[dali_led_number] = min_level;
			STATUS_LIMIT_ERROR(dali_led_number) = BIT_CLR;
			STATUS_POWER_FAILURE(dali_led_number) = BIT_CLR;
			break;

		case COMMAND_STEP_DOWN_AND_OFF:/* STEP DOWN AND OFF */
			/* Set the actual arc power level one step lower immediately without fading. */
			/* If the actual arc power level is already at the ÅeMIN LEVELÅf, */
			/* the lamp shall be switched off by this command. */
			if ( actual_level <= min_level ){
				dali_actual_level[dali_led_number] = 0;
			}else{
				dali_actual_level[dali_led_number]--;
			}
			STATUS_LIMIT_ERROR(dali_led_number) = BIT_CLR;
			STATUS_POWER_FAILURE(dali_led_number) = BIT_CLR;
			break;

		case COMMAND_ON_AND_STEP_UP:/* ON AND STEP UP */
			/* Set the actual arc power level one step higher immediately without fading. */
			/* If the lamp is switched off the lamp shall be ignited with this command  */
			/* and shall be set to the 'MIN LEVEL'. */
			if ( actual_level==0){
				dali_actual_level[dali_led_number] = min_level;

			}else if ( actual_level < max_level ){
				dali_actual_level[dali_led_number]++;
			}
			STATUS_LIMIT_ERROR(dali_led_number) = BIT_CLR;
			STATUS_POWER_FAILURE(dali_led_number) = BIT_CLR;
			break;

		case COMMAND_ENABLE_DAPC_SEQUENCE:/* ENABLE DAPC SEQUENCE */
			DALI_StartTimer( TIMER_DAPC_INTERVAL );
			break;

		default:	/* GO TO SCENE */
			/* Set the actual arc power level to the value stored for scene XXXX using the actual fade time. */
			/* If the lamp is off it shall be ignited with this command. */
			if( (cmd>= COMMAND_GO_TO_SCENE) && (cmd<= (COMMAND_GO_TO_SCENE+15)) ){
				DALI_SetArcPowerWithFade( dali_current_variable->scene[cmd & 0x0F] );
			}
			break;

	}
	
}

/******************************************************************************
* Function Name : DALI_QueryCommand
* Description : Operation query commands.
* Argument : cmd
* Return Value : none
******************************************************************************/
void DALI_QueryCommand( uint8_t cmd )
{
	uint8_t answer;

	/* disalble to write memory bank */
	enable_memory_bank = FALSE;

	/* Query Commands*/
	switch ( cmd ){
		case COMMAND_QUERY_STATUS:/* QUERY STATUS */
			/* Answer 'STATUS INFORMATION'. */
			DALI_CheckReset();	/* Update RESET STATUS */
			if ( dali_actual_level[dali_led_number] != 0 ){
				STATUS_LAMP_POWER_ON(dali_led_number) = BIT_SET;
			}else{
				STATUS_LAMP_POWER_ON(dali_led_number) = BIT_CLR;
			}
			SET_ANSWER(STATUS_ALL(dali_led_number));
			
			break;
			
		case COMMAND_QUERY_CONTROL_GEAR:/* QUERY CONTROL GEAR */
			/* If there is a control gear with the given address that is able to communicate, answer 'YES'. */
			if ( !STATUS_CONTROL_GEAR(dali_led_number) ){
				SET_ANSWER(ANSWER_YES);
			}
			break;
			
		case COMMAND_QUERY_LAMP_FAILURE:/* QUERY LAMP FAILURE */
			/* If the lamp failure is foune,answer 'YES'. */
			if ( STATUS_LAMP_FAILURE(dali_led_number) ){
				SET_ANSWER(ANSWER_YES);
			}
			break;
			
		case COMMAND_QUERY_LAMP_POWER_ON:/* QUERY LAMP POWER ON */
			/* If the lamp power supply is turning on, answer 'YES'. */
			if ( dali_actual_level[dali_led_number] != 0 ){
				STATUS_LAMP_POWER_ON(dali_led_number) = BIT_SET;
				SET_ANSWER(ANSWER_YES);
			}else{
				STATUS_LAMP_POWER_ON(dali_led_number) = BIT_CLR;
			}
			break;
			
		case COMMAND_QUERY_LIMIT_ERROR:/* QUERY LIMIT ERROR */
			/* if  limit error is found, answer 'YES'. */
			if ( STATUS_LIMIT_ERROR(dali_led_number) ){
				SET_ANSWER(ANSWER_YES);
			}
			break;
			
		case COMMAND_QUERY_RESET_STATE:/* QUERY RESET STATE */
			/* If variable are equal to the reset value, answer 'YES'. */
			DALI_CheckReset();
			
			if (STATUS_RESET_STATE(dali_led_number) ){
				SET_ANSWER(ANSWER_YES);
			}
			break;
			
		case COMMAND_QUERY_MISSING_SHORT_ADDRESS:/* QUERY MISSING SHORT ADDRESS */
			/* If the control gear doesn't have a short address, answer 'YES'. */
			if ( STATUS_MISSING_SHORT_ADDRESS(dali_led_number) ){
				SET_ANSWER(ANSWER_YES);
			}
			break;
			
		case COMMAND_QUERY_VERSION_NUMBER:/* QUERY VERSION NUMBER */
			/* Answer shall be the 'VERSION NUMBER' as an 8 bit number. */
			SET_ANSWER(dali_current_variable->version_number);
			break;
			
		case COMMAND_QUERY_CONTENT_DTR:/* QUERY CONTENT DTR */
			/* Answer shall be the content of the DTR as an 8 bit number. */
			SET_ANSWER(dali_current_dtr[dali_led_number]);
			break;
			
		case COMMAND_QUERY_DEVICE_TYPE:/* QUERY DEVICE TYPE */
			/* Answer shall be an 8 bit number. */
			SET_ANSWER(dali_current_variable->device_type);
			break;
			
		case COMMAND_QUERY_PHYSICAL_MINIMUM_LEVEL:/* QUERY PHYSICAL MINIMUM LEVEL */
			/* Answer shall be the 'PHYSICAL MIN. LEVEL'. */
			SET_ANSWER(dali_current_variable->physical_min_level);
			break;
			
		case COMMAND_QUERY_POWER_FAILURE:/* QUERY POWER FAILURE */
			/* Answer shall be 'YES' if the control gear has not received a 'RESET' or
			   one of the following arc power control commands since the last power-on. */ 
			if ( STATUS_POWER_FAILURE(dali_led_number) ){
				SET_ANSWER(ANSWER_YES);
			}
			break;
			
		case COMMAND_QUERY_CONTENT_DTR1:
			SET_ANSWER(dali_current_dtr1[dali_led_number]);
			break;

		case COMMAND_QUERY_CONTENT_DTR2:
			SET_ANSWER(dali_current_dtr2[dali_led_number]);
			break;

		case COMMAND_QUERY_ACTUAL_LEVEL:/* QUERY ACTUAL LEVEL */
			/* Answer shall be this level as an 8 bit number. */ 
			SET_ANSWER(dali_actual_level[dali_led_number]);
			break;
			
		case COMMAND_QUERY_MAX_LEVEL:/* QUERY MAX LEVEL */
			/* Answer shall be 'MAX LEVEL' as an 8 bit number. */ 
			SET_ANSWER(dali_current_variable->max_level);
			break;
			
		case COMMAND_QUERY_MIN_LEVEL:/* QUERY MIN LEVEL */
			/* Answer shall be 'MIN LEVEL' as an 8 bit number. */ 
			SET_ANSWER(dali_current_variable->min_level);
			break;
			
		case COMMAND_QUERY_POWER_ON_LEVEL:/* QUERY POWER ON LEVEL */
			/* Answer shall be 'POWER ON LEVEL' as an 8 bit number. */ 
			SET_ANSWER(dali_current_variable->power_on_level);
			break;
			
		case COMMAND_QUERY_SYSTEM_FAILURE_LEVEL:/* QUERY SYSTEM FAILURE LEVEL */
			/* Answer shall be 'SYSTEM FAILURE LEVEL' as an 8 bit number. */ 
			SET_ANSWER(dali_current_variable->system_failure_level);
			break;
			
		case COMMAND_QUERY_FADE_SETTING:/* QUERY FADE SETTING */
			/* Answer shall be 'FADE TIME/FADE RATE' as an 8 bit number. */ 
			SET_ANSWER((dali_current_variable->fade_time << 4) + dali_current_variable->fade_rate)
			break;
			
		case COMMAND_QUERY_GROUPS_0_7:/* QUERY GROUPS 0-7 */
			/* Answer shall be belonging groups. */
			SET_ANSWER(dali_current_variable->group_0_7);
			break;
			
		case COMMAND_QUERY_GROUPS_8_15:/* QUERY GROUPS 8-15 */
			/* Answer shall be belonging groups. */
			SET_ANSWER(dali_current_variable->group_8_15);
			break;
			
		case COMMAND_QUERY_RANDOM_ADDRESS_H:/* QUERY RANDOM ADDRESS(H) */
			/* Answer the 8 high bits of the random address. */
			SET_ANSWER(dali_current_variable->random_address_h);
			break;
			
		case COMMAND_QUERY_RANDOM_ADDRESS_M:/* QUERY RANDOM ADDRESS(M) */
			/* Answer the 8 mid bits of the random address. */
			SET_ANSWER(dali_current_variable->random_address_m);
			break;
			
		case COMMAND_QUERY_RANDOM_ADDRESS_L:/* QUERY RANDOM ADDRESS(L) */
			/* Answer the 8 low bits of the random address. */
			SET_ANSWER(dali_current_variable->random_address_l);
			break;
			
		case COMMAND_READ_MEMORY_LOCATION:
			/* The answer shall be the 8-bit content of a memory location. */
			if (DALI_ReadMemorybank(dali_current_dtr1[dali_led_number],dali_current_dtr[dali_led_number],&answer)==TRUE){
				SET_ANSWER(answer);
				dali_current_dtr[dali_led_number]++;
				DALI_ReadMemorybank(dali_current_dtr1[dali_led_number],dali_current_dtr[dali_led_number],&dali_current_dtr2[dali_led_number]);
			}
			break;

		case COMMAND_QUERY_EXTENDED_VERSION_NUMBER:
			break;

		default:
			if ( (cmd >= COMMAND_QUERY_SCENE_LEVEL) && (cmd < COMMAND_QUERY_GROUPS_0_7) ){
				/* Answer shall be the arc power level of scene XXXX as an 8 bit number. */
				SET_ANSWER(dali_current_variable->scene[cmd & 0x0F]);
			}
			break;
	} /* case */
	
}

/******************************************************************************
* Function Name : DALI_ConfigCommand
* Description : Operation congifuration commands.
* Argument : cmd
* Return Value : none
******************************************************************************/
void DALI_ConfigCommand( uint8_t cmd )
{
	uint8_t update_variable = TRUE;	 /* If 'TRUE', it should be save. */
	uint8_t dtr = dali_current_dtr[dali_led_number];

	/* disalble to write memory bank */
	enable_memory_bank = FALSE;
	
	/* Config command */
	switch ( cmd ){
		case COMMAND_RESET:/* RESET */
			/* The variable shall be changed to their reset values. */
			DALI_ResetValue();

			dali_search_address_h = RESET_SEARCHADDRESSH;
			dali_search_address_m = RESET_SEARCHADDRESSM;
			dali_search_address_l = RESET_SEARCHADDRESSL;

			STATUS_ALL(dali_led_number) = RESET_STATUS_INFORMATION;
			dali_actual_level[dali_led_number] = RESET_ACTUAL_LEVEL;

			if ( dali_current_variable->short_address == DEFAULT_SHORTADDRESS ){
				STATUS_MISSING_SHORT_ADDRESS(dali_led_number) = BIT_SET;
			}else{
				STATUS_MISSING_SHORT_ADDRESS(dali_led_number) = BIT_CLR;
			}
			break;
			
		case COMMAND_STORE_ACTUAL_LEVEL_IN_THE_DTR:/* STORE ACTUAL LEVEL IN THE DTR */
			/* Store actual level in the DTR. */
			dali_current_dtr[dali_led_number] = dali_actual_level[dali_led_number];
			update_variable = FALSE;	/* It isn't necessary to save. */
			break;
			
		case COMMAND_STORE_THE_DTR_AS_MAX_LEVEL:/* STORE THE DTR AS MAX LEVEL */
			/* If DTR is below the "MIN LEVEL"  then the "MIN LEVEL" shall be */
			/* stored as the new "MAX LEVEL". */
			if ( dtr < dali_current_variable->min_level ){
				dtr = dali_current_variable->min_level;
			}

			/* If DTR is MASK then 254 shall be stored as MAX LEVEL. */
			if ( dtr == 255 ){
				dtr = RANGE_MAXLEVEL;
			}
	
			/* If the "ACTUAL LEVEL" is above the new "MAX LEVEL" it should be set to */
			/* the new "MAX LEVEL" immediately without fading. */
			if ( dtr < dali_actual_level[dali_led_number] ){
				if (DALI_IsFading(dali_led_number)){
					DALI_StopFadeTimer( dali_led_number );
				}
				dali_actual_level[dali_led_number] = dtr;
			}
			dali_current_variable->max_level = dtr;
			break;
			
		case COMMAND_STORE_THE_DTR_AS_MIN_LEVEL:/* STORE THE DTR AS MIN LEVEL */
			/* If DTR is less than the "PHYSICAL MIN LEVEL" , */
			/* then the "PHYSICAL MIN LEVEL" shall be stored as the new "MIN LEVEL". */
			if ( dtr < dali_current_variable->physical_min_level){
				dtr = dali_current_variable->physical_min_level;
			}
			
			/* If DTR is MASK or is above the "MAX LEVEL" , */
			/* then the "MAX LEVEL" shall be stored as the new "MIN LEVEL". */
			if ( dtr > dali_current_variable->max_level ){
				dtr = dali_current_variable->max_level;
			}

			/* If the "ACTUAL LEVEL" is below the new "MIN LEVEL" */
			/* it should be set to the new "MIN LEVEL" immediately without fading, */
			/* but if the "ACTUAL LEVEL" is "0" (OFF) it should not be affected by */
			/* "STORE THE DTR AS MIN LEVEL". */
			if ( dali_actual_level[dali_led_number] != 0 && dtr > dali_actual_level[dali_led_number] ){
				if (DALI_IsFading(dali_led_number)){
					DALI_StopFadeTimer( dali_led_number );
				}
				dali_actual_level[dali_led_number] = dtr;
			}
			dali_current_variable->min_level = dtr;
			break;
			
		case COMMAND_STORE_THE_DTR_AS_SYSTEM_FAILURE_LEVEL:/* STORE THE DTR AS SYSTEM FAILURE LEVEL */
			dali_current_variable->system_failure_level = dtr;
			break;
			
		case COMMAND_STORE_THE_DTR_AS_POWER_ON_LEVEL:/* STORE THE DTR AS POWER ON LEVEL */
			dali_current_variable->power_on_level = dtr;
			break;
			
		case COMMAND_STORE_THE_DTR_AS_FADE_TIME:/* STORE THE DTR AS FADE TIME */
			/* Set the 'FADE TIME'. */
			if ( dtr > 15 ){
				dtr = 15;
			}
			dali_current_variable->fade_time = dtr;
			break;
			
		case COMMAND_STORE_THE_DTR_AS_FADE_RATE:/* STORE THE DTR AS FADE RATE */
			/* Set the 'FADE RATE'. */
			if ( dtr == 0 ){
				dtr = 1;
			}else{
				if ( dtr > 15 ){
					dtr = 15;
				}
			}
			dali_current_variable->fade_rate = dtr;
			break;
			
		case COMMAND_STORE_DTR_AS_SHORT_ADDRESS:/* STORE DTR AS SHORT ADDRESS */
			/* Save the value in the DTR as new short address. */
			/* If the value of DTR is 0b11111111 then shall remove the short address. */
			if ( dtr == 0b11111111 ){
				dali_current_variable->short_address = DEFAULT_SHORTADDRESS;
				STATUS_MISSING_SHORT_ADDRESS(dali_led_number) = BIT_SET;
			}else if ( (dtr & 0b10000001) == 0b00000001 ){
				dali_current_variable->short_address = ((dtr >> 1) & 0b00111111);
				STATUS_MISSING_SHORT_ADDRESS(dali_led_number) = BIT_CLR;
			}else{
				update_variable = FALSE;
			}
			break;
			
		case COMMAND_ENABLE_WRITE_MEMORY:
			/* The command shall enable subsequent writes to memory. */
			enable_memory_bank = TRUE;
			break;

		default:
			if ( (cmd >= COMMAND_STORE_THE_DTR_AS_SCENE) && (cmd < COMMAND_REMOVE_FROM_SCENE) ){
				/* STORE THE DTR AS SCENE */
				dali_current_variable->scene[cmd & 0x0F] = dtr;
			
			}else if ( (cmd >= COMMAND_REMOVE_FROM_SCENE) && (cmd < COMMAND_ADD_TO_GROUP) ){
				/* REMOVE FROM SCENE */
				dali_current_variable->scene[cmd & 0x0F] = DEFAULT_SCENE;

			}else if ( (cmd >= COMMAND_ADD_TO_GROUP ) && (cmd < COMMAND_REMOVE_FROM_GROUP) ){
				/* ADD TO GROUP */
				uint8_t group = cmd & 0x0F;
				if ( group < 8 ){
					dali_current_variable->group_0_7 = (dali_current_variable->group_0_7 | MASK_GROUP[group]);
				}else{
					dali_current_variable->group_8_15 = (dali_current_variable->group_8_15 | MASK_GROUP[group]);
				}

			}else if ( (cmd >= COMMAND_REMOVE_FROM_GROUP ) && (cmd < COMMAND_STORE_DTR_AS_SHORT_ADDRESS) ){
				/* REMOVE FROM GROUP */
				uint8_t group = cmd & 0x0F;
				if ( group < 8 ){
					dali_current_variable->group_0_7 = (dali_current_variable->group_0_7 & (~MASK_GROUP[group]));
				}else{
					dali_current_variable->group_8_15 = (dali_current_variable->group_8_15 & (~MASK_GROUP[group]));
				}

			}else{
				/* It isn't necessary to save variable. */
				update_variable = FALSE;
				break;
			}
			break;
			
	}
	if (update_variable==TRUE){
		/* Prepare to save variables. */
		DALI_UpdateVariable(dali_led_number);
	}
}

/******************************************************************************
* Function Name : DALI_Fade200ms
* Description : Start fading by 200ms.
* Argument : fade rate, fade direction
* Return Value : none
******************************************************************************/
void DALI_Fade200ms( uint8_t fade_rate,uint8_t fade_direction )
{
	uint32_t step_rate = FADE_RATE_STEP[fade_rate];

	if ( step_rate != 0 ){
		dali_fade_direction[dali_led_number] = fade_direction;
		dali_actual_target[dali_led_number] = 0;
		DALI_StartFadeTimer(step_rate, 20 );
	}
}

/******************************************************************************
* Function Name : DALI_SetArcPowerWithFade
* Description : Set the arc power controlol with fading.
* Argument : level_new
* Return Value : none
******************************************************************************/

void DALI_SetArcPowerWithFade(uint8_t level_new)
{
	uint8_t min_level = dali_current_variable->min_level;
	uint8_t max_level = dali_current_variable->max_level;

	/* Clear flag */
	STATUS_LIMIT_ERROR(dali_led_number) = BIT_CLR;
	STATUS_POWER_FAILURE(dali_led_number) = BIT_CLR;

	if ( level_new == 0xFF ){
		/* Stop fading */
		DALI_StopFadeTimer( dali_led_number );
		return ;

	}else if ( level_new == 0x00 ){
		/* if the value below 'MIN LEVEL' then switches off. */
		level_new = min_level - 1;

	}else if ( level_new < min_level ){
		/* Control to 'MIN LEVEL'. */
		level_new = min_level;
		STATUS_LIMIT_ERROR(dali_led_number) = BIT_SET;

	}else if ( level_new > max_level ){
		/* Control to 'MAX LEVEL'. */
		level_new = max_level;
		STATUS_LIMIT_ERROR(dali_led_number) = BIT_SET;

	}

	if ( ( dali_current_variable->fade_time != 0 ) || DALI_IsTimerRunning(TIMER_DAPC_INTERVAL) ){
		uint8_t actual_level =  dali_actual_level[dali_led_number];
		uint8_t fade_steps;
		uint16_t fade_time;
		unsigned long fade_steps_per_10ms;

		if ( actual_level == 0 ){
			actual_level = min_level - 1;
		}

		if ( actual_level == level_new ){
			/* No Fading */
			DALI_StopFadeTimer( dali_led_number );
			return;

		}else if ( actual_level > level_new ){
			/* Fade 'ucLevel' to 'ucEndLevel' */
			dali_fade_direction[dali_led_number] = FADE_DOWN;
			fade_steps = actual_level - level_new;

		}else{
			/* Fade 'ucEndLevel' to 'ucLevel' */
			dali_fade_direction[dali_led_number] = FADE_UP;
			fade_steps = level_new - actual_level;
		}
	
		if (DALI_IsTimerRunning(TIMER_DAPC_INTERVAL)){
			/* DAPC sequence Fading control */
			fade_time = 20; /* 200ms */
		}else{
			fade_time = FADE_TIME[dali_current_variable->fade_time];
		}
		
		dali_actual_target[dali_led_number] = level_new;
		fade_steps_per_10ms = ((unsigned long)fade_steps) * 65536 / fade_time;
		DALI_StartFadeTimer( fade_steps_per_10ms , fade_time + 5 );

	}else{
		/* No Fading */
		DALI_StopFadeTimer( dali_led_number );
		if ( level_new < min_level ){
			level_new = 0;
		}
		dali_actual_level[dali_led_number] = level_new;
	}
}

/******************************************************************************
* Function Name : DALI_SpecialCommand
* Description : Operation special commands.
* Argument : cmd,data
* Return Value : none
******************************************************************************/
void DALI_SpecialCommand( uint8_t cmd,uint8_t data )
{
	//uint8_t ucRetValue;
	uint8_t ramdom_address_h = dali_current_variable->random_address_h;
	uint8_t ramdom_address_m = dali_current_variable->random_address_m;
	uint8_t ramdom_address_l = dali_current_variable->random_address_l;

	/* disalble to write memory bank */
	if (cmd!=EXCOMMAND_WRITE_MEMORY_LOCATION){
		enable_memory_bank = FALSE;
	}

	if (cmd>=EXCOMMAND_RANDOMISE && cmd<=EXCOMMAND_PHYSICAL_SELECTION){
		/* The command is effective only for the address setting period. */
		if ( !DALI_IsTimerRunning( TIMER_ADDRESSING_PERIOD )){
			return;
		}
	}
	
	/* Distribution of Extension command */
	switch ( cmd ){
		case EXCOMMAND_TERMINATE:/* TERMINATE command */
			/* The address setting period is stopped. */
			DALI_StopTimer( TIMER_ADDRESSING_PERIOD );
			dali_status_withdraw[dali_led_number] = BIT_CLR;
			break;
			
		case EXCOMMAND_DTR:/* DTR command */
			/* The value is stored in DTR. */
			dali_current_dtr[dali_led_number] = data;
			break;
			
		case EXCOMMAND_INITIALIZE:/* INITIALIZE command */
			if ( data == 0b00000000 ){
				/*All control gear shall react */
			}else if ( data == 0b11111111 ){
				/* Control gear without short address shall react */
				if ( dali_current_variable->short_address != DEFAULT_SHORTADDRESS){
					return;
				}
			}else if ( (data & 10000001) == 0b00000001 ){
				/* Control gear with the address AAAAAAb shall react */
				if ( dali_current_variable->short_address != ((data >> 1) & 0b00111111) ){
					return;
				}
			}else{
				return;
			}

			if (DALI_CheckConfigCommand( cmd )){
				DALI_StartTimer( TIMER_ADDRESSING_PERIOD );
			}

			/* set seed of random value for RANDOMIZE command */
			{
				uint16_t	random_seed = DALI_GetRandomValue(0xFFFF);
				srand( random_seed );
			}

			break;

		case EXCOMMAND_RANDOMISE:/* RANDOMISE command */
			if (DALI_CheckConfigCommand( cmd )){
				int	random_value;
				random_value = rand();
				dali_current_variable->random_address_h = (uint8_t)(random_value>>8);
				dali_current_variable->random_address_m = (uint8_t)random_value;
				random_value = rand();
				dali_current_variable->random_address_l = (uint8_t)random_value;
				DALI_UpdateVariable(dali_led_number);

				dali_status_withdraw[dali_led_number] = BIT_CLR;
			}
			break;

		case EXCOMMAND_COMPARE:/* COMPARE command */
			if (dali_physical_selected){
				/* Disable because physically selected. */
				break;
			}
			
			if (dali_status_withdraw[dali_led_number] == BIT_SET){
			 	/* Already WITHDRAW */
				break;
			}
			
			/* When RANDOM ADDRESS is smaller than SEARCH ADDRESS, YES is transmitted. */
			if ( ramdom_address_h > dali_search_address_h){
				break;

			}else if ( ramdom_address_h == dali_search_address_h && ramdom_address_m > dali_search_address_m){
				break;

			}else if ( (ramdom_address_h == dali_search_address_h) && (ramdom_address_m == dali_search_address_m) &&	(ramdom_address_l > dali_search_address_l) ){
				break;
			}

			SET_ANSWER(ANSWER_YES);
			break;

		case EXCOMMAND_WITHRAW:/* WITHDRAW command */
			/* When RANDOM ADDRESS is equal to SEARCH ADDRESS, the WITHDRAW setting is turned on. */
			if (dali_search_address_h!=ramdom_address_h){
				break;
			}
			if (dali_search_address_m!=ramdom_address_m){
				break;
			}
			if (dali_search_address_l!=ramdom_address_l){
				break;
			}
					
			dali_status_withdraw[dali_led_number] = BIT_SET;
			break;
					
		case EXCOMMAND_SEARCHADDRH:/* SEARCHADDRH command */
			dali_search_address_h = data;
			break;
					
		case EXCOMMAND_SEARCHADDRM:/* SEARCHADDRM command */
			dali_search_address_m = data;
			break;
					
		case EXCOMMAND_SEARCHADDRL:/* SEARCHADDRL command */
			dali_search_address_l = data;
			break;
					
		case EXCOMMAND_PROGRAM_SHORT_ADDRESS:/* PROGRAM SHORT ADDRESS command */
			/* Store short address when  SEARCH ADDRESS is equal to RANDOM ADDRESS, or physically selected. */
			if (!dali_physical_selected){
				if (dali_search_address_h!=ramdom_address_h){
					break;
				}
				if (dali_search_address_m!=ramdom_address_m){
					break;
				}
				if (dali_search_address_l!=ramdom_address_l){
					break;
				}
			}

			if (data==255){
				STATUS_MISSING_SHORT_ADDRESS(dali_led_number) = BIT_SET;
			}else{
				data = ((data >> 1) & 0b00111111);	/* get the short address */
				STATUS_MISSING_SHORT_ADDRESS(dali_led_number) = BIT_CLR;	
			}

			dali_current_variable->short_address = data;
			DALI_UpdateVariable(dali_led_number);
			break;
					
		case EXCOMMAND_VERIFY_SHORT_ADDRESS:/* VERIFY SHORT ADDRESS command */
			/* When it is equal to a short address, YES is returned. */
			if ( data == ((dali_current_variable->short_address << 1) | 0b00000001)){
				SET_ANSWER(ANSWER_YES);
			}
			break;
					
		case EXCOMMAND_QUERY_SHORT_ADDRESS:/* QUERY SHORT ADDRESS command */
			/* When SEARCH ADDRESS is equal to RANDOM ADDRESS, a short address is returned. */
			if (!dali_physical_selected){
				if (dali_search_address_h!=ramdom_address_h){
					break;
				}
				if (dali_search_address_m!=ramdom_address_m){
					break;
				}
				if (dali_search_address_l!=ramdom_address_l){
					break;
				}
			}
						
			SET_ANSWER((dali_current_variable->short_address << 1) | 0b00000001);
			break;
					
		case EXCOMMAND_PHYSICAL_SELECTION:/* PHYSICAL SELECTION command */
			dali_physical_selected = TRUE;
			break;
								
		case EXCOMMAND_ENABLE_DEVICE_TYPE_X:
			/* Not implement */
			break;

		case EXCOMMAND_DATA_TRANSFER_REGISTER_1:
			dali_current_dtr1[dali_led_number] = data;
			break;

		case EXCOMMAND_DATA_TRANSFER_REGISTER_2:
			dali_current_dtr2[dali_led_number] = data;
			break;

		case EXCOMMAND_WRITE_MEMORY_LOCATION:
			if (DALI_WriteMemorybank(dali_current_dtr1[dali_led_number],dali_current_dtr[dali_led_number],data)==TRUE){
				dali_current_dtr[dali_led_number]++;
				SET_ANSWER(data);
			}
			break;

		default:
			;
	}
}
