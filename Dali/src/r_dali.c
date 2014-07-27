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
#pragma interrupt	INTSRDL4 DALI_ReceiveCommand
#pragma interrupt	INTSTDL4 DALI_SendIntr
/******************************************************************************
Includes <System Includes> , ÅgProject IncludesÅh
******************************************************************************/
#include <string.h>
#include "r_macrodriver.h"
#include "r_user.h"
#include "r_dali.h"
#include "r_dali_analyze.h"
#include "r_dali_timer.h"
#include "r_dali_variable.h"
#include "r_dali_enumerate.h"
#include "r_dali_slave.h"
#include "r_master_state_machine.h"

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern const uint8_t enable_channel[NUMBER_OF_CHANNEL];
extern uint8_t enumeration_required;

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
const uint8_t MASK_SAVE_REQUEST[4] = { 0x01,0x02,0x04,0x08 };
static uint8_t dali_recv_circular_buff[DALI_MAX_SLAVE];
static uint8_t dali_read_index = DALI_MAX_SLAVE - 1;
static uint8_t dali_write_index = DALI_MAX_SLAVE - 1;
static void (*DALI_ExtRxHandler)();

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
static uint8_t sent_err_int;

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

	DALI_ExtRxHandler = NULL;
	
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
	STDLMK4	= 0;							/* INTSTDL4 interrupt disable		*/
	SRDLMK4	= 0;							/* INTSRDL4 interrupt disable		*/
	SREDLMK4= 0;							/* INTSREDL4 interrupt disable		*/

	SS4		= 0x0003;						/* start UART4 ch.0 and 1			*/
//	DALI_slave_initialize();
//	DALI_Enumerate(0);
}

/******************************************************************************
* Function Name : Dali_IsDwnResponseNeeded
* Description : Checks if response to down stream device needed.
* Argument : command
* Return Value : yes/no
******************************************************************************/
uint32_t Dali_IsDwnResponseNeeded( uint16_t command )
{
	uint8_t cmd;
	uint8_t add;
	
	cmd = (uint8_t)command;
	add = (uint8_t)(command >> 8); //with byte 0 and byte 7

	if((command >> 8) && 0x1) //direct arc power
		return 0;
	else if(((cmd >= 144) && (cmd <= 155)) || (cmd == 159) || ((cmd >= 160) && (cmd <= 196)))
		return 1;
	return 0;
}
/******************************************************************************
* Function Name : DALI_SendCommand
* Description : DALI command send.
* Argument : Command
* Return Value : none
******************************************************************************/
void DALI_SendCommand(uint16_t Command)
{
	dali_write_index = dali_read_index; //flush any earlier data
	sent_err_int = 2;//two error interrupts are received for each send
	SDTL4 = (uint16_t)Command;
}

/******************************************************************************
* Function Name : DALI_INT_Receive
* Description : Interrupt handler to receive DALI command.
* Argument : none
* Return Value : none
******************************************************************************/
__interrupt void DALI_SendIntr( void )
{
	STDLIF4 = 0;
}

/******************************************************************************
* Function Name : DALI_TestReceive
* Description : Test receive, it fills circular buffer for testing.
* Argument : none
* Return Value : none
******************************************************************************/
void DALI_TestReceive( uint8_t data )
{
	dali_write_index = (dali_write_index + 1) % DALI_MAX_SLAVE;
	dali_recv_circular_buff[dali_write_index] = (uint8_t)data;
}
/******************************************************************************
* Function Name : DALI_INT_Receive
* Description : Interrupt handler to receive DALI command.
* Argument : none
* Return Value : none
******************************************************************************/
uint8_t DALI_ReadData( uint8_t *data )
{
	if(dali_write_index == dali_read_index)
		return 0;
	dali_read_index = (dali_read_index + 1) % DALI_MAX_SLAVE;
	*data = dali_recv_circular_buff[dali_read_index];
	return 1;
}

/******************************************************************************
* Function Name : DALI_RegisterExtRxHandler
* Description : Extended Interrupt handler for Dali receive.
* Argument : none
* Return Value : none
******************************************************************************/
void DALI_RegisterExtRxHandler(void (*rx_ext)(void))
{
	DALI_ExtRxHandler = rx_ext;
}

/******************************************************************************
* Function Name : DALI_UnRegisterExtRxHandler
* Description : Extended Interrupt handler for Dali receive.
* Argument : none
* Return Value : none
******************************************************************************/
void DALI_UnRegisterExtRxHandler(void (*rx_ext)(void))
{
	DALI_ExtRxHandler = NULL;
}
/******************************************************************************
* Function Name : DALI_INT_Receive
* Description : Interrupt handler to receive DALI command.
* Argument : none
* Return Value : none
******************************************************************************/
__interrupt void DALI_ReceiveCommand( void )
{
	uint16_t received_data;
	uint16_t received_status;
	static int ucCount = 0;

	received_status = SSR41;
	received_data	= SDCL4;

	if ( received_status & 0x87) {
		SIR41			= received_status;
		if((!sent_err_int) && (get_master_state() == ENUMERATION)){
			dali_write_index = (dali_write_index + 1) % DALI_MAX_SLAVE;
			dali_recv_circular_buff[dali_write_index] = (uint8_t)YES;
		}
		sent_err_int--;
		
	} else if ( received_status & 0x20) {
		dali_write_index = (dali_write_index + 1) % DALI_MAX_SLAVE;
		dali_recv_circular_buff[dali_write_index] = (uint8_t)received_data;
		if(DALI_ExtRxHandler)
			DALI_ExtRxHandler();
	}
	SRDLIF4	= 0;
	SREDLIF4= 0;
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
