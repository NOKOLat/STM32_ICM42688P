/*
 * ICM42688P_HAL_SPI.cpp
 *
 *  Created on: Mar 15, 2025
 *      Author: Sezakiaoi
 */


#ifdef HAL_SPI_MODULE_ENABLED
#include "ICM42688P_HAL_SPI.h"

ICM42688P_HAL_SPI::ICM42688P_HAL_SPI(SPI_HandleTypeDef* spi_pin, GPIO_TypeDef* cs_pin_type, uint16_t cs_pin_num){

    //使用するピンの情報を受け取る
    this->spi_pin = spi_pin;
    this->cs_pin_type = cs_pin_type;
    this->cs_pin_num = cs_pin_num;

}

void ICM42688P_HAL_SPI::Write(ICM42688P::BANK0 reg_addr, uint8_t* tx_buffer, uint8_t len){

    uint8_t tx_tmp[2] = {};

    for(uint8_t i = 0; i < len; i++){

        tx_tmp[0] = ((uint8_t)reg_addr + i) | 0x00;
        tx_tmp[1] = tx_buffer[i];

        HAL_GPIO_WritePin(cs_pin_type, cs_pin_num, GPIO_PIN_RESET);

        HAL_SPI_Transmit(spi_pin, tx_tmp, 2, 10);

        HAL_GPIO_WritePin(cs_pin_type, cs_pin_num, GPIO_PIN_SET);

    }
}

void ICM42688P_HAL_SPI::Read(ICM42688P::BANK0 reg_addr, uint8_t* rx_buffer, uint8_t len){

    uint8_t rx_tmp[2] = {};
    uint8_t tx_tmp[2] = {};

    for(uint8_t i = 0; i < len; i++){

        tx_tmp[0] = (uint8_t(reg_addr) + i) | 0x80;
        tx_tmp[1] = 0x00;

        HAL_GPIO_WritePin(cs_pin_type, cs_pin_num, GPIO_PIN_RESET);

        HAL_SPI_TransmitReceive(spi_pin, tx_tmp, rx_tmp, 2, 10);

        HAL_GPIO_WritePin(cs_pin_type, cs_pin_num, GPIO_PIN_SET);

        rx_buffer[i] = rx_tmp[1];
    }
}
#endif
