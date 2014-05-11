/*******************************************************************************
* File Name : host_timer.c
* Device(s) : 
* Description : This file implement host protocol. 
******************************************************************************/
#pragma	SFR
#pragma DI
#pragma EI

/******************************************************************************
Includes <System Includes> , ÅgProject IncludesÅh
******************************************************************************/
#include "r_macrodriver.h"
#include "r_user.h"
#include "r_timer.h"
#include "host_timer.h"

static uint32_t timecount_addressing[NUMBER_OF_CHANNEL];
static uint8_t timecount_rcv_command[NUMBER_OF_CHANNEL];
static uint8_t timecount_answer;
static uint8_t timecount_dapc_sequence[NUMBER_OF_CHANNEL];
static uint8_t timecount_actual_unchange[NUMBER_OF_CHANNEL];
static uint8_t timecount_interface_failure;
static uint8_t timecount_prohibit_reception;
static uint8_t timecount_10ms;
static uint32_t timecount_totaltime;
static struct host_timer host_timeout;

/******************************************************************************
* Function Name : host_InitTimer
* Description : Initialize the interval timer.
* Argument : none
* Return Value : none
******************************************************************************/
void host_InitTimer( void )
{
	uint8_t i;

	/* initialize variable. */
	timecount_totaltime = 0;
	timecount_answer = 0;
	timecount_interface_failure = TIME_INTERFACE_FAILURE;
	timecount_10ms = 10;

	host_timeout.timecount_4ms = 0;
	host_timeout.timecount_8ms = 0;
	host_timeout.timecount_10ms = 0;
	host_timeout.timecount_50ms = 0;
	host_timeout.timecount_100ms = 0;
	host_timeout.timecount_15min = 0;
	host_timeout.timecount_totaltime = 0;

	host_timeout.func_timecount_4ms = (fn_t)0;
	host_timeout.func_timecount_8ms = (fn_t)0;
	host_timeout.func_timecount_10ms = (fn_t)0;
	host_timeout.func_timecount_50ms = (fn_t)0;
	host_timeout.func_timecount_100ms = (fn_t)0;
	host_timeout.func_timecount_15min = (fn_t)0;
	Timer_StartInterval_1ms(host_Counter);
}


/******************************************************************************
* Function Name : host_counter
* Description : Interval timer process.
* Argument : none
* Return Value : none
******************************************************************************/
void host_Counter( void )
{
	host_timeout.timecount_totaltime++;
	if(host_timeout.timecount_4ms){
		host_timeout.timecount_4ms--;
		if((host_timeout.func_timecount_4ms) && (host_timeout.timecount_4ms == 0))
			host_timeout.func_timecount_4ms();
	}
	if(host_timeout.timecount_8ms){
		host_timeout.timecount_8ms--;
		if((host_timeout.func_timecount_8ms) && (host_timeout.timecount_8ms == 0))
			host_timeout.func_timecount_8ms();
	}
	if(host_timeout.timecount_10ms){
		host_timeout.timecount_10ms--;
		if((host_timeout.func_timecount_10ms) && (host_timeout.timecount_10ms == 0))
			host_timeout.func_timecount_10ms();
	}
	if(host_timeout.timecount_50ms){
		host_timeout.timecount_50ms--;
		if((host_timeout.func_timecount_50ms) && (host_timeout.timecount_50ms == 0))
			host_timeout.func_timecount_50ms();
	}
	if(host_timeout.timecount_100ms){
		host_timeout.timecount_100ms--;
		if((host_timeout.func_timecount_100ms) && (host_timeout.timecount_100ms == 0))
			host_timeout.func_timecount_100ms();
	}
	if(host_timeout.timecount_15min){
		host_timeout.timecount_15min--;
		if((host_timeout.func_timecount_15min) && (host_timeout.timecount_15min == 0))
			host_timeout.func_timecount_15min();
	}

}

/******************************************************************************
* Function Name : host_StartTimer
* Description : Start timer count.
* Argument : type
* Return Value : none
******************************************************************************/
void host_StartTimer( uint16_t type )
{
	DI();

	switch ( type ){
		case MS_4:
			host_timeout.timecount_4ms = MS_4 + FORWARD_FRAME_SIZE;
			break;
		case MS_8:
			host_timeout.timecount_8ms = MS_8 + FORWARD_FRAME_SIZE;
			break;
		case MS_10:
			host_timeout.timecount_10ms = MS_10 + FORWARD_FRAME_SIZE;
			break;
		case MS_50:
			host_timeout.timecount_50ms = MS_50 + FORWARD_FRAME_SIZE;
			break;
		case MS_100:
			host_timeout.timecount_100ms = MS_100 + FORWARD_FRAME_SIZE;
			break;
		case MIN_15:
			host_timeout.timecount_15min = MIN_15 + FORWARD_FRAME_SIZE;
			break;
		default:
			break;
	}

	EI();
}


/******************************************************************************
* Function Name : host_StopTimer
* Description : Stop timer count.
* Argument : type
* Return Value : none
******************************************************************************/
void host_StopTimer( uint16_t type )
{
	DI();

	switch ( type ){
		case MS_4:
			host_timeout.timecount_4ms = 0;
			break;
		case MS_8:
			host_timeout.timecount_8ms = 0;
			break;
		case MS_10:
			host_timeout.timecount_10ms = 0;
			break;
		case MS_50:
			host_timeout.timecount_50ms = 0;
			break;
		case MS_100:
			host_timeout.timecount_100ms = 0;
			break;
		case MIN_15:
			host_timeout.timecount_15min = 0;
			break;
		default:
			break;
	}


	EI();
}

/******************************************************************************
* Function Name : host_IsTimerRunning
* Description : Check the timer count is running or not.
* Argument : type:timer mode
* Return Value : TRUE:running  FALSE:stopped
******************************************************************************/
uint8_t host_IsTimerRunning( uint16_t type )
{
	uint8_t ret = FALSE;

	switch ( type ){
		case MS_4:
			if(host_timeout.timecount_4ms)
				ret = TRUE;
			break;
		case MS_8:
			if(host_timeout.timecount_8ms)
				ret = TRUE;
			break;
		case MS_10:
			if(host_timeout.timecount_10ms)
				ret = TRUE;
			break;
		case MS_50:
			if(host_timeout.timecount_50ms)
				ret = TRUE;
			break;
		case MS_100:
			if(host_timeout.timecount_100ms)
				ret = TRUE;
			break;
		case MIN_15:
			if(host_timeout.timecount_15min)
				ret = TRUE;
			break;
		default:
			break;
	}
	return ret;
}


/******************************************************************************
* Function Name : host_RegisterTimer
* Description : RegisterTimer.
* Argument : size
* Return Value : value
******************************************************************************/
void host_RegisterTimer(uint16_t type, fn_t func)
{
	switch ( type ){
		case MS_4:
			host_timeout.func_timecount_4ms = func;
			break;
		case MS_8:
			host_timeout.func_timecount_8ms = func;
			break;
		case MS_10:
			host_timeout.func_timecount_10ms = func;
			break;
		case MS_50:
			host_timeout.func_timecount_50ms = func;
			break;
		case MS_100:
			host_timeout.func_timecount_100ms = func;
			break;
		case MIN_15:
			host_timeout.func_timecount_15min = func;
			break;
		default:
			break;
	}
}
