#ifndef INC_ICM42688P_WIRE_I2C_H_
#define INC_ICM42688P_WIRE_I2C_H_

#include "ICM42688P.h"
#include <Wire.h>

class ICM42688P_WIRE_I2C: public ICM42688P{

	public:

		ICM42688P_WIRE_I2C(uint8_t SDA_pin, uint8_t SCL_pin);

	private:

		void Write(ICM42688P::BANK0 reg_addr, uint8_t* TxBuffer, uint8_t Len);
		void Read(ICM42688P::BANK0 reg_addr, uint8_t* RxBuffer, uint8_t Len);

		uint8_t SDA_pin = 21;
    	uint8_t SCL_pin = 22;
		uint8_t i2c_addr = 0b1101000;

    	uint8_t single_byte = 1;
};















#endif