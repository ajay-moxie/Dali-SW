/*******************************************************************************
* File Name : r_macrodriver.h
* Description : This file is for common definition.
******************************************************************************/

#ifndef	_MDSTATUS_
#define _MDSTATUS_

/******************************************************************************
Includes <System Includes> , ÅgProject IncludesÅh
******************************************************************************/
#ifdef __IAR_SYSTEMS_ICC__
#include <ior5f107de.h>
#include <ior5f107de_ext.h>
#include <intrinsics.h>
#endif

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed long int32_t;
typedef unsigned long uint32_t;
#ifndef __IAR_SYSTEMS_ICC__
typedef unsigned int _Bool;
#endif

/******************************************************************************
Macro definitions
******************************************************************************/

/* definition of logical value */
#define TRUE				1
#define FALSE				0

/* definition of bit setting value */
#define	BIT_SET				1
#define	BIT_CLR				0

/* definition of port I/O value */
#define	LEVEL_LOW			0
#define	LEVEL_HIGH			1

/* definition of port mode setting value */
#define	INPUT				1
#define	OUTPUT				0

/* definition of interrupt mask setting value */
#define	INTERRUPT_UNMASKED	0
#define	INTERRUPT_MASKED	1

/* definition of flag variables setting value */
//#define	FLAG_DOWN			0
//#define	FLAG_UP				1

/* definition of priority setting value */
#define	PRIORITY_HIGH			0
#define PRIORITY_LOW			1


#endif
