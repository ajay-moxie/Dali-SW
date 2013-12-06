/*******************************************************************************
* File Name : DALI_timer.h
* Device(s) : 
* Description : This file implements DALI.
******************************************************************************/

#ifndef __DALI_TIMER__
#define __DALI_TIMER__

/******************************************************************************
Macro definitions
******************************************************************************/
#define TIME_WAIT_ANSWER			4					/* x 1ms:Backword transmission interval */
#define TIME_PROHIBIT_RECEPTION		19		 			/* x 1ms:Address setting period */
#define TIME_RECEIVE_INTERVAL		10					/* x10ms:recieve interval */
#define TIME_SAVE_ACTUAL			50					/* x10ms:If the actual level doesn't change until this time, save variable. */
#define TIME_INTERFACE_FAILURE		50					/* x10ms:If disconnect interface until this time, set "SYSTEM FAILURE". */
#define TIME_DAPCSEQUENCE_CMD		20					/* x10ms:DAPC command interval */
#define TIME_ADDRESSING_PERIOD		90000				/* x10ms:Address setting period */

enum timermode { TIMER_WAIT_ANSWER,
				 TIMER_PROHIBIT_RECEPTION,
				 TIMER_CMD,
				 TIMER_ADDRESSING_PERIOD,
				 TIMER_DAPC_INTERVAL,
				 TIMER_ACTUAL_SAVING,
			   };

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
void DALI_InitTimer( void );
void DALI_Interval( void );
void DALI_StartTimer( uint8_t type );
void DALI_StopTimer( uint8_t type );
void DALI_StartFadeTimer( uint32_t fadestep ,uint16_t fadetime );
void DALI_StopFadeTimer( uint8_t channel  );
uint8_t DALI_IsTimerRunning( uint8_t type );
uint8_t DALI_IsFading( uint8_t channel );

uint16_t DALI_GetRandomValue( uint16_t size );

#endif
