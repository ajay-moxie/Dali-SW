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
* File Name : r_dali_user.c
* Device(s) : 
* Description : This file implement DALI protocol 
******************************************************************************/

/******************************************************************************
Includes <System Includes> , ÅgProject IncludesÅh
******************************************************************************/
#include "r_macrodriver.h"
#include "r_user.h"
#include "r_dali.h"
#include "config.h"

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

const uint8_t enable_channel[NUMBER_OF_CHANNEL] = {1,0,0,};
DALI_VARS_T	dali_variable[NUMBER_OF_CHANNEL] = {
			/* Channel 1 */
				   1,	/* version_number */
				   1,	/* physical_min_level */
				   6,	/* device_type (LED modules)*/
				 254,	/* power_on_level */
				 254,	/* system_failure_level */
				   1,	/* min_level */
				 254,	/* max_level */
				   7,	/* fade_rate */
				   0,	/* fade_time */
				 255,	/* short_address */
				 255,	/* random_address_h */
				 255,	/* random_address_m */
				 255,	/* random_address_l */
				   0,	/* group_0_7 */
				   0,	/* group_8_15 */
				 255,	/* scene 0 */
				 255,	/* scene 1 */
				 255,	/* scene 2 */
				 255,	/* scene 3 */
				 255,	/* scene 4 */
				 255,	/* scene 5 */
				 255,	/* scene 6 */
				 255,	/* scene 7 */
				 255,	/* scene 8 */
				 255,	/* scene 9 */
				 255,	/* scene 10 */
				 255,	/* scene 11 */
				 255,	/* scene 12 */
				 255,	/* scene 13 */
				 255,	/* scene 14 */
				 255,	/* scene 15 */
				   0,	/* actual level */
				   0,	/* reserved */
				   0,	/* reserved */
				   0,	/* reserved */
				   0,	/* reserved */
				   0,	/* for check data */
				   SLAVE_NAME,
				   ROOM_NAME,

			/* Channel 2 */
				   1,	/* version_number */
				   1,	/* physical_min_level */
				   6,	/* device_type (LED modules)*/
				 254,	/* power_on_level */
				 254,	/* system_failure_level */
				   1,	/* min_level */
				 254,	/* max_level */
				   7,	/* fade_rate */
				   0,	/* fade_time */
				 255,	/* short_address */
				 255,	/* random_address_h */
				 255,	/* random_address_m */
				 255,	/* random_address_l */
				   0,	/* group_0_7 */
				   0,	/* group_8_15 */
				 255,	/* scene 0 */
				 255,	/* scene 1 */
				 255,	/* scene 2 */
				 255,	/* scene 3 */
				 255,	/* scene 4 */
				 255,	/* scene 5 */
				 255,	/* scene 6 */
				 255,	/* scene 7 */
				 255,	/* scene 8 */
				 255,	/* scene 9 */
				 255,	/* scene 10 */
				 255,	/* scene 11 */
				 255,	/* scene 12 */
				 255,	/* scene 13 */
				 255,	/* scene 14 */
				 255,	/* scene 15 */
				   0,	/* actual level */
				   0,	/* reserved */
				   0,	/* reserved */
				   0,	/* reserved */
				   0,	/* reserved */
				   0,	/* for check data */
				   SLAVE_NAME,
				   ROOM_NAME,

			/* Channel 3 */
				   1,	/* version_number */
				   1,	/* physical_min_level */
				   6,	/* device_type (LED modules)*/
				 254,	/* power_on_level */
				 254,	/* system_failure_level */
				   1,	/* min_level */
				 254,	/* max_level */
				   7,	/* fade_rate */
				   0,	/* fade_time */
				 255,	/* short_address */
				 255,	/* random_address_h */
				 255,	/* random_address_m */
				 255,	/* random_address_l */
				   0,	/* group_0_7 */
				   0,	/* group_8_15 */
				 255,	/* scene 0 */
				 255,	/* scene 1 */
				 255,	/* scene 2 */
				 255,	/* scene 3 */
				 255,	/* scene 4 */
				 255,	/* scene 5 */
				 255,	/* scene 6 */
				 255,	/* scene 7 */
				 255,	/* scene 8 */
				 255,	/* scene 9 */
				 255,	/* scene 10 */
				 255,	/* scene 11 */
				 255,	/* scene 12 */
				 255,	/* scene 13 */
				 255,	/* scene 14 */
				 255,	/* scene 15 */
				   0,	/* actual level */
				   0,	/* reserved */
				   0,	/* reserved */
				   0,	/* reserved */
				   0,	/* reserved */
				   0,	/* for check data */
				   SLAVE_NAME,
				   ROOM_NAME,

};
