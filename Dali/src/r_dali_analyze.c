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
* File Name : DALI_analyze.c
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

/******************************************************************************
Private global variables and functions
******************************************************************************/
static uint8_t DALI_Check2ndCommand( uint8_t ucCommandByte );
static uint8_t DALI_CheckAddress(uint8_t ucAddressByte);


/******************************************************************************
* Function Name : DALI_CheckConfigCommand
* Description : Check the two commannds or not for configuration command.
* Argument : command
* Return Value : TRUE:continue, FALSE:skip
******************************************************************************/
uint8_t DALI_CheckConfigCommand( uint8_t command )
{
	uint8_t ret = FALSE;
	
	/* Configuration commands(32...129) must be sent 2x within 100ms. */
	
	/* If 1st command received, set the timer count and countdown. */
	/* If 2nd command received within 100ms is not same as the 1st, the commands are ignored. */  
	if (DALI_IsTimerRunning( TIMER_CMD )){
		if ( dali_last_command[dali_led_number] == command ){
			ret = TRUE;
		}
		dali_last_command[dali_led_number] = COMMAND_NONE;
		DALI_StopTimer( TIMER_CMD );

	}else{
		dali_last_command[dali_led_number] = command;
		DALI_StartTimer( TIMER_CMD );
	}

	return ret;

}

/******************************************************************************
* Function Name : DALI_Check2ndCommand
* Description : Check the two commannds or not.
* Argument : command
* Return Value : TRUE:continue, FALSE:skip
******************************************************************************/
uint8_t DALI_Check2ndCommand( uint8_t command )
{
	/* Configuration commands(32...129) must be sent 2x within 100ms. */
	
	/* If the timer countdown running, check this command same as the last command or not. */
	if ( DALI_IsTimerRunning( TIMER_CMD )){
		if (dali_last_command[dali_led_number] != command ){
			dali_last_command[dali_led_number] = COMMAND_NONE;
			DALI_StopTimer( TIMER_CMD );
			return FALSE;
		}
	}
	dali_last_command[dali_led_number] = command;
	return TRUE;		
}

/******************************************************************************
* Function Name : DALI_AnalyzeCommand
* Description : Analyze the DALI command.
* Argument : dalicommand
* Return Value : none
******************************************************************************/
void DALI_AnalyzeCommand(uint16_t dalicommand)
{
	uint8_t cmd_address = (uint8_t)((dalicommand >> 8) & 0x00FF);
	uint8_t cmd_data = (uint8_t)(dalicommand & 0x00FF);

	if (cmd_address<=0x9F || cmd_address>=0xFE){
		/* Check Address */
		if (DALI_CheckAddress(cmd_address)){
			if ((cmd_address & 0x01)==0){
				/* Direct Arc Power Control */
				if (DALI_IsTimerRunning(TIMER_DAPC_INTERVAL)){
					DALI_StartTimer( TIMER_DAPC_INTERVAL );
				}else{
					DALI_StopTimer( TIMER_DAPC_INTERVAL );
				}
				
				if ( DALI_Check2ndCommand( cmd_data ) ){
					DALI_SetArcPowerWithFade(cmd_data);
				}
				
			}else{
				/* Command */
				DALI_StopTimer( TIMER_DAPC_INTERVAL );

				if ( DALI_Check2ndCommand( cmd_data ) ){
					if (cmd_data< (COMMAND_RESET)){
						/* Arc Power Control Commands */
						DALI_LightingCommand( cmd_data );

					}else if (cmd_data<=COMMAND_ENABLE_WRITE_MEMORY){
						/* Configuration Commands */
						if (DALI_CheckConfigCommand( cmd_data )){
							DALI_ConfigCommand( cmd_data );
						}
					}else if (cmd_data<COMMAND_QUERY_STATUS){
						/* reserved */
					
					}else if (cmd_data<0xE0){
						/* Query Commands */ 	
						DALI_QueryCommand( cmd_data );
						
					}else{
						/* Application Extended Commands */ 	
						//ä÷êîÇópà”Ç∑ÇÈÅB
					
					}
				}
			}
		}
		
	}else{
		/* Special Command */
		if ( DALI_Check2ndCommand( cmd_address ) ){ 
			DALI_SpecialCommand( cmd_address, cmd_data );
		}
	}

}

/******************************************************************************
* Function Name : DALI_CheckAddress
* Description : Check the address byte.
* Argument : cmd_address
* Return Value : TRUE:continue, FALSE:skip
******************************************************************************/
uint8_t DALI_CheckAddress(uint8_t  cmd_address)
{
	uint8_t ret = FALSE;
	if ((cmd_address & ADDRESS_BROADCAST)==ADDRESS_BROADCAST){
		/* broadcast */
		ret = TRUE;
	}else if ((cmd_address & 0x80) == 0x00){
		/* short address */
		if (((cmd_address>>1)&0x3F) == dali_current_variable->short_address){
			/* match address */
			ret = TRUE;
		}
	}else{
		/* group address */
		uint8_t group_address = (cmd_address>>1)&0x0f;
		uint8_t mask_group = MASK_GROUP[group_address];
	
		if ( group_address < 8 ){
			if ((dali_current_variable->group_0_7 & mask_group)){
				ret = TRUE;
			}
		}else{
			if ((dali_current_variable->group_8_15 & mask_group)){
				ret = TRUE;
			}
		}
		
	}
	return ret;
}

