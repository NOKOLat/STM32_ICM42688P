/*
 * ICM42688P_HAL_SPI.h
 *
 *  Created on: Mar 15, 2025
 *      Author: Sezakiaoi
 */

#ifndef INC_ICM42688P_HAL_SPI_H_
#define INC_ICM42688P_HAL_SPI_H_

#include "ICM42688P.h"
#include "spi.h"
#include "gpio.h"

class ICM42688P_HAL_SPI: public ICM42688P{

	public:

		ICM42688P_HAL_SPI(SPI_HandleTypeDef* SpiPin, GPIO_TypeDef* CsPinType, uint16_t CsPinNum);

	private:

		void Write(ICM42688P::BANK0, uint8_t* TxBuffer, uint8_t Len);
		void Read(ICM42688P::BANK0, uint8_t* RxBuffer, uint8_t Len);

		SPI_HandleTypeDef* SpiPin;
		GPIO_TypeDef* CsPinType;
		uint16_t CsPinNum = 0x00;
};

#endif /* INC_ICM42688P_HAL_SPI_H_ */
