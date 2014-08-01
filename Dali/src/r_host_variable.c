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
* File Name : r_selfprogramming.c
* Device(s) : 
* Description : This file implements device driver for self programming.
******************************************************************************/

#pragma	SFR
#pragma	DI
#pragma	EI

/******************************************************************************
Includes <System Includes> , ÅgProject IncludesÅh
******************************************************************************/
#include "r_macrodriver.h"
#include "r_user.h"
#include "r_dali.h"
#include "r_host_variable.h"
#include "eel.h"
#include "eel_types.h"
#include "fdl_types.h"
#include "fdl_descriptor.h"
#include "fdl.h"
#include "r_dali_slave.h"

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* data written in flash memory */

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/


/******************************************************************************
* Function Name : self_Init
* Description : Initialize the EEPROM emulation.
* Argument : void
* Return Value : INIT_OK / STATUS_NG
******************************************************************************/
unsigned char Host_InitEmulation( void )
{
	/* writing permission status */
	uint8_t Status = STATUS_NG;
	eel_request_t	request;

	if ( FAL_Init( (const __far fal_descriptor_t*)&fal_descriptor_str) !=FAL_OK ) {
		goto exit_Init;
	}

	if ( EEL_Init() != EEL_OK ) {
		goto exit_Init;
	}

	EEL_Open();

	while (1) {
		request.address_pu08	= 0x00;
		request.identifier_u08	= 0x00;
		request.timeout_u08		= 0xFF;
		request.command_enu		= EEL_CMD_STARTUP;
		request.status_enu		= EEL_BUSY;

		EEL_Execute( (eel_request_t*)&request );

		if ( request.status_enu == EEL_OK ) {
			Status	= INIT_OK;
			goto exit_Init;

		} else if ( request.status_enu == EEL_ERR_POOL_INCONSISTENT ) {
			request.address_pu08	= 0x00;
			request.identifier_u08	= 0x00;
			request.timeout_u08		= 0xFF;
			request.command_enu		= EEL_CMD_FORMAT;
			request.status_enu		= EEL_BUSY;

			EEL_Execute( (eel_request_t*)&request );

			if ( request.status_enu != EEL_OK ) {
				goto exit_Init;
			}
		} else {
			goto exit_Init;
		}
	}

exit_Init:
	EEL_Close();
	return Status;
}

/******************************************************************************
* Function Name : Host_ReadVariables
* Description : Read data to use EEPROM emulation.
* Argument : DataNumber
* Return Value : READ_OK / STATUS_NG
******************************************************************************/
uint8_t Host_ReadVariables( uint8_t DataNumber, void *addr  )
{
	/* writing permission status */
	uint8_t Status = STATUS_NG;
	eel_request_t	request;

	EEL_Open();

	request.address_pu08	= 0x00;
	request.identifier_u08	= 0x00;
	request.timeout_u08		= 0xFF;
	request.command_enu		= EEL_CMD_STARTUP;
	request.status_enu		= EEL_BUSY;

	EEL_Execute( (eel_request_t*)&request );

	if ( request.status_enu != EEL_OK ) {
		goto exit_Read;
	}

	request.address_pu08	= (eel_u08 *)addr;
	request.identifier_u08	= DataNumber;
	request.timeout_u08		= 0xFF;
	request.command_enu		= EEL_CMD_READ;
	request.status_enu		= EEL_BUSY;

	EEL_Execute( (eel_request_t*)&request );

	if ( request.status_enu == EEL_ERR_NO_INSTANCE ) {
		Status	= READ_NODATA;
		goto exit_Read;

	} else if ( request.status_enu != EEL_OK ) {
		goto exit_Read;

	}
	Status	= READ_OK;


exit_Read:
	EEL_Close();
	return Status;
}

/******************************************************************************
* Function Name : Host_SaveVariables
* Description : Write data to use EEPROM emulation.
* Argument : DataNumber
* Return Value : WRITE_OK / STATUS_NG
******************************************************************************/
uint8_t Host_SaveVariables( uint8_t DataNumber, void * addr )
{
	/* writing permission status */
	uint8_t Status = STATUS_NG;
	eel_request_t	request;

	EEL_Open();

	request.address_pu08	= 0x00;
	request.identifier_u08	= 0x00;
	request.timeout_u08		= 0xFF;
	request.command_enu		= EEL_CMD_STARTUP;
	request.status_enu		= EEL_BUSY;

	EEL_Execute( (eel_request_t*)&request );

	if ( request.status_enu != EEL_OK ) {
		goto exit_Write;
	}

	request.address_pu08	= (eel_u08 *)addr;
	request.identifier_u08	= DataNumber;
	request.timeout_u08		= 0xFF;
	request.command_enu		= EEL_CMD_WRITE;
	request.status_enu		= EEL_BUSY;

	EEL_Execute( (eel_request_t*)&request );

	if ( request.status_enu != EEL_OK ) {
		goto exit_Write;
	}

	Status	= WRITE_OK;


exit_Write:
	EEL_Close();

	return Status;
}
