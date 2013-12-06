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
extern DALI_VARS_T	dali_variable[NUMBER_OF_CHANNEL];		/* configuration parameters */

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
uint8_t DALI_InitEmulation( void );
uint8_t DALI_ReadVariables( uint8_t DataNumber );
uint8_t DALI_SaveVariables( uint8_t DataNumber );


#endif
