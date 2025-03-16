/*
 * ICM42688P_HAL_I2C.cpp
 *
 *  Created on: Mar 16, 2025
 *      Author: Sezakiaoi
 */

#include "ICM42688P_HAL_I2C.h"

ICM42688P_HAL_I2C::ICM42688P_HAL_I2C(I2C_HandleTypeDef* I2cPin){

	this->I2cPin = I2cPin;
}

void ICM42688P_HAL_I2C::Write(ICM42688P::BANK0 REGISTER_ADDR, uint8_t* TxBuffer, uint8_t Len){

	HAL_I2C_Mem_Write(I2cPin, I2C_ADDR, uint8_t(REGISTER_ADDR), 1, TxBuffer, Len, 1);
}

void ICM42688P_HAL_I2C::Read(ICM42688P::BANK0 REGISTER_ADDR, uint8_t* RxBuffer, uint8_t Len){

	HAL_I2C_Mem_Read(I2cPin, I2C_ADDR, uint8_t(REGISTER_ADDR), 1, RxBuffer, Len, 1);
}
