enum device_name_states { 
	NAME0,
	NAME1,
	NAME2,
	NAME3,
	NAME4,
	NAME5,
	NAME6,
	NAME7,
	NAME_RESPONSE,
};

enum name_substates { 
	SUB_NAME_OFFSET,
	SUB_NAME_BYTE,
	SUB_NAME_ADDRESS,
	
};

void DALI_NameDevice(uint8_t name[8]);
//DALI_NameDeviceInitTimer();
