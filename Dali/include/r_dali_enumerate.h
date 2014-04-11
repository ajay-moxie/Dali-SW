enum enumeration_states { 
	INITIALIZE,
	RANDOMIZE,
	SEARCH_H,
	SEARCH_M,
	SEARCH_L,
	ADDRESS,
	VERIFY,
	WITHDRAW,
	TERMINATE,
};

enum search_substates { 
	SUB_SEARCH_H,
	SUB_SEARCH_M,
	SUB_SEARCH_L,
	SUB_COMPARE,
	SUB_RESPONSE,
};


void DALI_Enumerate(uint8_t address);	
