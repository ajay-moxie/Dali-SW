/*******************************************************************************
* File Name : r_user.h
* Device(s) : 
* Description : This file implements HCD common.
*               This file is updated by Applilet for HCD application.
******************************************************************************/

#ifndef	_MDUSER_
#define _MDUSER_

/******************************************************************************
Macro definitions
******************************************************************************/
/* clock generator type definition */
#define	CLOCK_TYPE_INTERNAL			0		/* Internal high-speed oscillator */
#define	CLOCK_TYPE_EXTOSC			1		/* External clock input */
#define	CLOCK_TYPE_EXTCRYSTAL		2		/* X1 oscillation by Crystal/ceramic resonator */

#define CLOCK_TYPE					CLOCK_TYPE_INTERNAL
#define CLOCK_FREQ					32

#define NUMBER_OF_CHANNEL			3

#endif
