/*******************************************************************************
* File Name : r_dali.h
* Device(s) : 
* Description : This file implements DALI.
******************************************************************************/

#ifndef __DALI__
#define __DALI__

/******************************************************************************
Includes <System Includes> , ÅgProject IncludesÅh
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/
/* This is DALI grobal variable to save FLASH memory. */
/* Don't change order and size. */
typedef struct{
	uint8_t version_number;			/* + 0 */
	uint8_t physical_min_level;		/* + 1 */
	uint8_t device_type;			/* + 2 */
	uint8_t power_on_level;			/* + 3 */
	uint8_t system_failure_level;	/* + 4 */
	uint8_t min_level;				/* + 5 */
	uint8_t max_level;				/* + 6 */
	uint8_t fade_rate;				/* + 7 */
	uint8_t fade_time;				/* + 8 */
	uint8_t short_address;			/* + 9 */
	uint8_t random_address_h;		/* +10 */
	uint8_t random_address_m;		/* +11 */
	uint8_t random_address_l;		/* +12 */
	uint8_t group_0_7;				/* +13 */
	uint8_t group_8_15;				/* +14 */
	uint8_t scene[16];				/* +15 */
	uint8_t actual_level;			/* +31 */
	uint8_t reserved1;				/* +32 */
	uint8_t reserved2;				/* +33 */
	uint8_t reserved3;				/* +34 */
	uint8_t reserved4;				/* +35 */
	uint32_t dataexist;			/* +36 - +39 */
}DALI_VARS_T;

/* STATUS INFORMATION */
typedef struct {
	uint8_t	bit0	:1;
	uint8_t	bit1	:1;
	uint8_t	bit2	:1;
	uint8_t	bit3	:1;
	uint8_t	bit4	:1;
	uint8_t	bit5	:1;
	uint8_t	bit6	:1;
	uint8_t	bit7	:1;
}STATUS_T;

typedef union {
	STATUS_T status_bit;
	uint8_t status_byte;
}STATUS_U;

/******************************************************************************
Macro definitions
******************************************************************************/
#define CLR_ANSWER				{								\
									dali_answer  	  = 0;		\
									dali_answer_ready = FALSE;	\
								}

#define SET_ANSWER(data)		{								\
									dali_answer		  = data;	\
									dali_answer_ready = TRUE;	\
								}

/* for access the STATUS INFORMATION */
#define STATUS_ALL(ch)						dali_status[ch].status_byte			/* All status information */
#define STATUS_CONTROL_GEAR(ch)				dali_status[ch].status_bit.bit0		/* The status of the ballast */
#define STATUS_LAMP_FAILURE(ch)				dali_status[ch].status_bit.bit1		/* The status of the lamp failure */
#define STATUS_LAMP_POWER_ON(ch)			dali_status[ch].status_bit.bit2		/* The status of the lamp power on  */
#define STATUS_LIMIT_ERROR(ch)				dali_status[ch].status_bit.bit3		/* The limit error of actul level */
#define STATUS_FADE_RUNNING(ch)				dali_status[ch].status_bit.bit4		/* The status of fade */
#define STATUS_RESET_STATE(ch)				dali_status[ch].status_bit.bit5		/* The status of the config data */
#define STATUS_MISSING_SHORT_ADDRESS(ch)	dali_status[ch].status_bit.bit6		/* The status of the missing short address */
#define STATUS_POWER_FAILURE(ch)			dali_status[ch].status_bit.bit7		/* The status of power failure */
#define DALI_MAX_SLAVE	64

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern uint8_t dali_led_number;
extern uint8_t dali_answer;
extern uint8_t dali_answer_ready;
extern uint8_t dali_actual_level[NUMBER_OF_CHANNEL];
extern uint8_t dali_actual_target[NUMBER_OF_CHANNEL];		/* for fading */
extern DALI_VARS_T	 *dali_current_variable;	/* current configuration parameters */
extern STATUS_U 	 dali_status[NUMBER_OF_CHANNEL];

extern uint8_t dali_physical_selected;
extern uint8_t dali_last_command[NUMBER_OF_CHANNEL];
extern uint8_t dali_current_dtr[NUMBER_OF_CHANNEL];
extern uint8_t dali_current_dtr1[NUMBER_OF_CHANNEL];
extern uint8_t dali_current_dtr2[NUMBER_OF_CHANNEL];
extern uint8_t dali_fade_direction[NUMBER_OF_CHANNEL];
extern uint8_t dali_status_withdraw[NUMBER_OF_CHANNEL];
extern uint8_t dali_search_address_h;	/* Search Address(H) */
extern uint8_t dali_search_address_l;	/* Search Address(M) */
extern uint8_t dali_search_address_m;	/* Search Address(L) */

extern const uint8_t MASK_GROUP[16];

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
void DALI_init( void );
void DALI_ReceiveCommand( void );
uint8_t DALI_getValue( uint8_t channel );
void DALI_ActualLevelChangecheck( uint8_t );
void DALI_UpdateVariable( uint8_t channel );
void DALI_Fading( uint8_t channel);
void DALI_CheckActualLevelChanged( uint8_t channel );
void DALI_SetSystemFailure( uint8_t channel );
void DALI_ResetValue( void );
void DALI_CheckReset( void );
void DALI_SendCommand(uint16_t Command);
uint8_t DALI_ReadData( uint8_t *data );
void DALI_TestReceive( uint8_t data );
uint32_t Dali_IsDwnResponseNeeded( uint16_t command );
void DALI_RegisterExtRxHandler(void (*rx_ext)(void));
void DALI_UnRegisterExtRxHandler(void (*rx_ext)(void));

#endif
