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
* File Name : r_led_user.c
* Device(s) : 
* Description : This file implements the duty list for Convert Duty.
******************************************************************************/

/******************************************************************************
Includes <System Includes> , ÅgProject IncludesÅh
******************************************************************************/
#include "r_macrodriver.h"


/* This is data table for level conversion (Logarithmic mode) */
/* For convert [0-255(Linear)] to [0 - 255(Logarithmic)]      */
const uint8_t	dimming_table[256] = {
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 2, 2, 2, 2,
	 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	 2, 2, 2, 2, 2, 3, 3, 3, 3, 3,
	 3, 3, 3, 3, 3, 3, 3, 4, 4, 4,
	 4, 4, 4, 4, 4, 4, 4, 5, 5, 5,
	 5, 5, 5, 5, 6, 6, 6, 6, 6, 6,
	 7, 7, 7, 7, 7, 8, 8, 8, 8, 8,
	 9, 9, 9, 9, 10, 10, 10, 10, 11, 11,
	 11, 12, 12, 12, 13, 13, 13, 14, 14, 15,
	 15, 15, 16, 16, 17, 17, 18, 18, 19, 19,
	 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
	 26, 26, 27, 28, 29, 29, 30, 31, 32, 33,
	 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
	 44, 46, 47, 48, 50, 51, 52, 54, 55, 57,
	 58, 60, 62, 63, 65, 67, 69, 71, 73, 75,
	 77, 79, 81, 83, 86, 88, 90, 93, 95, 98,
	 101, 104, 106, 109, 112, 116, 118, 122, 125, 129,
	 132, 136, 140, 144, 148, 152, 156, 160, 165, 169,
	 174, 179, 184, 189, 194, 199, 205, 211, 216, 222,
	 229, 235, 241, 248, 255, 255,
	};

