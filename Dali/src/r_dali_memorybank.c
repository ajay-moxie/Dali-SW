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
* File Name : r_dali_memorybank.c
* Device(s) : 
* Description : This file implement DALI protocol. 
******************************************************************************/

/******************************************************************************
Includes <System Includes> , ÅgProject IncludesÅh
******************************************************************************/
#include "r_macrodriver.h"
#include "r_user.h"
#include "r_dali.h"
#include "r_dali_command.h"

#define SIZE_OF_MEMORY_BANK0				16
#define SIZE_OF_MEMORY_BANK1				16
#define SIZE_OF_MEMORY_BANK					8
#define MAXIMUM_NUMBER_OF_MEMORY_BAMNKS	  	3

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
uint8_t enable_memory_bank	= FALSE;

/******************************************************************************
Private global variables and functions
******************************************************************************/
static void DALI_UpdateChecksum( uint16_t bank );

static uint8_t memory_bank0[SIZE_OF_MEMORY_BANK0] ={
	SIZE_OF_MEMORY_BANK0-1,	/* address of last accessible memory location */
	0x00,	/* checksum of memory bank 0 */
	0x00,	/* number of last accessible memory bank */
	0x00,	/* GTIN byte 0 (MSB) */
	0x00,	/* GTIN byte 1 */
	0x00,	/* GTIN byte 2 */
	0x00,	/* GTIN byte 3 */
	0x00,	/* GTIN byte 4 */
	0x00,	/* GTIN byte 5 */
	0x00,	/* control gear firmware version (major) */
	0x00,	/* control gear firmware version (minor) */
	0x00,	/* serial number byte 1 (MSB) */
	0x00,	/* serial number byte 2 */
	0x00,	/* serial number byte 3 */
	0x00,	/* serial number byte 4 */
	0xFF,	/* additional OEM information */
};

static uint8_t memory_bank1[SIZE_OF_MEMORY_BANK1] ={
	SIZE_OF_MEMORY_BANK1-1,	/* address of last accessible memory location */
	0x00,	/* checksum of memory bank 0 */
	0xFF,	/* memory bank lock byte (read-only if not 0x55) */
	0xFF,	/* OEM GTIN byte 0 (MSB) */
	0xFF,	/* OEM GTIN byte 1 */
	0xFF,	/* OEM GTIN byte 2 */
	0xFF,	/* OEM GTIN byte 3 */
	0xFF,	/* OEM GTIN byte 4 */
	0xFF,	/* OEM GTIN byte 5 */
	0xFF,	/* OEM serial number byte 1 (MSB) */
	0xFF,	/* OEM serial number byte 2 */
	0xFF,	/* OEM serial number byte 3 */
	0xFF,	/* OEM serial number byte 4 (LSB) */
	0xFF,	/* Subsystem (bit 4 to bit 7) Device number (bit 0 to bit 3)  */
	0xFF,	/* Lamp type number (lockable) */
	0xFF,	/* additional OEM information */
};

static uint8_t memory_bank2[SIZE_OF_MEMORY_BANK] ={
	SIZE_OF_MEMORY_BANK-1,	/* address of last accessible memory location */
	0x00,	/* checksum of memory bank 0 */
	0xFF,	/* memory bank lock byte (read-only if not 0x55) */
	0xFF,	/* for free use */
	0xFF,	/* for free use */
	0xFF,	/* for free use */
	0xFF,	/* for free use */
	0xFF,	/* for free use */
};

static const uint8_t* memory_map[MAXIMUM_NUMBER_OF_MEMORY_BAMNKS+1] ={
	memory_bank0,	/* address of memory_bank_0 */
	memory_bank1,	/* address of memory_bank_1 */
	memory_bank2,	/* address of memory_bank_2 */
	0,			/* no bank */
};

/******************************************************************************
* Function Name : DALI_init_memorybank
* Description : initialize the memory-bank.
* Argument : none
* Return Value : none
******************************************************************************/
void DALI_InitMemorybank( )
{
	uint16_t i;
	
	enable_memory_bank = FALSE;
	memory_bank0[2] = MAXIMUM_NUMBER_OF_MEMORY_BAMNKS-1;	/* number of last accessible memory bank */

	for (i=0;i<MAXIMUM_NUMBER_OF_MEMORY_BAMNKS;i++){
		if (memory_map[i]!=0){
			DALI_UpdateChecksum(i);
		}
	}


}

/******************************************************************************
* Function Name : DALI_write_memorybank
* Description : write data to memory-bank.
* Argument : uint8_t bank    : 0...(MAXIMUM_OF_MEMORY_BAMNKS - 1)
*            uint8_t address : 0...(SIZE_OF_MEMORY_BANK -1)
*            uint8_8 data
* Return Value : FALSE(0)    : error
*                TRUE(1)     : success
******************************************************************************/
uint8_t DALI_WriteMemorybank( uint8_t bank, uint8_t address, uint8_t data )
{
	uint8_t* selected_bank = 0;

	if (enable_memory_bank!=TRUE) return FALSE;			/* disabled */
	if (bank>=MAXIMUM_NUMBER_OF_MEMORY_BAMNKS) return FALSE;	/* out of maximum bank number */
	if (bank==0) return FALSE;							/* memory bank 0 is read-only */

	selected_bank = (uint8_t*)memory_map[bank];
	if (selected_bank==0) return FALSE;				/* no memory bank */
	if (address>selected_bank[0]) return FALSE;		/* out of memory bank area */

	if (address<2) return FALSE;					/* read-only */
	if (address!=2 && selected_bank[0x02]!=0x55) return FALSE;	/* read-only (memory bank is locked) */

	selected_bank[address] = data;
	if (address==selected_bank[0]) enable_memory_bank = FALSE;	/* disable, becaulse it could not increment address */

	DALI_UpdateChecksum( bank );

	return TRUE;
}

/******************************************************************************
* Function Name : DALI_read_memorybank
* Description : write data to memory-bank.
* Argument : uint8_t bank    : 0...(MAXIMUM_OF_MEMORY_BAMNKS - 1)
*            uint8_t address : 0...(SIZE_OF_MEMORY_BANK -1)
*            uint8_8 *data   : read data
* Return Value : FALSE(0)    : error
*                TRUE(1)     : success
******************************************************************************/
uint8_t DALI_ReadMemorybank( uint8_t bank, uint8_t address, uint8_t* data )
{
	uint8_t* selected_bank = 0;

	if (bank>=MAXIMUM_NUMBER_OF_MEMORY_BAMNKS) return FALSE;	/* out of maximum bank number */

	selected_bank = (uint8_t*)memory_map[bank];
	if (selected_bank==0) return FALSE;				/* no memory bank */
	if (address>selected_bank[0]) return FALSE;		/* out of memory bank area */

	*data = selected_bank[address];
	return TRUE;
}

/******************************************************************************
* Function Name : DALI_update_checksum
* Description : upcdate checksum.
* Argument : uint8_t bank  : 0...(MAXIMUM_OF_MEMORY_BAMNKS - 1)
* Return Value : none
******************************************************************************/
void DALI_UpdateChecksum( uint16_t bank )
{
	uint16_t i;
	uint8_t check_sum = 0;
	uint8_t* selected_bank = (uint8_t*)memory_map[bank];

	/* calcurate checksum */
	for (i=2;i<=selected_bank[0];i++){
		check_sum -= selected_bank[i];
	}
	selected_bank[1] = check_sum;
}

