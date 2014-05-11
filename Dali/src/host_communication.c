#include "r_macrodriver.h"
#include "host_communication.h"
#include "host_commands.h"
#include "r_dali_enumerate.h"
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
void host_RegisterTxRx(fn upstrm_tx, fn upstrm_rx, fn1 down_tx, fn2 down_rx)
{
	host_comm.usp_tx = upstrm_tx;
	host_comm.usp_rx = upstrm_rx;
	host_comm.dwn_tx = down_tx;
	host_comm.dwn_rx = down_rx;
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
	uint8_t temp;
	cmd = buff[0];
	switch(cmd){
		case ENUMERATE:
		DALI_Enumerate(buff[1]);
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
/******************************************************************************
 * Function Name : host_analyze_commdiwand
 * Description : analyze command for slave or master command.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
uint8_t host_AnalyzeCommand(uint8_t *buff)
{
	uint8_t ret = 0;
	
	if(buff[0] == SLAVE_COMMAND){
		host_ProcessSlaveCommand(buff+1);
	}
	else if(buff[0] == MASTER_COMMAND){
		host_ProcessMasterCommand(buff+1);
	}else
		ret = 1;
	return ret;
}

/******************************************************************************
 * Function Name : UART1_RxDataCount
 * Description : Function to read data from circular buffer.
 * Argument : none
 * Return Value : none
 ******************************************************************************/


/******************************************************************************
 * Function Name : UART1_RxDataCount
 * Description : Function to read data from circular buffer.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
