 
#include "SD_SPI.h"


/**
 * @brief declaring a command format structure to be used throughout the code to cast the commands into.
 */
cmd_format Cmd;

/**
 * @brief declaring an argument buffer to be used to cast the commands.
 */
uint8_t arg_cmds[ARG_SIZE]={0};

/**
 * @brief delcaring the response box structure to capture the response of any command.
 */
resp response;

/**
 * @brief getCRC calculates the CRC7 of a given sequence.
 * @param uint8_t* addr passes the address of the data whose CRC7 has to be calculated.
 * @param uint16_t size passes the size in bytes (if not byte aligned, passed the corresponding higher byte) of the data whose CRC7 has to be calculated.
 * @retval uint8_t returns the 8 bit value whose lower 7 bits are CRC7.
 */
uint8_t getCRC7(uint8_t* addr, uint16_t size){
    uint8_t crc = 0; // Initial CRC value is 0
    
    for (uint8_t i = 0; i < size; i++) {
        uint8_t byte = addr[i];
        for (uint8_t bit = 0; bit < 8; bit++) {
            crc <<= 1;
            if ((byte & 0x80) ^ (crc & 0x80)) { // Check MSB
                crc ^= CRC7_POL; // XOR with polynomial if MSBs differ
            }
            byte <<= 1; // Shift to process the next bit
        }
    }
    return (crc & 0x7F); // Return only the lower 7 bits of the CRC
}


/**
 * @brief getCRC calculates the CRC16 of a given sequence.
 * @param uint8_t* addr passes the address of the data whose CRC16 has to be calculated.
 * @param uint16_t size passes the size in bytes (if not byte aligned, passed the corresponding higher byte) of the data whose CRC16 has to be calculated.
 * @retval uint16_t returns the 16 bit CRC16.
 */
uint16_t getCRC16(uint8_t* addr, uint16_t size){
    uint16_t crc = 0x0000; // Initial CRC value

    for (size_t i = 0; i < size; i++) {
        crc ^= (addr[i] << 8); // XOR the MSB of the current byte with CRC
        for (uint8_t bit = 0; bit < 8; bit++) { // Process each bit
            if (crc & 0x8000) {
                crc = (crc << 1) ^ CRC16_POL; // XOR with polynomial
            } else {
                crc <<= 1; // Shift left
            }
        }
    }
    return crc; // Final 16-bit CRC
}

/**
 * @brief Selects the SD card by asserting CS low.
 * @param void
 * @retval void
 */
void SD_Select(void){
	HAL_GPIO_WritePin(CS_PORT_STRUCT_PTR, CS_PORT_PIN_INDEX, GPIO_PIN_RESET);
}

/**
 * @brief De-selects the SD card by de-asserting CS high.
 * @param void
 * @retval void
 */
void SD_Deselect(void){
	HAL_GPIO_WritePin(CS_PORT_STRUCT_PTR, CS_PORT_PIN_INDEX, GPIO_PIN_SET);
}

/**
 * @brief Sends a specific command
 * @param uint8_t command passes the command value corresponding to the command to be sent.
 * @param uint8_t* arg passes the pointer to the 4 byte argument array.
 * @retval uint8_t* returns the pointer to appropriate buffer in 'resp' structure.
 */
uint8_t* SendSD_Command(cmd_format* cmd, uint8_t command, uint8_t cmd_type, uint8_t* arg, resp* respbox){
	
	vcom_printf("CURRENT_CMD_EXEC : CMD%d\r\n",command&(0x3F));


	// Preparing the command to be sent.
	cmd->CMD=command;
	for(uint8_t i=0;i<4;i++){
		(cmd->ARG)[i]=arg[i];
	}
	
	switch(command){
		case CMD0 :
			cmd->CRC7=CRC_CMD0;
			vcom_printf("Setting CMD0 CRC...\r\n");
			break;

		case CMD8 :
			vcom_printf("Setting CMD8 CRC...\r\n");
			cmd->CRC7=CRC_CMD8_DEFAULT;
			break;

		case CMD55 :
			vcom_printf("Setting CMD55 CRC...\r\n");
			cmd->CRC7=CRC_CMD55_DEFAULT;
			break;

		default :
			cmd->CRC7=((getCRC7((uint8_t*)cmd,ARG_SIZE+1)<<1)|(SEND_CMD_END_BIT));
			vcom_printf("CRC7 : %d %#x\r\n",cmd->CRC7,cmd->CRC7);
	}

	vcom_printf("COMMAND DUMP : \r\n");
	for(uint8_t i=0;i<6;i++){
		vcom_printf("CMD_DMP[%d] : %d %#x\r\n",i,((uint8_t*)cmd)[i],((uint8_t*)cmd)[i]);
	}

	SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
	SD_Select();
	SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
	// sending the command
	vcom_printf("Sending command...\r\n");
	uint8_t stat=HAL_SPI_Transmit(HSPI_STRUCT_PTR, (uint8_t*)cmd, sizeof(*cmd), HAL_MAX_DELAY);
	vcom_printf("stat : %d %#x\r\n",stat,stat);
	if(stat!=HAL_OK){

		return NULL;
	}
	SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
	SD_Deselect();
	SET_RESP(DUMMY_BYTE);
	// waiting for response...
	if(cmd_type==CMD_TYPE_R1){
		// SD card, in response of void receive routines returns dummy bytes 0xFF and thus can be easily polled for non-dummy bytes.
			for(uint8_t count=0;count<20;count++){
				HAL_SPI_TransmitReceive(HSPI_STRUCT_PTR, respbox->r1b, respbox->r1, sizeof(uint8_t), HAL_MAX_DELAY);
				if(*(respbox->r1) != DUMMY_BYTE){
					return respbox->r1;
				}
			}
			vcom_printf("CMD_TYPE_R1\r\n");
	   }else if(cmd_type==CMD_TYPE_R1B ){
		   for(uint8_t count=0;count<8;count++){

			   HAL_SPI_TransmitReceive(HSPI_STRUCT_PTR, respbox->r2, respbox->r1b, sizeof(uint8_t), HAL_MAX_DELAY);
			   if(*(respbox->r1b) != DUMMY_BYTE){

				   return respbox->r1b;
			   }

		   }
		   vcom_printf("CMD_TYPE_R1B\r\n");
	   }else if(cmd_type==CMD_TYPE_R2){	// checking for command with r2 response
		   for(uint8_t count=0;count<8;count++){


			   HAL_SPI_TransmitReceive(HSPI_STRUCT_PTR, respbox->r3, respbox->r2, sizeof(uint8_t), HAL_MAX_DELAY);
			   if((respbox->r2)[0] != DUMMY_BYTE){
				   HAL_SPI_TransmitReceive(HSPI_STRUCT_PTR, respbox->r3, &((respbox->r2)[1]), sizeof(uint8_t), HAL_MAX_DELAY);

				   return respbox->r2;
			   }

		   }
		   vcom_printf("CMD_TYPE_R2\r\n");
	   }else if(cmd_type==CMD_TYPE_R3){	// checking for command with r3 response
		   for(uint8_t count=0;count<8;count++){


			   HAL_SPI_TransmitReceive(HSPI_STRUCT_PTR, respbox->r7, respbox->r3, sizeof(uint8_t), HAL_MAX_DELAY);
			   if((respbox->r3)[0] != DUMMY_BYTE){
				   HAL_SPI_TransmitReceive(HSPI_STRUCT_PTR, respbox->r7, &((respbox->r3)[1]), sizeof(respbox->r3)/sizeof(uint8_t)-1, HAL_MAX_DELAY);

				   return respbox->r3;
			   }

		   }
		   vcom_printf("CMD_TYPE_R3\r\n");
	   }else if(cmd_type==CMD_TYPE_R7){	// checking for command with r7 response

		    vcom_printf("Setting response for CMD8...\r\n");

		    for(uint8_t count=0;count<8;count++){


		    	HAL_SPI_TransmitReceive(HSPI_STRUCT_PTR, respbox->r3, respbox->r7, sizeof(uint8_t), HAL_MAX_DELAY);
		    	if((respbox->r7)[0] != DUMMY_BYTE){
		    		vcom_printf("count value : %d\r\n",count);
		    		HAL_SPI_TransmitReceive(HSPI_STRUCT_PTR, respbox->r3, &((respbox->r7)[1]), sizeof(respbox->r7)/sizeof(uint8_t)-1, HAL_MAX_DELAY);
		    		return respbox->r7;
		    	}
		    }
		    vcom_printf("CMD_TYPE_R7\r\n");
	   }
	   vcom_printf("LAST NULL RETURN SD_init()\r\n");
	   return NULL;
}

/**
 * @brief Transmit specific number of bytes, can be used with data write etc commands to send entire data block. Chip must always be selected before using this routine.
 * @param uint8_t* bytestream passes the pointer to the bytestream to be transmitted i.e pointer to the data to be sent or written to the SD card
 * @param uint16_t byte_count passes the size of the data in bytes to be transferred
 * @retval uint16_t returns the size of transmitted data in bytes
 */
uint16_t SD_TransmitBytes(uint8_t* bytestream, uint16_t byte_count){
	if(HAL_SPI_Transmit(HSPI_STRUCT_PTR, bytestream, byte_count, HAL_MAX_DELAY)!=HAL_OK){
		return 0x00;
	}
	return byte_count;
}

/**
 * @brief Receives specific number of bytes, can be used just after polling confirmation, like can be used to read data.  Chip must always be selected before using this routine.
 * @param uint8_t* buffer passes hte pointer to the memory region where the incoming data has to be stored
 * @param uint16_t byte_count passes the size of the data to be received in bytes
 * @retval uint16_t returns the size of received data in bytes
 */
uint16_t SD_ReceiveBytes(uint8_t* buffer, uint16_t byte_count){
	uint8_t db=DUMMY_BYTE;
	for(uint16_t size=0;size<byte_count;size++){
		if(HAL_SPI_TransmitReceive(HSPI_STRUCT_PTR, &db, &(buffer[size]), sizeof(uint8_t), HAL_MAX_DELAY)!=HAL_OK){
			return size;
		}
	}
	return byte_count;
}

/**
 * @brief sends specific number of dummy bytes i.e. spare clock cycles, programmer itself needs to decide whether to select or deselect the SD card chip before sending the clock cycles.
 * @param SPI_HandleTypeDef* hspiX passes the pointer of the SPI interface structure to which dummy bytes has to be sent.'
 * @param uint16_t num_bytes tells exact number of dummy bytes need to be sent over the specified SPI interface
 * @retval void
 */
void SD_SendDummyBytes(SPI_HandleTypeDef* hspiX, uint16_t num_bytes){
	uint8_t db=DUMMY_BYTE;
	uint8_t res=0x00;
	for(;num_bytes>0;num_bytes--){
		HAL_SPI_TransmitReceive(HSPI_STRUCT_PTR, &db, &res,1,HAL_MAX_DELAY);
	}

}


/**
 * @brief Initializes the SD card in SPI mode.
 * @param void
 * @retval uint8_t
 */
uint8_t SD_init(cmd_format* _cmd, uint8_t* _arg_cmds , resp* _respbox){

	// Sending ~74 clock cycles with CS high.
	SD_Deselect();
	SD_SendDummyBytes(HSPI_STRUCT_PTR,10);

	// Clearing all the above static structures and arrays.
	SET_ALL(_cmd,_arg_cmds,_respbox,~DUMMY_BYTE);

	// +++++++++++++++++++++++++++++++++++++++++++++++++ CMD0 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// selecting the chip and sending CMD0 until response comes out to be r1 : 0x01
	
	SD_Select();
	SD_SendDummyBytes(HSPI_STRUCT_PTR,1);		// sending 8 clock cycles.

	vcom_printf("Sending CMD0 and capturing response...\r\n");

	for(uint8_t try=0;try<CMD0_MAX_TRIES;try++){
		if(SendSD_Command(_cmd,CMD0,CMD_TYPE_R1,_arg_cmds,_respbox)==NULL){
			SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
			SD_Deselect();
			vcom_printf("CMD0 failed.\r\n");
			return 0x01;	// CMD0 send failed.
		}

		if((*(_respbox->r1) != 0x01)  &&  (*(_respbox->r1) != 0x02)  &&  (*(_respbox->r1) != 0x00)){				// checking the response returned by the CMD0, thus checking response box.
			SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
			SD_Deselect();
			vcom_printf("CMD0 response checking, not in idle state.\r\n");
			vcom_printf("CMD0 response : %d %#x\r\n",*(_respbox->r1), *(_respbox->r1));
			return 0x02;	// CMD0 response checking, not in idle state.
		}else{
			break;
		}
	}
	vcom_printf("CMD0 response : %d %#x\r\n",*(_respbox->r1), *(_respbox->r1));
	SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
	SD_Deselect();

	// Clearing all the above static structures and arrays.
	SET_ALL(_cmd,_arg_cmds,_respbox,~DUMMY_BYTE);
	
	// ++++++++++++++++++++++++++++++++++++++++++++++++ CMD8 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// argument prepration for CMD8
	_arg_cmds[2]=0x01;
	_arg_cmds[3]=0xAA;

	SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
	SD_Select();
	SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
	
	if(SendSD_Command(_cmd,CMD8,CMD_TYPE_R7,_arg_cmds,_respbox)==NULL){
		SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
		SD_Deselect();
		return 0x03;	// CMD8 send failed.
	}

	if((_respbox->r7)[0] != 0x01){
		SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
		SD_Deselect();
		for(uint8_t t=0;t<5;t++){
			vcom_printf("CMD8 response : r7[%d] : %d %#x\r\n",t,(_respbox->r7)[t], (_respbox->r7)[t]);
		}
		return 0x04;	// CMD8 response checking, not in idle state.
	}
	
	if( (_respbox->r7)[3] != VHS_CMD8_DEFAULT || (_respbox->r7)[4] != CMD8_CHECK_PATTERN_DEFAULT ){
		SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
		SD_Deselect();
		return 0x05;	// CMD8 check pattern echo failed or voltage acception failed.
	}
	vcom_printf("CMD8 completed !!!\r\n");
	SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
	SD_Deselect();

	// Clearing all the above static structures and arrays.
	SET_ALL(_cmd,_arg_cmds,_respbox,~DUMMY_BYTE);


	// ++++++++++++++++++++++++++++++++++++++++++++ CMD55-ACMD41 +++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//uint16_t counterA=0x0000;
	//uint16_t counterB=0x0000;
	SET_RESP(DUMMY_BYTE);	// to check the R1 response of CMD55, it should be 0x00 in order to proceed.

	for(uint8_t cnt=0;cnt<10;cnt++){
		SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
		SD_Select();
		SD_SendDummyBytes(HSPI_STRUCT_PTR,1);

		SET_ARG_CMDS(~DUMMY_BYTE);
		if(SendSD_Command(_cmd,CMD55,CMD_TYPE_R1,_arg_cmds,_respbox)==NULL){
			SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
			SD_Deselect();
			vcom_printf("UNKNOWN FAILURE:((((((\r\n");
			return 0x06;
		}
		vcom_printf("CMD55 response checking...\r\n");
		vcom_printf("CMD55 response : %d %#x\r\n",*(_respbox->r1),*(_respbox->r1));


		if(*(_respbox->r1)==0x01){	// ACMD41 execution condition checking.
			SET_RESP(DUMMY_BYTE);
			vcom_printf("Trying ACMD41...\r\n");
			// arg preparation for ACMD41
			_arg_cmds[0]=0x40;
			vcom_printf("Dumping ACMD41...\r\n");
			for(uint8_t num=0;num<6;num++){
				vcom_printf("ACMD41[%d] : %d %#x\r\n",num,((uint8_t*)_cmd)[num],((uint8_t*)_cmd)[num]);
			}

			if(SendSD_Command(_cmd,ACMD41,CMD_TYPE_R1,_arg_cmds,_respbox)==NULL){
				SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
				SD_Deselect();
				return 0x07;
			}
			if(*(_respbox->r1)!=0x00){
				vcom_printf("Unacceptable ACMD41 response.\r\n");
				vcom_printf("ACMD41 response : %d %#x\r\n",*(_respbox->r1),*(_respbox->r1));
				SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
				SD_Deselect();
				continue;

			}
			SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
			SD_Deselect();

			vcom_printf("ACMD41 response : %d %#x\r\n",*(_respbox->r1),*(_respbox->r1));
			break;
		}
		SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
		SD_Deselect();

	}


/*
	do{
		SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
		SD_Select();
		SD_SendDummyBytes(HSPI_STRUCT_PTR,1);

		if(SendSD_Command(_cmd,CMD55,CMD_TYPE_R1,_arg_cmds,_respbox)==NULL){
			SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
			SD_Deselect();
			return 0x06;
		}
		vcom_printf("CMD55 response checking...\r\n");
		vcom_printf("CMD55 response : %d %#x\r\n",*(_respbox->r1),*(_respbox->r1));

		if(*(_respbox->r1)==0x01){	// ACMD41 execution condition checking.
			SET_RESP(DUMMY_BYTE);
			// arg preparation for ACMD41
			_arg_cmds[0]=0x40;
			if(SendSD_Command(_cmd,ACMD41,CMD_TYPE_R1,_arg_cmds,_respbox)==NULL){
				SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
				SD_Deselect();
				return 0x07;
			}
			if(*(_respbox->r1)!=0x00){
				SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
				SD_Deselect();
				continue;
			}
			SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
			SD_Deselect();

			vcom_printf("ACMD41 response : %d %#x\r\n",*(_respbox->r1),*(_respbox->r1));

			break;
		}
		SD_SendDummyBytes(HSPI_STRUCT_PTR,1);
		SD_Deselect();

	}while(1U);

*/


	return 0x00;
}
