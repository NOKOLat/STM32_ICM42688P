/*
 * ICM42688P_HAL_I2C.cpp
 *
 *  Created on: Mar 16, 2025
 *      Author: Sezakiaoi
 */

#include "ICM42688P_HAL_I2C.h"

ICM42688P_HAL_I2C::ICM42688P_HAL_I2C(I2C_HandleTypeDef* i2c_pin){
    this->i2c_pin = i2c_pin;
}

void ICM42688P_HAL_I2C::Write(ICM42688P::BANK0 reg_addr, uint8_t* tx_buffer, uint8_t len){
    HAL_I2C_Mem_Write(i2c_pin, i2c_addr, uint8_t(reg_addr), 1, tx_buffer, len, 1);
}

void ICM42688P_HAL_I2C::Read(ICM42688P::BANK0 reg_addr, uint8_t* rx_buffer, uint8_t len){
    HAL_I2C_Mem_Read(i2c_pin, i2c_addr, uint8_t(reg_addr), 1, rx_buffer, len, 1);
}
