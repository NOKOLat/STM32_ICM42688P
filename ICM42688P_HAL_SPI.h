/*
 * ICM42688P_HAL_SPI.h
 *
 *  Created on: Mar 15, 2025
 *      Author: Sezakiaoi
 */

 #ifndef INC_ICM42688P_HAL_SPI_H_
 #define INC_ICM42688P_HAL_SPI_H_
 

#ifdef HAL_SPI_MODULE_ENABLED
#include "ICM42688P.h"
#include "spi.h"
#include "gpio.h"
 
 class ICM42688P_HAL_SPI: public ICM42688P{
 
	 public:
 
		 ICM42688P_HAL_SPI(SPI_HandleTypeDef* spi_pin, GPIO_TypeDef* cs_pin_type, uint16_t cs_pin_num);
 
	 private:
 
		 void Write(ICM42688P::BANK0 reg_addr, uint8_t* tx_buffer, uint8_t len) override;
		 void Read(ICM42688P::BANK0 reg_addr, uint8_t* rx_buffer, uint8_t len) override;
 
		 SPI_HandleTypeDef* spi_pin;
		 GPIO_TypeDef* cs_pin_type;
		 uint16_t cs_pin_num = 0x00;
 };
 
 #endif /* INC_ICM42688P_HAL_SPI_H_ */
#endif /* HAL_SPI_MODULE_ENABLED */