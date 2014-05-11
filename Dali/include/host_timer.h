/*******************************************************************************
* File Name : host_timer.h
* Device(s) : 
* Description : This file implements host.
******************************************************************************/

#ifndef __host_TIMER__
#define __host_TIMER__

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
#define FORWARD_FRAME_SIZE		16 //16ms
enum timermode { TIMER_WAIT_ANSWER,
				 TIMER_PROHIBIT_RECEPTION,
				 TIMER_CMD,
				 TIMER_ADDRESSING_PERIOD,
				 TIMER_DAPC_INTERVAL,
				 TIMER_ACTUAL_SAVING,
			   };

enum timermode1 {
	MS_4 = 25,
	MS_8 = 8,
	MS_10 = 30,
	MS_50 = 50,
	MS_100 = 100,
	MIN_15 = 15000,
};

typedef void (*fn_t)(void);
struct host_timer {
	uint8_t timecount_4ms;
	uint8_t timecount_8ms;
	uint8_t timecount_10ms;
	uint16_t timecount_50ms;
	uint16_t timecount_100ms;
	uint32_t timecount_15min;
	uint32_t timecount_totaltime;
	fn_t func_timecount_4ms;
	fn_t func_timecount_8ms;
	fn_t func_timecount_10ms;
	fn_t func_timecount_50ms;
	fn_t func_timecount_100ms;
	fn_t func_timecount_15min;
};

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
void host_InitTimer( void );
void host_RegisterTimer(uint16_t type, fn_t func);
void host_Interval( void );
void host_Counter( void );
void host_StartTimer( uint16_t type );
void host_StopTimer( uint16_t type );
uint8_t host_IsTimerRunning( uint16_t type );


#endif
