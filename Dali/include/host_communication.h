#ifndef __HOST_COMM__
#define __HOST_COMM__

typedef uint8_t (*fn)(uint8_t *buff, uint8_t size);
typedef void (*fn1)(uint16_t buff);
typedef uint8_t (*fn2)(uint8_t *buff);
typedef uint8_t (*usp_tx)(uint8_t *buff, uint8_t size);
typedef uint8_t (*usp_rx)(uint8_t *buff, uint8_t size);
typedef void (*dwn_tx)(uint16_t command);
typedef uint8_t (*dwn_rx)( uint8_t *data );
typedef uint32_t (*dwn_response)( uint16_t data );
typedef void (*rx_ext)(void);
typedef void (*dwn_register_rx_handler)(rx_ext rx_ext1);
typedef struct {
	usp_tx usp_tx;
	usp_rx usp_rx;
	dwn_tx dwn_tx;
	dwn_rx dwn_rx;
	dwn_response dwn_response;
	dwn_register_rx_handler dwn_register_rx_handler;
	dwn_register_rx_handler dwn_unregister_rx_handler;

}t_host_comm;

void host_RegisterTxRx(t_host_comm host_comm);
uint8_t host_AnalyzeCommand(uint8_t *buff);
#endif
