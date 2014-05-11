#ifndef __HOST_COMM__
#define __HOST_COMM__

typedef uint8_t (*fn)(uint8_t *buff, uint8_t size);
typedef void (*fn1)(uint16_t buff);
typedef uint8_t (*fn2)(uint8_t *buff);
typedef uint8_t (*usp_tx)(uint8_t *buff, uint8_t size);
typedef uint8_t (*usp_rx)(uint8_t *buff, uint8_t size);
typedef void (*dwn_tx)(uint16_t command);
typedef uint8_t (*dwn_rx)( uint8_t *data );

typedef struct {
	usp_tx usp_tx;
	usp_rx usp_rx;
	dwn_tx dwn_tx;
	dwn_rx dwn_rx;
}t_host_comm;

void host_RegisterTxRx(fn upstrm_tx, fn upstrm_rx, fn1 down_tx, fn2 down_rx);
uint8_t host_AnalyzeCommand(uint8_t *buff);
#endif