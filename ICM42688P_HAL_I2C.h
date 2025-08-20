/*
 * ICM42688P_HAL_I2C.h
 *
 *  Created on: Mar 16, 2025
 *      Author: Sezakiaoi
 */

#ifndef SRC_ICM42688P_HAL_I2C_H_
#define SRC_ICM42688P_HAL_I2C_H_

#ifdef HAL_I2C_MODULE_ENABLED

#include "ICM42688P.h"
#include "i2c.h"

class ICM42688P_HAL_I2C: public ICM42688P {

    public:

        ICM42688P_HAL_I2C(I2C_HandleTypeDef* i2c_pin);

    private:

        void Write(ICM42688P::BANK0 reg_addr, uint8_t* tx_buffer, uint8_t len) override;
        void Read(ICM42688P::BANK0 reg_addr, uint8_t* rx_buffer, uint8_t len) override;

        I2C_HandleTypeDef* i2c_pin;
        const uint8_t i2c_addr = 0b1101000 << 1;
};

#endif /* SRC_ICM42688P_HAL_I2C_H_ */
#endif /* HAL_I2C_MODULE_ENABLED */
