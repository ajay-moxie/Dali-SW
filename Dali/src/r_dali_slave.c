#include "r_macrodriver.h"
#include "r_dali_slave.h"
#include "r_host_variable.h"
#define DALI_SLAVE_DATA_EXIST 0x5aba23
#define SLAVE_DATA_AREA 1

static struct slave{
	uint32_t data_exist;
	struct dali_slave dali_slaves[TOTAL_SLAVES];
}slaves;

/******************************************************************************
 * Function Name : DALI_slave_initialize
 * Description : DALI initialize slave data base.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
void DALI_slave_initialize()
{
	uint32_t i;
	for(i = 0; i < TOTAL_SLAVES; i++)
	{
		slaves.dali_slaves[i].address = 0xFF;
		slaves.dali_slaves[i].search_h = 0xFF;
		slaves.dali_slaves[i].search_m = 0xFF;
		slaves.dali_slaves[i].search_l = 0xFF;

	}
}

/******************************************************************************
 * Function Name : DALI_get_slave_count
 * Description : DALI get count of total slaves.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
uint8_t DALI_get_slave_count()
{
	uint8_t i = 0;
	uint8_t count = 0;
	while(i < TOTAL_SLAVES){
		if(slaves.dali_slaves[i].address != 0xFF)
			count++;
		i++;
	}
	return count; //no address available
}
/******************************************************************************
 * Function Name : DALI_get_new_address
 * Description : DALI get new available address.
 * Argument : address
 * Return Value : none
 ******************************************************************************/
uint8_t DALI_get_new_slave_address()
{
	uint8_t i = 0;
	while(i < TOTAL_SLAVES){
		if(slaves.dali_slaves[i].address == 0xFF)
			return i;
		i++;
	}
	return 0xFF; //no address available
}

/******************************************************************************
 * Function Name : DALI_set_slave_config
 * Description : DALI set slave config.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
void DALI_set_slave_config(struct dali_slave slave)
{
	uint8_t i = slave.address;
	slaves.dali_slaves[i].address = slave.address;
	slaves.dali_slaves[i].search_h = slave.search_h;
	slaves.dali_slaves[i].search_m = slave.search_m;
	slaves.dali_slaves[i].search_l = slave.search_l;
}

/******************************************************************************
 * Function Name : DALI_set_address
 * Description : DALI set slave address.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
void DALI_set_address(uint8_t address, uint32_t offset)
{
	slaves.dali_slaves[offset].address = address;
}


/******************************************************************************
 * Function Name : DALI_free_slave_address
 * Description : DALI free address from list.
 * Argument : address
 * Return Value : none
 ******************************************************************************/
void DALI_free_slave_address(uint8_t address)
{
	uint8_t i = 0;
	while(i < TOTAL_SLAVES){
		if(slaves.dali_slaves[i].address == address)
			slaves.dali_slaves[i].address = 0xFF;
		i++;
	}
}

uint8_t DALI_save_slave_data()
{
	slaves.data_exist = DALI_SLAVE_DATA_EXIST;
	return Host_SaveVariables(SLAVE_DATA_AREA, (void *)&slaves);
}

uint8_t DALI_read_saved_slave_data()
{
	return Host_ReadVariables(SLAVE_DATA_AREA, (void *)&slaves);
}

uint8_t DALI_slave_data_exist()
{
	return (slaves.data_exist == DALI_SLAVE_DATA_EXIST)? 1 : 0;
}
