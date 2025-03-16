/*
 * ICM42688P_HAL_I2C.h
 *
 *  Created on: Mar 16, 2025
 *      Author: Sezakiaoi
 */

#ifndef SRC_ICM42688P_HAL_I2C_H_
#define SRC_ICM42688P_HAL_I2C_H_

#include "ICM42688P.h"
#include "i2c.h"

class ICM42688P_HAL_I2C: public ICM42688P {


	public:

		ICM42688P_HAL_I2C(I2C_HandleTypeDef* I2cPin);

	private:

		void Write(ICM42688P::BANK0, uint8_t* TxBuffer, uint8_t Len);
		void Read(ICM42688P::BANK0, uint8_t* RxBuffer, uint8_t Len);

		I2C_HandleTypeDef* I2cPin;
		const uint8_t I2C_ADDR = 0b1101000 << 1;
};

#endif /* SRC_ICM42688P_HAL_I2C_H_ */
