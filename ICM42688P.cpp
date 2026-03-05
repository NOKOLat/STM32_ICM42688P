/*
 * ICM42688P.cpp
 *
 *  Created on: Mar 6, 2025
 *      Author: Sezakiaoi
 */

//#include "stdio.h"
#include "ICM42688P.h"

/* @brief コンストラクタ
 *
 * @param [in]Write レジスタに値を書き込む関数
 * @param [in]Read  レジスタから値を読み取る関数
 * @param [in]log   ログを出力する関数
 */
ICM42688P::ICM42688P(uint8_t (*Write)(uint8_t reg_addr, uint8_t* tx_buffer, uint8_t len), uint8_t (*Read)(uint8_t reg_addr, uint8_t* rx_buffer, uint8_t len), void (*log)(char* msg)){

    this->Write = Write;
    this->Read = Read;
    this->log = log;
}

/* @brief センサーとの接続を確認
 *
 * ICM42688PのWHO_AM_Iレジスタとの通信を用いて、接続を確認します
 * 最大100回の再試行をします
 *
 * @return uint8_t 0: 接続成功、1: 接続失敗
 */
uint8_t ICM42688P::Connection(){

    uint8_t product_id = 0x00;
    uint8_t error = 0;

    while(product_id != 0x47){

        Read((uint8_t)ICM42688P::BANK0::WHO_AM_I, &product_id, 1);
        error ++;

        if(error > 100){

            log("[ICM42688P] Not Found\n");
            return 1;//接続失敗
        }
    }

    log("[ICM42688P] Connection Successful\n");
    
    return 0;//接続成功
}

/* @brief 加速度センサーの設定
 *
 * 詳しい説明はデータシート参照
 * @param [in]ICM42688P::ACCEL_Mode   加速度センサーのモード
 * @param [in]ICM42688P::ACCEL_SCALE  センサーの測定スケール
 * @param [in]ICM42688P::ACCEL_ODR    出力レート
 * @param [in]ICM42688P::ACCEL_DLPF   DLPFの設定
 *
 * @return uint8_t 0: 設定完了 1: PWR_MGMT0 設定失敗 2: ACCEL_CONFIG0　設定失敗 3: GYRO_ACCEL_CONFIG0 設定失敗
 */
uint8_t ICM42688P::AccelConfig(ICM42688P::ACCEL_Mode accel_mode, ICM42688P::ACCEL_SCALE accel_scale, ICM42688P::ACCEL_ODR accel_odr, ICM42688P::ACCEL_DLPF accel_dlpf){

    accel_mode_tmp = (uint8_t)accel_mode;

    uint8_t command = (gyro_mode_tmp << 2) | accel_mode_tmp;

    uint8_t error = 0;
    uint8_t now_mode = 0;
    while(command != now_mode){

        Write((uint8_t)ICM42688P::BANK0::PWR_MGMT0, &command, 1);
        Read((uint8_t)ICM42688P::BANK0::PWR_MGMT0, &now_mode, 1);

        error ++;
        if(error > 100){

            log("[ICM42688P] AccelConfig: PWR_MGMT0 Setting Failed\n");
            return 1;
        }
    }

    command = (uint8_t)accel_scale << 5 | (uint8_t)accel_odr;

    error = 0;
    now_mode = 0;
    while(command != now_mode){

        Write((uint8_t)ICM42688P::BANK0::ACCEL_CONFIG0, &command, 1);
        Read((uint8_t)ICM42688P::BANK0::ACCEL_CONFIG0, &now_mode, 1);

        error ++;
        if(error > 100){

            log("[ICM42688P] AccelConfig: ACCEL_CONFIG0 Setting Failed\n");
            return 2;
        }
    }

    accel_dlpf_tmp = (uint8_t)accel_dlpf;

    command = gyro_dlpf_tmp | (accel_dlpf_tmp << 4);

    error = 0;
    now_mode = 0;
    while(command != now_mode){

        Write((uint8_t)ICM42688P::BANK0::GYRO_ACCEL_CONFIG0, &command, 1);
        Read((uint8_t)ICM42688P::BANK0::GYRO_ACCEL_CONFIG0, &now_mode, 1);

        error ++;
        if(error > 100){

            log("[ICM42688P] AccelConfig: GYRO_ACCEL_CONFIG0 Setting Failed\n");
            return 3;
        }
    }

    accel_scale_value = (16.0 / pow(2,(uint8_t)accel_scale) ) / 32768 * 9.8;

    return 0;
}

/* @brief ジャイロセンサーの設定
 *
 * 詳しい説明はデータシート参照
 * @param [in]ICM42688P::GYRO_Mode   ジャイロセンサーのモード
 * @param [in]ICM42688P::GYRO_SCALE  センサーの測定スケール
 * @param [in]ICM42688P::GYRO_ODR    出力レート
 * @param [in]ICM42688P::GYRO_DLPF   DLPFの設定
 *
 *  @return uint8_t 0: 設定完了 1: PWR_MGMT0 設定失敗 2: Gyro_CONFIG0　設定失敗 3: GYRO_ACCEL_CONFIG0 設定失敗
 */
uint8_t ICM42688P::GyroConfig(ICM42688P::GYRO_MODE gyro_mode, ICM42688P::GYRO_SCALE gyro_scale, ICM42688P::GYRO_ODR gyro_odr, ICM42688P::GYRO_DLPF gyro_dlpf){

    gyro_mode_tmp = (uint8_t)gyro_mode;

    uint8_t command = (gyro_mode_tmp << 2) | accel_mode_tmp;

    uint8_t error = 0;
    uint8_t now_mode = 0;
    while(command != now_mode){

        Write((uint8_t)ICM42688P::BANK0::PWR_MGMT0, &command, 1);
        Read((uint8_t)ICM42688P::BANK0::PWR_MGMT0, &now_mode, 1);

        error ++;
        if(error > 100){

            log("[ICM42688P] GyroConfig: PWR_MGMT0 Setting Failed\n");
            return 1;
        }
    }

    command = (uint8_t)gyro_scale << 5 | (uint8_t)gyro_odr;

    now_mode = 0;
    error = 0;
    while(command != now_mode){

        Write((uint8_t)ICM42688P::BANK0::GYRO_CONFIG0, &command, 1);
        Read((uint8_t)ICM42688P::BANK0::GYRO_CONFIG0, &now_mode, 1);

        error ++;
        if(error > 100){

            log("[ICM42688P] GyroConfig: GYRO_CONFIG0 Setting Failed\n");
            return 2;
        }
    }

    gyro_dlpf_tmp = (uint8_t)gyro_dlpf;

    command = gyro_dlpf_tmp | (accel_dlpf_tmp << 4);

    error = 0;
    now_mode = 0;
    while(command != now_mode){

        Write((uint8_t)ICM42688P::BANK0::GYRO_ACCEL_CONFIG0, &command, 1);
        Read((uint8_t)ICM42688P::BANK0::GYRO_ACCEL_CONFIG0, &now_mode, 1);

        error ++;
        if(error > 100){

            log("[ICM42688P] GyroConfig: GYRO_ACCEL_CONFIG0 Setting Failed\n");
            return 3;
        }
    }

    gyro_scale_value = 2000.0 / pow(2, (uint8_t)gyro_scale) / 32768.0;

    return 0;
}

/* @brief 加速度センサーとジャイロセンサーからデータを取得
 *
 * xyzの順番で配列に値がはいってきます
 * 100回実行に失敗するとReturn 1する
 * ODRに対して実行が早い場合は同じ値を何度も取得することになります
 *
 * @param [out]int16_t Accel_Data[3] 加速度データを入れる配列
 * @param [out]int16_t Gyro_Data[3]  角速度データを入れる配列
 *
 * @return uint8_t 成功: 0、失敗: 1
 */
uint8_t ICM42688P::GetRawData(int16_t accel_buffer[3], int16_t gyro_buffer[3]){

    uint8_t raw_data[12];

    Read((uint8_t)ICM42688P::BANK0::ACCEL_DATA_X1, raw_data, 12);

    accel_buffer[0]  = (int16_t)(raw_data[1] | (raw_data[0] << 8)) - accel_offset[0];
    accel_buffer[1]  = (int16_t)(raw_data[3] | (raw_data[2] << 8)) - accel_offset[1];
    accel_buffer[2]  = (int16_t)(raw_data[5] | (raw_data[4] << 8)) - accel_offset[2];

    gyro_buffer[0]  = (int16_t)(raw_data[7]  | raw_data[6]  << 8) - gyro_offset[0];
    gyro_buffer[1]  = (int16_t)(raw_data[9]  | raw_data[8]  << 8) - gyro_offset[1];
    gyro_buffer[2]  = (int16_t)(raw_data[11] | raw_data[10] << 8) - gyro_offset[2];
	
    return 0;
}

/* @brief 加速度センサーとジャイロセンサーからデータを取得
 *
 * XYZの順番でデータが入ります
 * 単位は加速度(m/s^2)、加速度(dps)になっています
 *
 * @param [out]float Accel_Data[3] 加速度データを入れる配列
 * @param [out]float Gyro_Data[3]  角速度データを入れる配列
 *
 * @return uint8_t 成功: 0、失敗: 1
 */
uint8_t ICM42688P::GetData(float accel_data[3], float gyro_data[3]){

    int16_t accel_buffer[3] = {};
    int16_t gyro_buffer[3] = {};

    GetRawData(accel_buffer, gyro_buffer);

    for(uint8_t i = 0; i < 3; i++){

        accel_data[i] = (float)(accel_buffer[i] *accel_scale_value);
        gyro_data[i]  = (float)(gyro_buffer[i] * gyro_scale_value);
    }

    return 0;
}

/* @brief キャリブレーション
 *
 * GetRawData関数を使用して、指定回数値を取得し
 * その値をオフセットとして登録します
 *
 * @param  [in]Count キャリブレーションのデータ取得回数
 *
 * @return uint8_t 成功: 0、失敗: 1
 */
uint8_t ICM42688P::Calibration(uint16_t Count){

	int32_t accel_tmp[3] = {};
	int32_t gyro_tmp[3] = {};

	int16_t accel_raw[3];
	int16_t gyro_raw[3];

	for(uint16_t i=0; i<Count; i++){

		GetRawData(accel_raw, gyro_raw);

		for(uint8_t j=0; j<3; j++){

			accel_tmp[j] += accel_raw[j];
			gyro_tmp[j] += gyro_raw[j];
		}
	}

	for(uint8_t i=0; i<3; i++){

		accel_offset[i] = accel_tmp[i] / Count;
		gyro_offset[i] = gyro_tmp[i] / Count;
	}

	accel_offset[2] -= 32768 / accel_scale_value * 32768 * 9.8;

	return 0;

}
