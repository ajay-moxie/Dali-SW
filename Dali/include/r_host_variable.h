/*******************************************************************************
* File Name : r_dali_variable.h
* Description : This file implements self programming.
******************************************************************************/

#ifndef __SELFPROGRAMMING__
#define __SELFPROGRAMMING__

/******************************************************************************
Macro definitions
******************************************************************************/
#define STATUS_NG				0xFF			/* init value of Status */

#define	INIT_ENTRYRAM_OK		0x00
#define	INIT_OK					0x00

#define	WRITE_OK				0x00

#define	READ_OK					0x00
#define READ_NODATA				0xFF

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
uint8_t Host_InitEmulation( void );
uint8_t Host_ReadVariables( uint8_t DataNumber, void *addr );
uint8_t Host_SaveVariables( uint8_t DataNumber, void *addr );


#endif
