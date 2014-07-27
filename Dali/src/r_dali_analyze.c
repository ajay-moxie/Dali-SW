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
 * File Name : DALI_analyze.c
 * Device(s) : 
 * Description : This file implement DALI protocol. 
 ******************************************************************************/

#pragma	SFR

/******************************************************************************
  Includes <System Includes> , �gProject Includes�h
 ******************************************************************************/
#include "r_macrodriver.h"
#include "r_user.h"
#include "r_dali.h"
#include "r_dali_analyze.h"

/******************************************************************************
 * Function Name : DALI_AnalyzeResponse
 * Description : Analyze the DALI response.
 * Argument : dalicommand
 * Return Value : none
 ******************************************************************************/
uint8_t DALI_AnalyzeResponse(uint8_t response, uint8_t type)
{
	uint8_t ret = NO;
	switch( type ){
		case SPECIAL_COMMAND:
			if(response == YES)
				ret = YES;
			break;
		default:
			break;
	}
	return ret;
}

