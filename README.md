# SanDisk-SD-card-SPI-Driver
This driver can be used to interact with the SD cards (SanDisk specially but you can try using with others if hardware doesn't differs that much) over SPI.
This driver is written on top of HAL library provided by specific micro-controller, this driver is specially written for STM32 devices supporting included HAL routines.

This library aims cards of all capacities like SDSC, SDHC, SDXC etc.

Usage of this library : 

initialization routines ------> IO/IOCTL routines ------> de-initialization routines


Programmer must initialize the HAL library and include any SPI relevant header file to ensure functionality of this driver.
