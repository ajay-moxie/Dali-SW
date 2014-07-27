#include "r_macrodriver.h"
#include "r_user.h"
#include "host_communication.h"
#include "r_master_state_machine.h"

static master_state_t master_state = NORMAL;
extern t_host_comm host_comm;

/******************************************************************************
 * Function Name : master_state_machine_init
 * Description : Host State Machine Init.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
void master_state_machine_init()
{
	master_state = NORMAL;
}

/******************************************************************************
 * Function Name : get_master_state
 * Description : Get Master State.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
master_state_t get_master_state()
{
	return master_state;
}

/******************************************************************************
 * Function Name : set_master_state
 * Description : Set Master State.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
void set_master_state(master_state_t state)
{
	master_state = state;
}
