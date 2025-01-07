#ifndef SD_SPI_H
#define SD_SPI_H

    /**
     * File: SD_SPI.h
     * Description: This header file contains the commands, command formats and routine declarations for SPI driver t communicate with SD card over SPI interface.
     * Author: Piyush Choudhary
     * Date: December 25, 2024
     * Version: 1.0
     * Architecture : Little Endian
     */



#include<stdint.h>


#define CARD_SDSC 0x00
#define CARD_SDHC 0x01
#define CARD_SDXC 0x02
 
#define CARD_TYPE CARD_SDHC     /* Must be modified as per needs. */


#define CRC7_POL    0x89    // x^7 + x^3 + 1
#define CRC16_POL   0x1021  // x^16 + x^12 + x^5 + 1
#define ARG_SIZE    0x04


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
    uint8_t CRC;
} cmd_format ;

/**
 * @brief CreateCMD create a command to be used for data transaction.
 * @param cmd_format* cmd passes a pointer to the cmd_format structure which in turn gets populated with the subsequent arguments passed to the routine.
 * @param uint8_t command passes the command to be used in transaction.
 * @param uint32_t arg passes the argument for the command being used in transaction.
 * @retval void
 */
void CreateCMD(cmd_format* cmd, uint8_t command, uint32_t arg, uint8_t crc);

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
uint8_t getCRC16(uint8_t* addr, uint16_t size);
/**
 * @}
 */


/**
 * @defgroup BASIC_CMDs commands
 * @brief macros for CMDs (basic SD commands.) for SPI mode only. 
 * @{
 */

#define CMD0    0x00    /* GO_IDLE_STATE */
#define CMD1    0x01    /* SEND_OP_COND */
#define CMD6    0x06    /* SWITCH_FUNC */
#define CMD8    0x08    /* SEND_IF_COND */
#define CMD9    0x09    /* SEND_CSD */
#define CMD10   0x0A    /* SEND_CID */
#define CMD12   0x0C    /* STOP_TRANSMISSION */
#define CMD13   0x0D    /* SEND_STATUS */
#define CMD16   0x10    /* SET_BLOCKLEN */
#define CMD17   0x11    /* READ_SINGLE_BLOCK */
#define CMD18   0x12    /* READ_MULTIPLE_BLOCK */
#define CMD24   0x18    /* WRITE_BLOCK */
#define CMD25   0x19    /* WRITE_MULTIPLE_BLOCK */
#define CMD27   0x1B    /* PROGRAM_CSD */
#define CMD28   0x1C    /* SET_WRITE_PROT */
#define CMD29   0x1D    /* CLR_WRITE_PROT */
#define CMD30   0x1E    /* SEND_WRITE_PROT */
#define CMD32   0x20    /* ERASE_WR_BLK_START_ADDR */
#define CMD33   0x21    /* ERASE_WR_BLK_END_ADDR */
#define CMD38   0x26    /* ERASE */
#define CMD42   0x2A    /* LOCK_UNLOCK */
#define CMD55   0x33    /* APP_CMD */
#define CMD56   0x34    /* GEN_CMD */
#define CMD58   0x36    /* READ_OCR */
#define CMD59   0x37    /* CRC_ON_OFF */

/** 
 * @}
 */

/**
 * @defgroup APP_CMDs apps_commands
 * @breif macros for application specific commands for SPI mode only.
 * @{
 */

#define ACMD13  0x0D    /* SD_STATUS */
#define ACMD18  0x12    /* Reserved for SD security */
#define ACMD22  0x16    /* SEND_NUM_WR_BLOCK */
#define ACMD23  0x17    /* SET_WR_BLK_ERASE_COUNT */
#define ACMD25  0x19    /* Reserved for SD security */
#define ACMD26  0x1A    /* Reserved for SD security */
#define ACMD38  0x26    /* Reserved for SD security */
#define ACMD41  0x29    /* SEND_SD_OP_COND */
#define ACMD42  0x2A    /* SET_CLR_CARD_DETECT */
#define ACMD43  0x2B    /* Reserved for SD security */
#define ACMD44  0x2C    /* Reserved for SD security */
#define ACMD45  0x2D    /* Reserved for SD security */
#define ACMD46  0x2E    /* Reserved for SD security */
#define ACMD47  0x2F    /* Reserved for SD security */
#define ACMD48  0x30    /* Reserved for SD security */
#define ACMD49  0x31    /* Reserved for SD security */
#define ACMD51  0x33    /* SEND_SCR */


/**
 * @}
 */


/**
 * @brief Initializes the SD card in SPI mode.
 * @retval void
 */
void SD_init();





#endif /* SD_SPI_H */
