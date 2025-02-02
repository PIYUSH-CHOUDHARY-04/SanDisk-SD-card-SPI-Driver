#ifndef SD_SPI_H
#define SD_SPI_H

    /**
     * File: SD_SPI.h
     * Description: This header file contains the commands, command formats and routine declarations for SPI driver to communicate with SD card over SPI interface.
     * Author: Piyush Choudhary
     * Date: December 25, 2024
     * Version: 1.0
     * Architecture : Little Endian
     */



#include<stdint.h>
#include<string.h>
#include "main.h"


/**
 * @brief macros for SPI interface information.
 */
#define CS_PORT_STRUCT_PTR		(GPIO_TypeDef*)GPIOB	// need to be modified by programmer.
#define CS_PORT_PIN_INDEX		(uint16_t)GPIO_PIN_12	// need to be modified by programmer.
#define HSPI_STRUCT_PTR			(SPI_HandleTypeDef*)(&hspi2)	// need to be modified by programmer.

/**
 * @brief macros specific to card.
 */
#define CARD_SDSC 0x00
#define CARD_SDHC 0x01
#define CARD_SDXC 0x02
 
#define CARD_TYPE CARD_SDHC     /* Must be modified as per needs. */


#define CRC7_POL    0x89    // x^7 + x^3 + 1
#define CRC16_POL   0x1021  // x^16 + x^12 + x^5 + 1
#define ARG_SIZE    0x04
#define SEND_CMD_INIT_BITS	0x40		// 2-MSB bits of command byte of the command format (0-1-<CMD_VAL>)
#define SEND_CMD_END_BIT	0x01		// 1-LSB bit for signaling the end of the command format (<CRC>-1)
/**
 * @defgroup BASIC_CMDs commands
 * @brief macros for CMDs (basic SD commands.) for SPI mode only. 
 * @{
 */

#define CMD0    SEND_CMD_INIT_BITS|0x00    /* GO_IDLE_STATE */
#define CMD1    SEND_CMD_INIT_BITS|0x01    /* SEND_OP_COND */
#define CMD6    SEND_CMD_INIT_BITS|0x06    /* SWITCH_FUNC */
#define CMD8    SEND_CMD_INIT_BITS|0x08    /* SEND_IF_COND */
#define CMD9    SEND_CMD_INIT_BITS|0x09    /* SEND_CSD */
#define CMD10   SEND_CMD_INIT_BITS|0x0A    /* SEND_CID */
#define CMD12   SEND_CMD_INIT_BITS|0x0C    /* STOP_TRANSMISSION */
#define CMD13   SEND_CMD_INIT_BITS|0x0D    /* SEND_STATUS */
#define CMD16   SEND_CMD_INIT_BITS|0x10    /* SET_BLOCKLEN */
#define CMD17   SEND_CMD_INIT_BITS|0x11    /* READ_SINGLE_BLOCK */
#define CMD18   SEND_CMD_INIT_BITS|0x12    /* READ_MULTIPLE_BLOCK */
#define CMD24   SEND_CMD_INIT_BITS|0x18    /* WRITE_BLOCK */
#define CMD25   SEND_CMD_INIT_BITS|0x19    /* WRITE_MULTIPLE_BLOCK */
#define CMD27   SEND_CMD_INIT_BITS|0x1B    /* PROGRAM_CSD */
#define CMD28   SEND_CMD_INIT_BITS|0x1C    /* SET_WRITE_PROT */
#define CMD29   SEND_CMD_INIT_BITS|0x1D    /* CLR_WRITE_PROT */
#define CMD30   SEND_CMD_INIT_BITS|0x1E    /* SEND_WRITE_PROT */
#define CMD32   SEND_CMD_INIT_BITS|0x20    /* ERASE_WR_BLK_START_ADDR */
#define CMD33   SEND_CMD_INIT_BITS|0x21    /* ERASE_WR_BLK_END_ADDR */
#define CMD38   SEND_CMD_INIT_BITS|0x26    /* ERASE */
#define CMD42   SEND_CMD_INIT_BITS|0x2A    /* LOCK_UNLOCK */
#define CMD55   SEND_CMD_INIT_BITS|0x37    /* APP_CMD */
#define CMD56   SEND_CMD_INIT_BITS|0x38    /* GEN_CMD */
#define CMD58   SEND_CMD_INIT_BITS|0x3A    /* READ_OCR */
#define CMD59   SEND_CMD_INIT_BITS|0x3B    /* CRC_ON_OFF */

/** 
 * @}
 */

/**
 * @defgroup APP_CMDs apps_commands
 * @breif macros for application specific commands for SPI mode only.
 * @{
 */

#define ACMD13  SEND_CMD_INIT_BITS|0x0D    /* SD_STATUS */
#define ACMD18  SEND_CMD_INIT_BITS|0x12    /* Reserved for SD security */
#define ACMD22  SEND_CMD_INIT_BITS|0x16    /* SEND_NUM_WR_BLOCK */
#define ACMD23  SEND_CMD_INIT_BITS|0x17    /* SET_WR_BLK_ERASE_COUNT */
#define ACMD25  SEND_CMD_INIT_BITS|0x19    /* Reserved for SD security */
#define ACMD26  SEND_CMD_INIT_BITS|0x1A    /* Reserved for SD security */
#define ACMD38  SEND_CMD_INIT_BITS|0x26    /* Reserved for SD security */
#define ACMD41  SEND_CMD_INIT_BITS|0x29    /* SEND_SD_OP_COND */
#define ACMD42  SEND_CMD_INIT_BITS|0x2A    /* SET_CLR_CARD_DETECT */
#define ACMD43  SEND_CMD_INIT_BITS|0x2B    /* Reserved for SD security */
#define ACMD44  SEND_CMD_INIT_BITS|0x2C    /* Reserved for SD security */
#define ACMD45  SEND_CMD_INIT_BITS|0x2D    /* Reserved for SD security */
#define ACMD46  SEND_CMD_INIT_BITS|0x2E    /* Reserved for SD security */
#define ACMD47  SEND_CMD_INIT_BITS|0x2F    /* Reserved for SD security */
#define ACMD48  SEND_CMD_INIT_BITS|0x30    /* Reserved for SD security */
#define ACMD49  SEND_CMD_INIT_BITS|0x31    /* Reserved for SD security */
#define ACMD51  SEND_CMD_INIT_BITS|0x33    /* SEND_SCR */


/**
 * @}
 */

/**
 * @brief Command types based on the responses.
 */
#define CMD_TYPE_R1		0x01
#define CMD_TYPE_R1B	0x02
#define CMD_TYPE_R2		0x04
#define CMD_TYPE_R3		0x08
#define CMD_TYPE_R7		0x10


/**
 * @brief Command specific macros, basically some default hard-coded values.
 * @{
 */

/**
 * @brief macros :: CMD0
 */

//#define ARG_CMD0	(uint32_t)(0x00000000)	// only possible argument for CMD0 with no use of argument field.
#define CRC_CMD0 	(0x4A << 1)|SEND_CMD_END_BIT
#define CMD0_MAX_TRIES  0x0F

/**
 * @brief macros :: CMD8
 */
#define VHS_CMD8_ND			0x00	// Not defined value for VHS (Voltage supplied)
#define VHS_CMD8_DEFAULT	0x01	// Default value of voltage supply i.e. 2.7V-3.6V
#define VHS_CMD8_LVR		0x02	// reserved for low voltage region
#define VHS_CMD8_RES1		0x04	// reserved value 1
#define VHS_CMD8_RES2		0x08	// reserved value 2
#define CMD8_CHECK_PATTERN_DEFAULT	0xAA	// default check patter, will be received in echo in R7 response.

//#define ARG_CMD8	(uint32_t)(0x000001AA)	// a default voltage range selection and default preferred check pattern sending for echo back.
#define CRC_CMD8_DEFAULT	(0x43 << 1)|SEND_CMD_END_BIT		// default values for args : <reserved bits> | < VHS : VHS_CMD8_DEFAULT > | < Check pattern : CMD8_CHECK_PATTERN_DEFAULT >

/**
 * @brief macros :: CMD55
 */
//#define ARG_CMD55	(uint32_t)(0x00000000)
#define CRC_CMD55_DEFAULT	(0x32 << 1)|SEND_CMD_END_BIT



/**
 * @brief macros :: ACMD41
 */
//#define ARG_ACMD41	(uint32_t)(0x40000000)	// default argument setting the HCS(High Capacity SD) bit to tell SD card that host supports SDHC/SDXC as well.
#define CRC_ACMD41_DEFAULT

/**
 * @}
 */

#define DUMMY_BYTE	0xFF

/**
 * @defgroups CMD_FORMATTING cmd_formatting
 * @brief command formatting routines are structure required to create a command for data transaction.
 * @{
 */

/**
 * @brief cmd_format structure is used to create a structure which acts an entire command to be sent to the SD card.
 * @param uint8_t CMD holds the actual command, the bit[7] of CMD is always '0' as its starts bit of the transaction, and bit[6] is '1' as it represents the operation being performed i.e. transmission. 
 * @param uint8_t ARG holds the argument for the command CMD.
 * @param uint8_t CRC holds CRC of the entire 38 bits of CMD|ARG , upper 7 bits represents CRC and bit[0] is always '1' as its last bit of the transaction.
 */
typedef struct{
    uint8_t CMD;
    uint8_t ARG[ARG_SIZE];
    uint8_t CRC7;
} cmd_format ;

/**
 * @brief structure holding responses for SD commands over SPI interface.
 * @param uint8_t r1[1] holds r1 response of the last r1 responding command
 * @param uint8_t r1b[1] holds r1b response of the last r1b responding command
 * @param uint8_t r2[2] holds r2 response of the last r2 responding command 
 * @param uint8_t r3[5] holds r3 response of the last r3 responding command
 * @param uint8_t r7[5] holds r7 response of the last r7 responding command 
 */
typedef struct{
	uint8_t r1[1];
	uint8_t r1b[1];
	uint8_t r2[2];
	uint8_t r3[5];
	uint8_t r7[5];
} resp;

/**
 * @brief getCRC calculates the CRC7 of a given sequence.
 * @param uint8_t* addr passes the address of the data whose CRC7 has to be calculated.
 * @param uint16_t size passes the size of the data whose CRC7 has to be calculated.
 * @retval uint8_t returns the 8 bit value whose lower 7 bits are CRC7.
 */
uint8_t getCRC7(uint8_t* addr, uint16_t size);

/**
 * @brief getCRC calculates the CRC16 of a given sequence.
 * @param uint8_t* addr passes the address of the data whose CRC16 has to be calculated.
 * @param uint16_t size passes the size of the data whose CRC16 has to be calculated.
 * @retval uint16_t returns the 16 bit CRC16.
 */
uint16_t getCRC16(uint8_t* addr, uint16_t size);

/**
 * @}
 */


/**
 * @brief extern symbols necessary for below macro symbols.
 */
extern uint8_t arg_cmds[ARG_SIZE];
extern cmd_format Cmd;
extern resp response;
/**
 * @brief Macros for clearing specific structure buffers like cmd_format, arg_cmds, resp
 */
#define SET_CMD_FORMAT(val) 			memset(&Cmd, val, sizeof(cmd_format))
#define SET_ARG_CMDS(val)				memset(arg_cmds, val, ARG_SIZE)
#define SET_RESP(val)					memset(&response, val, sizeof(resp))

#define SET_ALL(_cmd_ptr,_args_cmds_ptr,_resp_ptr,_val)	do{		\
	SET_CMD_FORMAT(_val);										\
	SET_ARG_CMDS(_val);											\
	SET_RESP(_val);												\
}while(0U)


/**
 * @brief Selects the SD card by asserting CS low.
 * @param void
 * @retval void
 */
void SD_Select(void);

/**
 * @brief De-selects the SD card by de-asserting CS high.
 * @param void
 * @retval void
 */
void SD_Deselect(void);

/**
 * @brief Sends a specific command
 * @param uint8_t command passes the command value corresponding to the command to be sent.
 * @param uint8_t* arg passes the pointer to the 4 byte argument array.
 * @retval uint8_t* returns the pointer to appropriate buffer in 'resp' structure.
 */
uint8_t* SendSD_Command(cmd_format* cmd, uint8_t command, uint8_t cmd_type, uint8_t* arg, resp* respbox);

/**
 * @brief Transmit specific number of bytes, can be used with data write etc commands to send entire data block.  Chip must always be selected before using this routine.
 * @param uint8_t* bytestream passes the pointer to the bytestream to be transmitted i.e pointer to the data to be sent or written to the SD card
 * @param uint16_t byte_count passes the size of the data in bytes to be transferred
 * @retval uint16_t returns the size of transmitted data in bytes
 */
uint16_t SD_TransmitBytes(uint8_t* bytestream, uint16_t byte_count);

/**
 * @brief Receives specific number of bytes, can be used just after polling confirmation, like can be used to read data.  Chip must always be selected before using this routine.
 * @param uint8_t* buffer passes hte pointer to the memory region where the incoming data has to be stored
 * @param uint16_t byte_count passes the size of the data to be received in bytes
 * @retval uint16_t returns the size of received data in bytes
 */
uint16_t SD_ReceiveBytes(uint8_t* buffer, uint16_t byte_count);

/**
 * @brief sends specific number of dummy bytes i.e. spare clock cycles.
 * @param SPI_HandleTypeDef* hspiX passes the pointer of the SPI interface structure to which dummy bytes has to be sent.'
 * @param uint16_t num_bytes tells exact number of dummy bytes need to be sent over the specified SPI interface
 * @retval void
 */
void SD_SendDummyBytes(SPI_HandleTypeDef* hspiX, uint16_t num_bytes);

/**
 * @brief Initializes the SD card in SPI mode.
 * @param uint8_t tells the exit status of the SD-init like 0x00 in success else non-zero for failure.
 * @retval void
 */
uint8_t SD_init(cmd_format* _cmd, uint8_t* _arg_cmds , resp* _respbox);





#endif /* SD_SPI_H */
