#ifndef __MASTER_STATE_MACHINE__H
#define __MASTER_STATE_MACHINE__H
typedef enum master_states { 
	NORMAL,
	ENUMERATION,
	COLLISSION,
	SLAVE_RESPONSE_AWAITED,
	MASTER_CONFIG,
	PASSWD_CHANGE,
}master_state_t;


void master_state_machine_init();
void set_master_state(master_state_t state);
master_state_t get_master_state();

#endif
