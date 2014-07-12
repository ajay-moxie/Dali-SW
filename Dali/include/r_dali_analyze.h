/*******************************************************************************
* File Name : DALI_analyze.h
* Device(s) : 
* Description : This file implements DALI.
******************************************************************************/

#ifndef __DALI_ANALYZE__
#define __DALI_ANALYZE__

/******************************************************************************
Macro definitions
******************************************************************************/
#define ADDRESS_BROADCAST									0xFE			/* Broadcast value */

/* Command */
#define COMMAND_OFF											0x00			/* OFF */
#define COMMAND_UP											0x01			/* UP */
#define COMMAND_DOWN										0x02			/* DOWN */
#define COMMAND_STEP_UP										0x03			/* STEP UP */
#define COMMAND_STEP_DOWN									0x04			/* STEP DOWN */
#define COMMAND_RECALL_MAX_LEVEL							0x05			/* RECALL MAX LEVEL */
#define COMMAND_RECALL_MIN_LEVEL							0x06			/* RECALL MIN LEVEL */
#define COMMAND_STEP_DOWN_AND_OFF							0x07			/* STEP DOWN AND OFF */
#define COMMAND_ON_AND_STEP_UP								0x08			/* ON AND STEP UP */
#define COMMAND_ENABLE_DAPC_SEQUENCE						0x09			/* ENABLE DAPC SEQUENCE */
#define COMMAND_GO_TO_SCENE									0x10			/* GO TO SCENE */
#define COMMAND_RESET										0x20			/* RESET */
#define COMMAND_STORE_ACTUAL_LEVEL_IN_THE_DTR				0x21			/* STORE ACTUAL LEVEL IN THE DTR */
#define COMMAND_STORE_THE_DTR_AS_MAX_LEVEL					0x2A			/* STORE THE DTR AS MAX LEVEL */
#define COMMAND_STORE_THE_DTR_AS_MIN_LEVEL					0x2B			/* STORE THE DTR AS MIN LEVEL */
#define COMMAND_STORE_THE_DTR_AS_SYSTEM_FAILURE_LEVEL		0x2C			/* STORE THE DTR AS SYSTEM FAILURE LEVEL */
#define COMMAND_STORE_THE_DTR_AS_POWER_ON_LEVEL				0x2D			/* STORE THE DTR AS POWER ON LEVEL */
#define COMMAND_STORE_THE_DTR_AS_FADE_TIME					0x2E			/* STORE THE DTR AS FADE TIME */
#define COMMAND_STORE_THE_DTR_AS_FADE_RATE					0x2F			/* STORE THE DTR AS FADE RATE */
#define COMMAND_STORE_THE_DTR_AS_SCENE						0x40			/* STORE THE DTR AS SCENE */
#define COMMAND_REMOVE_FROM_SCENE							0x50			/* REMOVE FROM SCENE */
#define COMMAND_ADD_TO_GROUP								0x60			/* ADD TO GROUP */
#define COMMAND_REMOVE_FROM_GROUP							0x70			/* REMOVE FROM GROUP */
#define COMMAND_STORE_DTR_AS_SHORT_ADDRESS					0x80			/* STORE DTR AS SHORT ADDRESS */
#define COMMAND_ENABLE_WRITE_MEMORY							0x81			/* ENABLE WRITE MEMORY */
#define COMMAND_QUERY_STATUS								0x90			/* QUERY STATUS */
#define COMMAND_QUERY_CONTROL_GEAR							0x91			/* QUERY BALLAST */
#define COMMAND_QUERY_LAMP_FAILURE							0x92			/* QUERY LAMP FAILURE */
#define COMMAND_QUERY_LAMP_POWER_ON							0x93			/* QUERY LAMP POWER ON */
#define COMMAND_QUERY_LIMIT_ERROR							0x94			/* QUERY LIMIT ERROR */
#define COMMAND_QUERY_RESET_STATE							0x95			/* QUERY RESET STATE */
#define COMMAND_QUERY_MISSING_SHORT_ADDRESS					0x96			/* QUERY MISSING SHORT ADDRESS */
#define COMMAND_QUERY_VERSION_NUMBER						0x97			/* QUERY VERSION NUMBER */
#define COMMAND_QUERY_CONTENT_DTR							0x98			/* QUERY CONTENT DTR */
#define COMMAND_QUERY_DEVICE_TYPE							0x99			/* QUERY DEVICE TYPE */
#define COMMAND_QUERY_PHYSICAL_MINIMUM_LEVEL				0x9A			/* QUERY PHYSICAL MINIMUM LEVEL */
#define COMMAND_QUERY_POWER_FAILURE							0x9B			/* QUERY POWER FAILURE */
#define COMMAND_QUERY_CONTENT_DTR1							0x9C			/* QUERY CONTENT DTR1 */
#define COMMAND_QUERY_CONTENT_DTR2							0x9D			/* QUERY CONTENT DTR1 */
#define COMMAND_QUERY_HOME_AUTOMATION_DEVICE_ID							0x9F			/* QUERY HOME AUTOMATION DEVICE ID */
#define COMMAND_QUERY_ACTUAL_LEVEL							0xA0			/* QUERY ACTUAL LEVEL */
#define COMMAND_QUERY_MAX_LEVEL								0xA1			/* QUERY MAX LEVEL */
#define COMMAND_QUERY_MIN_LEVEL								0xA2			/* QUERY MIN LEVEL */
#define COMMAND_QUERY_POWER_ON_LEVEL						0xA3			/* QUERY POWER ON LEVEL */
#define COMMAND_QUERY_SYSTEM_FAILURE_LEVEL					0xA4			/* QUERY SYSTEM FAILURE LEVEL */
#define COMMAND_QUERY_FADE_SETTING							0xA5			/* QUERY FADE TIME / FADE RATE */
#define COMMAND_QUERY_SCENE_LEVEL							0xB0			/* QUERY SCENE LEVEL */
#define COMMAND_QUERY_GROUPS_0_7							0xC0			/* QUERY GROUPS 0-7 */
#define COMMAND_QUERY_GROUPS_8_15							0xC1			/* QUERY GROUPS 8-15 */
#define COMMAND_QUERY_RANDOM_ADDRESS_H						0xC2			/* QUERY RANDOM ADDRESS (H) */
#define COMMAND_QUERY_RANDOM_ADDRESS_M						0xC3			/* QUERY RANDOM ADDRESS (M) */
#define COMMAND_QUERY_RANDOM_ADDRESS_L						0xC4			/* QUERY RANDOM ADDRESS (L) */
#define COMMAND_READ_MEMORY_LOCATION						0xC5			/* READ MEMORY LOCATION */
#define COMMAND_QUERY_EXTENDED_VERSION_NUMBER				0xFF			/* QUERY EXTENDED VERSION NUMBER */
#define EXCOMMAND_TERMINATE									0xA1			/* TERMINATE */
#define EXCOMMAND_DTR										0xA3			/* DTR */
#define EXCOMMAND_INITIALIZE								0xA5			/* INITIALIZE */
#define EXCOMMAND_RANDOMISE									0xA7			/* RANDOMISE */
#define EXCOMMAND_COMPARE									0xA9			/* COMPARE */
#define EXCOMMAND_WITHRAW									0xAB			/* WITHDRAW */
#define EXCOMMAND_SEARCHADDRH								0xB1			/* SEARCHADDRH */
#define EXCOMMAND_SEARCHADDRM								0xB3			/* SEARCHADDRM */
#define EXCOMMAND_SEARCHADDRL								0xB5			/* SEARCHADDRL */
#define EXCOMMAND_PROGRAM_SHORT_ADDRESS						0xB7			/* PROGRAM SHORT ADDRESS */
#define EXCOMMAND_VERIFY_SHORT_ADDRESS						0xB9			/* VERIFY SHORT ADDRESS */
#define EXCOMMAND_QUERY_SHORT_ADDRESS						0xBB			/* QUERY SHORT ADDRESS */
#define EXCOMMAND_PHYSICAL_SELECTION						0xBD			/* PHYSICAL SELECTION */
#define EXCOMMAND_ENABLE_DEVICE_TYPE_X						0xC1			/* ENABLE DEVICE TYPE X */
#define EXCOMMAND_DATA_TRANSFER_REGISTER_1					0xC3			/* DATA TRANSFER REGISTER 1 */
#define EXCOMMAND_DATA_TRANSFER_REGISTER_2					0xC5			/* DATA TRANSFER REGISTER 2 */
#define EXCOMMAND_WRITE_MEMORY_LOCATION						0xC7			/* WRITE MEMORY LOCATION */

#define COMMAND_NONE										0xFF			/* COMMAND INVALID */

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
void DALI_AnalyzeCommand(uint16_t cmd_data);
uint8_t DALI_CheckConfigCommand( uint8_t cmd_data );

#endif
