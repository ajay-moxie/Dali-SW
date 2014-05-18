#include "r_macrodriver.h"
#include "host_communication.h"
#include "host_commands.h"
#include "r_dali_enumerate.h"
#include "r_dali_config.h"
#include "r_dali_slave.h"

/*uint8_t (*usp_tx)(uint8_t *buff, uint8_t size);
uint8_t (*usp_rx)(uint8_t *buff, uint8_t size);
uint8_t (*dwn_tx)(uint16_t command);
uint8_t (*dwn_rx)( uint8_t *data );*/
t_host_comm host_comm;

/******************************************************************************
 * Function Name : host_RegisterTxRx
 * Description : Register function to send and receive.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
void host_RegisterTxRx(t_host_comm host)
{
	host_comm.usp_tx = host.usp_tx;
	host_comm.usp_rx = host.usp_rx;
	host_comm.dwn_tx = host.dwn_tx;
	host_comm.dwn_rx = host.dwn_rx;
	host_comm.dwn_response = host.dwn_response;
	host_comm.dwn_register_rx_handler = host.dwn_register_rx_handler;
	host_comm.dwn_unregister_rx_handler = host.dwn_unregister_rx_handler;
}

/******************************************************************************
 * Function Name : host_ProcessMasterCommand
 * Description : Function to read data from circular buffer.
 * Argument : none
 * Return Value : none
 ******************************************************************************/

static void host_ProcessMasterCommand(uint8_t *buff)
{
	uint8_t cmd;
	static uint8_t host_response[3];
	uint16_t config_cmd;
	uint8_t temp;
	
	cmd = buff[0];
	switch(cmd){
		case ENUMERATE:
		DALI_Enumerate(buff[1]);
		break;
		case MASTER_CONFIGURATION_COMMAND:
		config_cmd = buff[1];
		config_cmd = (config_cmd << 8) | buff[2];
		DALI_Config(config_cmd);
		break;
		case DEVICE_COUNT:
		temp = DALI_get_new_slave_address();
		RESPONSE_BYTE(host_response,DALI_get_new_slave_address());
		host_comm.usp_tx(host_response, HOST_BACKWARD_FRAME_SIZE);
		default:
		break;
	}
	
}
/******************************************************************************
 * Function Name : host_process_slave_command
 * Description : All slave commands need to be send to downstream device
 * Argument : none
 * Return Value : none
 ******************************************************************************/
static void host_ProcessSlaveCommand(uint8_t *buff)
{
	uint16_t command;
	uint8_t dwn_data;
	command = buff[0];
	command = (command << 8) | buff[1];
	host_comm.dwn_tx(command);
	/*while(!host_comm.dwn_rx(&dwn_data))
		;

	host_comm.usp_tx(&dwn_data, 2);*/
}

void host_dwx_rx_handler()
{
	uint8_t response;
	static uint8_t host_response[3];
	host_comm.dwn_unregister_rx_handler(host_dwx_rx_handler);
	host_comm.dwn_rx(&response);
	RESPONSE_BYTE(host_response, response);
	host_comm.usp_tx(host_response, HOST_BACKWARD_FRAME_SIZE);
}
/******************************************************************************
 * Function Name : host_analyze_commdiwand
 * Description : analyze command for slave or master command.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
uint8_t host_AnalyzeCommand(uint8_t *buff)
{
	uint8_t ret = 0;
	uint16_t usp_cmd;
	usp_cmd = (buff[1] << 8) | (buff[2]);
	if(host_comm.dwn_response(usp_cmd)){
		host_comm.dwn_register_rx_handler(host_dwx_rx_handler);
		
	}
	
	if(buff[0] == SLAVE_COMMAND){
		host_ProcessSlaveCommand(buff+1);
	}
	else if(buff[0] == MASTER_COMMAND){
		host_ProcessMasterCommand(buff+1);
	}else
		ret = 1;
	return ret;
}

