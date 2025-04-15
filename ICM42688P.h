/*
 * ICM42688P.h
 *
 *  Created on: Mar 6, 2025
 *      Author: Sezakiaoi
 */

#ifndef SRC_ICM42688P_H_
#define SRC_ICM42688P_H_

#include <cstdint>
#include <bitset>
#include <cmath>

class ICM42688P{

    public:

        enum class BANK0: uint8_t{

            ACCEL_DATA_X1 = 0x1F,
            PWR_MGMT0 = 0x4E,
            GYRO_CONFIG0 = 0x4F,
            ACCEL_CONFIG0 = 0x50,
            GYRO_CONFIG1 = 0x51,
            GYRO_ACCEL_CONFIG0 = 0x52,
            WHO_AM_I = 0x75,
        };

        enum class GYRO_MODE: uint8_t{

            OFF = 0x00,
            Standby = 0x01,
            LowNoize = 0x03
        };

        enum class GYRO_SCALE: uint8_t{

            Dps2000 = 0x00,
            Dps1000 = 0x01,
            Dps0500 = 0x02,
            Dps0250 = 0x03,
            Dps0125 = 0x04,
            Dps0062 = 0x05,
            Dps0031 = 0x06,
            Dps0015 = 0x07
        };

        enum class GYRO_ODR: uint8_t{

            ODR32000hz = 0x01,
            ODR16000hz = 0x02,
            ODR08000hz = 0x03,
            ODR04000hz = 0x04,
            ODR02000hz = 0x05,
            ODR01000hz = 0x06,
            ODR00200hz = 0x07,
            ODR00100hz = 0x08,
            ODR00050hz = 0x09,
            ODR00025hz = 0x0A,
            ODR00012hz = 0x0B,
            ODR00500hz = 0x0F
        };

        enum class GYRO_DLPF: uint8_t{

            ODR02 = 0x00,
            ODR04 = 0x01,
            ODR05 = 0x02,
            ODR08 = 0x03,
            ODR10 = 0x04,
            ODR16 = 0x05,
            ODR20 = 0x06,
            ODR40 = 0x07,
        };

        enum class ACCEL_Mode: uint8_t{

            OFF = 0x00,
            LowPower = 0x02,
            LowNoize = 0x03
        };

        enum class ACCEL_SCALE: uint8_t{

            SCALE16g = 0x00,
            SCALE08g = 0x01,
            SCALE04g = 0x02,
            SCALE02g = 0x03
        };

        enum class ACCEL_ODR: uint8_t{

            ODR32000hz = 0x01,
            ODR16000hz = 0x02,
            ODR08000hz = 0x03,
            ODR04000hz = 0x04,
            ODR02000hz = 0x05,
            ODR01000hz = 0x06,
            ODR00200hz = 0x07,
            ODR00100hz = 0x08,
            ODR00050hz = 0x09,
            ODR00025hz = 0x0A,
            ODR00012hz = 0x0B,
            ODR00006hz = 0x0C,
            ODR00003hz = 0x0D,
            ODR00001hz = 0x0E,
            ODR00500hz = 0x0F
        };

        enum class ACCEL_DLPF: uint8_t{

            ODR02 = 0x00,
            ODR04 = 0x01,
            ODR05 = 0x02,
            ODR08 = 0x03,
            ODR10 = 0x04,
            ODR16 = 0x05,
            ODR20 = 0x06,
            ODR40 = 0x07,
        };

        uint8_t Connection();
        uint8_t GetRawData(int16_t accel_buffer[3], int16_t gyro_buffer[3]);
        uint8_t GetData(float accel_buffer[3], float gyro_buffer[3]);
        uint8_t AccelConfig(ICM42688P::ACCEL_Mode accel_mode, ICM42688P::ACCEL_SCALE accel_scale, ICM42688P::ACCEL_ODR accel_odr, ICM42688P::ACCEL_DLPF accel_dlpf);
        uint8_t GyroConfig(ICM42688P::GYRO_MODE gyro_mode, ICM42688P::GYRO_SCALE gyro_scale, ICM42688P::GYRO_ODR gyro_odr, ICM42688P::GYRO_DLPF gyro_dlpf);
        uint8_t Calibration(uint16_t count);

    private:

        virtual void Write(ICM42688P::BANK0 reg_addr, uint8_t* tx_buffer, uint8_t len) = 0;
        virtual void Read(ICM42688P::BANK0 reg_addr, uint8_t* rx_buffer, uint8_t len) = 0;

        // Offset
        int16_t accel_offset[3] = {};
        int16_t gyro_offset[3] = {};

        // Config
        float accel_scale_value = 0.0;
        float gyro_scale_value  = 0.0;
        uint8_t accel_mode_tmp = 0;
        uint8_t gyro_mode_tmp = 0;
        uint8_t accel_dlpf_tmp = 0;
        uint8_t gyro_dlpf_tmp = 0;
        float accel_norm = 0.0;

        // Calc
        float g = 9.80665;
};

#endif /* SRC_ICM42688P_H_ */
