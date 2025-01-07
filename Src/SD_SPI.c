

















/**
 * @brief CreateCMD create a command to be used for data transaction.
 * @param cmd_format* cmd passes a pointer to the cmd_format structure which in turn gets populated with the subsequent arguments passed to the routine.
 * @param uint8_t command passes the command to be used in transaction.
 * @param uint32_t arg passes the argument for the command being used in transaction.
 * @retval void
 */
void CreateCMD(cmd_format* cmd, uint8_t command, uint32_t arg){
    for(uint8_t i=0;i<ARG_SIZE;i++){
        (cmd->ARG)[i]=((uint8_t*)(&arg))[ARG_SIZE-i]
    }
    cmd->CMD=(command<<2);
    (cmd->CMD)|=(((uint8_t*)(&arg))[4]>>6); /* merging last 2 bytes */ 
                                                                                                                
}




/**
 * @brief getCRC calculates the CRC7 of a given sequence.
 * @param uint8_t* addr passes the address of the data whose CRC7 has to be calculated.
 * @param uint16_t size passes the size in bytes (if not byte aligned, passed the corresponding higher byte) of the data whose CRC7 has to be calculated.
 * @retval uint8_t returns the 8 bit value whose lower 7 bits are CRC7.
 */
uint8_t getCRC7(uint8_t* addr, uint16_t size){
    uint8_t crc = 0; // Initial CRC value
    for (size_t i = 0; i < size; i++) {
        crc ^= addr[i]; // XOR current byte with CRC
        for (uint8_t bit = 0; bit < 8; bit++) { // Process each bit
            if (crc & 0x80) {
                crc = (crc << 1) ^ CRC7_POL; // XOR with polynomial
            } else {
                crc <<= 1; // Shift left
            }
        }
    }
    return (crc >> 1); // Return only the top 7 bits
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
