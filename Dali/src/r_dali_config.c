#include "r_macrodriver.h"
#include "r_dali_config.h"
#include "r_user.h"
#include "r_dali.h"
#include "r_dali_analyze.h"
#include "r_dali_timer.h"
#include "host_communication.h"
#include "host_commands.h"
static uint8_t dali_resend_command = 0;
static uint8_t response;
static uint16_t command;
extern t_host_comm host_comm;
static void DALI_4ms_config_timeout();
static void DALI_50ms_config_timeout();
static void DALI_100ms_config_timeout();

/******************************************************************************
 * Function Name : DALI_Config
 * Description : DALI Config.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
void DALI_Config(uint16_t cmd)
{
	command = cmd;
	DALI_ConfigInitTimer();
	DALI_RegisterConfigTimer(MS_4, DALI_4ms_config_timeout);
	DALI_RegisterConfigTimer(MS_50, DALI_50ms_config_timeout);
	DALI_RegisterConfigTimer(MS_100, DALI_100ms_config_timeout);
	DALI_ConfigStartTimer(MS_4);
}

/******************************************************************************
 * Function Name : DALI_4ms_config_timeout
 * Description : Actions when 4ms timer timeout
 * Argument : none
 * Return Value : none
 ******************************************************************************/
static void DALI_4ms_config_timeout()
{
	DALI_SendCommand(command);
	dali_resend_command = 1;
	DALI_ConfigStartTimer(MS_50);
}



/******************************************************************************
 * Function Name : DALI_50ms_config_timeout
 * Description : Actions when 50ms timer timeout
 * Argument : none
 * Return Value : none
 ******************************************************************************/
static void DALI_50ms_config_timeout()
{
	if(dali_resend_command){
		DALI_SendCommand(command);
		dali_resend_command = 0;
		DALI_ConfigStartTimer(MS_100);
	}
}

/******************************************************************************
 * Function Name : DALI_100ms_config_timeout
 * Description : Actions when 100ms timer timeout
 * Argument : none
 * Return Value : none
 ******************************************************************************/
static void DALI_100ms_config_timeout()
{

	static uint8_t host_response[3];
	RESPONSE_ACK(host_response);
	host_comm.usp_tx(host_response, HOST_BACKWARD_FRAME_SIZE);
}
