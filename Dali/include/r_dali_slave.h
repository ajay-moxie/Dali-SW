#define TOTAL_SLAVES 64
struct dali_slave{
	uint8_t address;
	uint8_t search_h;
	uint8_t search_m;
	uint8_t search_l;
};

void DALI_slave_initialize();
uint8_t DALI_get_new_slave_address();
void DALI_set_slave_config(struct dali_slave slave);
void DALI_set_address(uint8_t address, uint32_t offset);
uint8_t DALI_get_slave_count();
