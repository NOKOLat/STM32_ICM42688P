/*
 * ICM42688P_HAL_SPI.cpp
 *
 *  Created on: Mar 15, 2025
 *      Author: Sezakiaoi
 */

#include <ICM42688P_HAL_SPI.h>

ICM42688P_HAL_SPI::ICM42688P_HAL_SPI(SPI_HandleTypeDef* SpiPin, GPIO_TypeDef* CsPinType, uint16_t CsPinNum){

	//使用するピンの情報を受け取る
	this->SpiPin = SpiPin;
	this->CsPinType = CsPinType;
	this->CsPinNum = CsPinNum;

}

void ICM42688P_HAL_SPI::Write(ICM42688P::BANK0 REGISTER_ADDR, uint8_t* TxBuffer, uint8_t Len){

	uint8_t TxTmp[2] = {};

	for(uint8_t i=0; i<Len; i++){

	TxTmp[0] = ((uint8_t)REGISTER_ADDR + i) | 0x00;
	TxTmp[1] = TxBuffer[i];

	HAL_GPIO_WritePin(CsPinType, CsPinNum, GPIO_PIN_RESET);

    HAL_SPI_Transmit(SpiPin, TxTmp, 2, 10);

    HAL_GPIO_WritePin(CsPinType, CsPinNum, GPIO_PIN_SET);

	}
}

void ICM42688P_HAL_SPI::Read(ICM42688P::BANK0 REGISTER_ADDR, uint8_t* RxBuffer, uint8_t Len){

	uint8_t RxTmp[2] = {};
	uint8_t TxTmp[2] = {};

	for(uint8_t i=0; i<Len; i++){

		TxTmp[0] = (uint8_t(REGISTER_ADDR) + i) | 0x80;
		TxTmp[1] = 0x00;

		HAL_GPIO_WritePin(CsPinType, CsPinNum, GPIO_PIN_RESET);

		HAL_SPI_TransmitReceive(SpiPin, TxTmp, RxTmp, 2, 10);

		HAL_GPIO_WritePin(CsPinType, CsPinNum, GPIO_PIN_SET);

		RxBuffer[i] = RxTmp[1];
	}
}
