#include "r_macrodriver.h"
#include "r_dali_slave.h"
static struct dali_slave dali_slaves[TOTAL_SLAVES];

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
		dali_slaves[i].address = 0xFF;
		dali_slaves[i].search_h = 0xFF;
		dali_slaves[i].search_m = 0xFF;
		dali_slaves[i].search_l = 0xFF;

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
		if(dali_slaves[i].address != 0xFF)
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
		if(dali_slaves[i].address == 0xFF)
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
	dali_slaves[i].address = slave.address;
	dali_slaves[i].search_h = slave.search_h;
	dali_slaves[i].search_m = slave.search_m;
	dali_slaves[i].search_l = slave.search_l;
}

/******************************************************************************
 * Function Name : DALI_set_address
 * Description : DALI set slave address.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
void DALI_set_address(uint8_t address, uint32_t offset)
{
	dali_slaves[offset].address = address;
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
		if(dali_slaves[i].address == address)
			dali_slaves[i].address = 0xFF;
		i++;
	}
}
