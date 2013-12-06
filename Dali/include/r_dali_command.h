/*******************************************************************************
* File Name : DALI_command.h
* Device(s) : 
* Description : This file implements DALI.
******************************************************************************/

#ifndef __DALI_COMMAND__
#define __DALI_COMMAND__

/******************************************************************************
Macro definitions
******************************************************************************/

/* Reset values */
#define RESET_ACTUAL_LEVEL					254					/* Actual level */
#define RESET_POWERONLEVEL					254					/* Power on level */
#define RESET_SYSTEMFAILURELEVEL			254					/* Sysem failure level */
#define RESET_MAXLEVEL						254					/* Max level */
#define RESET_FADERATE						7					/* Fade rate */
#define RESET_FADETIME						0					/* Fade time */
#define RESET_SEARCHADDRESSH				0xFF				/* Search address(H) */
#define RESET_SEARCHADDRESSM				0xFF				/* Search address(M) */
#define RESET_SEARCHADDRESSL				0xFF				/* Search address(L) */
#define RESET_RANDOMADDRESSH				0xFF				/* Random address(H) */
#define RESET_RANDOMADDRESSM				0xFF				/* Random address(M) */
#define RESET_RANDOMADDRESSL				0xFF				/* Random address(L) */
#define RESET_GROUP_0_7						0x00				/* Group0-7 */
#define RESET_GROUP_8_15					0x00				/* Group8-15 */
#define RESET_SCENE							255					/* Scene XX */
#define RESET_STATUS_INFORMATION			0x24				/* All status information */

/* initial values */
#define DEFAULT_SHORTADDRESS				255					/* Short address */
#define DEFAULT_SEARCHADDRESSH				0xFF				/* Search address(H) */
#define DEFAULT_SEARCHADDRESSM				0xFF				/* Search address(M) */
#define DEFAULT_SEARCHADDRESSL				0xFF				/* Search address(L) */
#define DEFAULT_SCENE						0xFF				/* Scene XX */
#define DEFAULT_STATUS_INFORMATION			0x80				/* All status information */
#define DEFAULT_DTR							0x00				/* DTR */

#define RANGE_MAXLEVEL						254					/* The max value of useful range at Actual level */

/* It uses it by the query command. */
#define ANSWER_YES							0xFF

/* Direction of fade */
#define FADE_DOWN							0				/* Fade down */
#define FADE_UP								1				/* Fade up */

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern uint8_t enable_memory_bank;

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
void DALI_LightingCommand( uint8_t cmd_data );
void DALI_ConfigCommand( uint8_t cmd_data );
void DALI_QueryCommand( uint8_t cmd_data );
void DALI_SpecialCommand( uint8_t cmd_data,uint8_t cmd_address );
void DALI_SetArcPowerWithFade(uint8_t cmd_data);

/*  for Memory Bank */
void	DALI_InitMemorybank( void );
uint8_t DALI_WriteMemorybank( uint8_t bank, uint8_t address, uint8_t data );
uint8_t DALI_ReadMemorybank( uint8_t bank, uint8_t address, uint8_t *data );

#endif
