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
* File Name : r_user_init.c
* Device(s) : 
* Description : This file implements dimmer program and LED initialization.
******************************************************************************/

#pragma	SFR

/******************************************************************************
Includes <System Includes> , ÅgProject IncludesÅh
******************************************************************************/
#include "r_macrodriver.h"
#include "r_user.h"
#include "r_dali.h"
#include "r_uart.h"
#include "r_timer.h"
#include "host_communication.h"
#include "r_master_state_machine.h"
#include "r_dali_slave.h"
#include "r_host_variable.h"
#include "config.h"

void host_init( void );

/******************************************************************************
* Function Name : user_init
* Description : initialize.
* Argument : none
* Return Value : none
******************************************************************************/
void user_init( void )
{
	t_host_comm host_comm;
	Timer_Init();
	DALI_init( );
	UART_init( );
	
#ifdef CONFIG_UART1
	UART1_start();
#endif
#ifdef CONFIG_UART0
	UART0_start();
#endif
	master_state_machine_init();
	DALI_slave_initialize();
	host_init();
	
	host_comm.usp_tx = USP_TX_FN;
	host_comm.usp_rx = USP_RX_FN;
	host_comm.dwn_rx = DALI_ReadData;
	host_comm.dwn_tx = DALI_SendCommand;
	host_comm.dwn_response = Dali_IsDwnResponseNeeded;
	host_comm.dwn_register_rx_handler = DALI_RegisterExtRxHandler;
	host_comm.dwn_unregister_rx_handler = DALI_UnRegisterExtRxHandler;
	host_RegisterTxRx(host_comm);
}

/******************************************************************************
* Function Name : host_init
* Description : initialize.
* Argument : none
* Return Value : none
******************************************************************************/
void host_init( void )
{
	struct dali_slave aa[40];
	uint8_t return_code;
	uint8_t i = 1;
	if (Host_InitEmulation() ==INIT_OK ){
			return_code = DALI_read_saved_slave_data();
			if ( (return_code==READ_NODATA) ||
			    ((return_code==READ_OK ) && (!DALI_slave_data_exist())) ) {
				DALI_slave_initialize();
				DALI_save_slave_data();
			}
	}

}
