#include "r_macrodriver.h"
#include "r_user.h"
#include "r_dali.h"
#include "r_dali_timer.h"
#include "r_dali_analyze.h"
#include "r_dali_name_device.h"
#include "host_communication.h"
#include "host_commands.h"
static enum device_name_states state;
static enum name_substates sub_state;
static uint8_t address;
static uint8_t enumeration_required = 1;
static uint8_t dali_resend_command = 0;
static uint8_t response;
extern t_host_comm host_comm;
static void DALI_4ms_timeout();
static void DALI_10ms_timeout();
static int name_change = 0;
static uint8_t *new_name;
static name_type_t name_type;

/******************************************************************************
 * Function Name : DALI_NameDevice
 * Description : DALI Name/Rename Slave Device.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
void DALI_NameDevice(uint8_t *name, name_type_t type)
{
	name_change = 1;
	address = name[0];
	new_name = name + 1;
	name_type = type;
	DALI_NameDeviceInitTimer();
	DALI_RegisterDeviceNameTimer(MS_4, DALI_4ms_timeout);
	DALI_RegisterDeviceNameTimer(MS_10, DALI_10ms_timeout);
	DALI_NameDeviceStartTimer(MS_4);
	state = NAME0;
}

/******************************************************************************
 * Function Name : DALI_search_address
 * Description : DALI guess search address. Need to find search address. 
 * Argument : none
 * Return Value : none
 ******************************************************************************/
static uint8_t DALI_search_address(uint8_t response, uint8_t *guess )
{
	static uint8_t high;
	static uint8_t last;
	static uint8_t low;
	uint8_t ret = NO;
	if(response == YES){
		if((high - last == 1) || (last - low == 1))
		{
			ret = YES;
			*guess = last;
			high = 0xFF;
			last = 0xFF;
			low = 0x00;
			return ret;
		}
		*guess = (uint8_t)(((uint16_t)last + (uint16_t)low) >> 1);
		high = last;
		last = *guess;
	}
	else if(response == NO){
		if(high - last == 1){
			*guess = high;
		}
		else
			*guess = (uint8_t)(((uint16_t)high + (uint16_t)last) >> 1);
		low = last;
		last = *guess;
	}
	else if(response == NA){/*first time, send 0xFF*/
		high = 0xFF;
		*guess = last = 0xFF;
		low = 0x0;
	}
	else{
	}
	return ret;
}


/******************************************************************************
 * Function Name : DALI_4ms_timeout
 * Description : Actions when 4ms timer timeout
 * Argument : none
 * Return Value : none
 ******************************************************************************/
static void DALI_4ms_timeout()
{
	uint8_t guess_address;
	static uint8_t host_response[3];
	switch(state){
		case NAME0:
		case NAME1:
		case NAME2:
		case NAME3:
		case NAME4:
		case NAME5:
		case NAME6:
		case NAME7:
		switch(sub_state){
			case SUB_NAME_OFFSET:
			DALI_SendCommand((EXCOMMAND_DATA_TRANSFER_REGISTER_1<< 8) | state);
			sub_state = SUB_NAME_BYTE;
			DALI_NameDeviceStartTimer(MS_4);
			break;
			case SUB_NAME_BYTE:
			DALI_SendCommand((EXCOMMAND_DTR<< 8) | new_name[state]);
			sub_state = SUB_NAME_ADDRESS;
			DALI_NameDeviceStartTimer(MS_4);
			break;
			case SUB_NAME_ADDRESS:
			if(name_type == SLAVE_NAME)
				DALI_SendCommand((EXCOMMAND_WRITE_NAME << 8) | address);
			else if(name_type == ROOM_NAME)
				DALI_SendCommand((EXCOMMAND_WRITE_ROOM << 8) | address);
			sub_state = SUB_NAME_OFFSET;
			if(state != NAME7){
				DALI_NameDeviceStartTimer(MS_4);
				state = state + 1;
			}
			else{
				//response to host
				DALI_NameDeviceStartTimer(MS_10);
			}
			break;
			default:
			break;
		}
		default:
		break;
	}
}


/******************************************************************************
 * Function Name : DALI_10ms_timeout
 * Description : Actions when 10ms timer timeout. By this time all responses
 * from slaves should be received
 * Argument : none
 * Return Value : none
 ******************************************************************************/
static void DALI_10ms_timeout()
{
	static uint8_t response[3];
	response[0] = NO;
	switch(state){
		case NAME7:
		DALI_ReadData(response);
		if(response[0] == YES){
			RESPONSE_ACK(response);
			host_comm.usp_tx(response, HOST_BACKWARD_FRAME_SIZE);
		}
		break;
		default:
		break;
	}
}


