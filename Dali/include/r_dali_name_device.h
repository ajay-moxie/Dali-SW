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

typedef enum {
	SLAVE_NAME,
	ROOM_NAME,
		
}name_type_t;

void DALI_SetDeviceName(uint8_t name[8], name_type_t type);
//DALI_NameDeviceInitTimer();
