#pragma	SFR
#pragma NOP
#pragma DI
#pragma EI

#pragma interrupt	INTST1 UART1_TxHandler
#pragma interrupt	INTSR1 UART1_RxHandler
#pragma interrupt	INTSRE1 UART1_RxErrorHandler

#include "r_macrodriver.h"
#include "r_uart.h"

#define UART_TSF_MASK 0x40
#define UART_BFF_MASK 0x20
#define UART_FEF_MASK 0x4
#define UART_PEF_MASK 0x2
#define UART_OVF_MASK 0x1
#define UART_RX_BUFFER_SIZE 16
static uint8_t uart_rx_circular_buff[UART_RX_BUFFER_SIZE];
static uint8_t uart_rx_read_index = UART_RX_BUFFER_SIZE - 1;
static uint8_t uart_rx_write_index = UART_RX_BUFFER_SIZE - 1;
static uint8_t *uart_tx_buffer;
static uint8_t uart_tx_size = 0;

/******************************************************************************
 * Function Name : UART1_RxDataCount
 * Description : Function to read data from circular buffer.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
int8_t UART1_RxDataCount()
{
	uint8_t ret;
	DI();
	ret = uart_rx_write_index - uart_rx_read_index;
	EI();
	if(uart_rx_write_index < uart_rx_read_index)
		ret += UART_RX_BUFFER_SIZE;
	return ret;
}
/******************************************************************************
 * Function Name : UART1_ReadData
 * Description : Function to read data from circular buffer.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
uint8_t UART1_ReadData( uint8_t *buff, int size)
{
	if(UART1_RxDataCount() < size)
		return 0;
	while(size > 0){
		uart_rx_read_index = (uart_rx_read_index + 1) % UART_RX_BUFFER_SIZE;
		*buff = uart_rx_circular_buff[uart_rx_read_index];
		size--;
		if(size)
			buff++;
	}
	return 1;
}
__interrupt void UART1_RxHandler( void )
{
	uint16_t status;
	uint8_t received_data;
	status = SSR03; 
	received_data = (uint8_t)SDR03;
	if(status & (UART_FEF_MASK | UART_PEF_MASK | UART_OVF_MASK)){
		SIR03 = status;
	}
	else if(status & (UART_BFF_MASK)){/*Valid Data*/
		uart_rx_write_index = (uart_rx_write_index + 1) % UART_RX_BUFFER_SIZE;
		uart_rx_circular_buff[uart_rx_write_index] = (uint8_t)received_data;
	}
}

__interrupt void UART1_RxErrorHandler( void )
{
	uint16_t status;
	uint8_t received_data;
	status = SSR03; 
	received_data = (uint8_t)SDR03;
	if(status & (UART_FEF_MASK | UART_PEF_MASK | UART_OVF_MASK)){
		SIR03 = status;
	}
	else if(status & (UART_BFF_MASK)){/*Valid Data*/
		uart_rx_write_index = (uart_rx_write_index + 1) % UART_RX_BUFFER_SIZE;
		uart_rx_circular_buff[uart_rx_write_index] = (uint8_t)received_data;
	}
}
__interrupt void UART1_TxHandler( void )
{
	uint16_t status;
	uint8_t data;
	status = SSR02; 
	SIR02 = status;
	if(uart_tx_size > 0){
		uart_tx_buffer++;
		data = *uart_tx_buffer;
		uart_tx_size--;
		SDR02 =  data;
	}
}

/******************************************************************************
 * Function Name : UART1_start
 * Description : UART1 start channel.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
void UART1_start()
{
	STIF1 = 0U;    /* clear INTST1 interrupt flag */
	STMK1 = 0U;    /* enable INTST1 interrupt */
	SRIF1 = 0U;    /* clear INTSR1 interrupt flag */
	SRMK1 = 0U;    /* enable INTSR1 interrupt */
	SREIF1 = 0U;   /* clear INTSRE1 interrupt flag */
	SREMK1 = 0U;   /* enable INTSRE1 interrupt */
	SO0 |= _0004_SAU_CH2_DATA_OUTPUT_1;    /* output level normal */
	SOE0 |= _0004_SAU_CH2_OUTPUT_ENABLE;    /* enable UART1 output */
	SS0 |= _0008_SAU_CH3_START_TRG_ON | _0004_SAU_CH2_START_TRG_ON;    /* enable UART1 receive and transmit */
}


/******************************************************************************
 * Function Name : UART1_stop
 * Description : UART1 stop channel.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
void UART1_stop()
{
	ST0 |= _0008_SAU_CH3_STOP_TRG_ON | _0004_SAU_CH2_STOP_TRG_ON;    /* disable UART1 receive and transmit */
	SOE0 &= ~_0004_SAU_CH2_OUTPUT_ENABLE;    /* disable UART1 output */
	STMK1 = 1U;    /* disable INTST1 interrupt */
	STIF1 = 0U;    /* clear INTST1 interrupt flag */
	SRMK1 = 1U;    /* disable INTSR1 interrupt */
	SRIF1 = 0U;    /* clear INTSR1 interrupt flag */
	SREMK1 = 1U;   /* disable INTSRE1 interrupt */
	SREIF1 = 0U;   /* clear INTSRE1 interrupt flag */
}


/******************************************************************************
 * Function Name : UART_send
 * Description : UART send 8 bit data.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
int UART1_send(uint8_t *data, uint8_t size)
{
	if(uart_tx_size > 0){
		return 0;
	}
	if(size > 0){
		DI();
		uart_tx_size = size;
		uart_tx_buffer = data;
		uart_tx_size--;
		EI();
		SDR02 = (*uart_tx_buffer);
		return 1;
	}
	return 0;

}
/******************************************************************************
 * Function Name : UART_init
 * Description : UART Initialize.
 * Argument : none
 * Return Value : none
 ******************************************************************************/
void UART1_init()
{
	int i;
	SAU0EN	= 1;
	for(i = 0; i < 20; i++)
		NOP();
	SPS0	= _0002_SAU_CK00_FCLK_2 | _0020_SAU_CK01_FCLK_2;

	ST0 |= _0008_SAU_CH3_STOP_TRG_ON | _0004_SAU_CH2_STOP_TRG_ON;    /* disable UART1 receive and transmit */
	STMK1 = 1U;    /* disable INTST1 interrupt */
	STIF1 = 0U;    /* clear INTST1 interrupt flag */
	SRMK1 = 1U;    /* disable INTSR1 interrupt */
	SRIF1 = 0U;    /* clear INTSR1 interrupt flag */
	SREMK1 = 1U;   /* disable INTSRE1 interrupt */
	SREIF1 = 0U;   /* clear INTSRE1 interrupt flag */
	/* Set INTST1 low priority */
	STPR11 = 1U;
	STPR01 = 1U;
	/* Set INTSR1 low priority */
	SRPR11 = 1U;
	SRPR01 = 1U;
	/* Set INTSRE1 low priority */
	SREPR11 = 1U;
	SREPR01 = 1U;
	SMR02 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_TRIGGER_SOFTWARE |
		_0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;

	SCR02 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 |
		_0007_SAU_LENGTH_8;
	SDR02 = _CE00_UART1_TRANSMIT_DIVISOR;

	NFEN0 |= _04_SAU_RXD1_FILTER_ON;
	SMR03 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0100_SAU_TRIGGER_RXD | _0040_SAU_EDGE_RISING |
		_0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
	SCR03 = _4000_SAU_RECEPTION | _0400_SAU_INTSRE_ENABLE | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 |
		_0007_SAU_LENGTH_8;
	SDR03 = _CE00_UART1_RECEIVE_DIVISOR;

	SO0 &= ~_0004_SAU_CH2_DATA_OUTPUT_1;
	//SOL0 |= _0000_SAU_CHANNEL2_NORMAL;
	SOL0 |= _0004_SAU_CHANNEL2_INVERTED;    /* output level normal */
	SOE0 |= _0004_SAU_CH2_OUTPUT_ENABLE;    /* enable UART1 output */

	/* Set RxD1 pin */
	PMC0 &= 0xF7U;
	PM0 |= 0x08U;
	/* Set TxD1 pin */
	PMC0 &= 0xFBU;
	P0 |= 0x04U;
	PM0 &= 0xFBU;
}
