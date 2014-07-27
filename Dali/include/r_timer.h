/*******************************************************************************
* File Name : r_timer.h
* Description : This file implements LVI.
******************************************************************************/

#ifndef __TIMER__
#define __TIMER__

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
void Timer_StartInterval_1ms( void (*proc)(void) );
void Timer_Init( );

#endif
