/*
 * ICM42688P.cpp
 *
 *  Created on: Mar 6, 2025
 *      Author: Sezakiaoi
 */

#include "ICM42688P.h"

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

        Read(ICM42688P::BANK0::WHO_AM_I, &product_id, 1);
        error ++;

        if(error > 100){
            return 1;//接続失敗
        }
    }
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

        Write(ICM42688P::BANK0::PWR_MGMT0, &command, 1);
        Read(ICM42688P::BANK0::PWR_MGMT0, &now_mode, 1);

        error ++;
        if(error > 100){
            return 1;
        }
    }

    command = (uint8_t)accel_scale << 5 | (uint8_t)accel_odr;

    error = 0;
    now_mode = 0;
    while(command != now_mode){

        Write(ICM42688P::BANK0::ACCEL_CONFIG0, &command, 1);
        Read(ICM42688P::BANK0::ACCEL_CONFIG0, &now_mode, 1);

        error ++;
        if(error > 100){
            return 2;
        }
    }

    accel_dlpf_tmp = (uint8_t)accel_dlpf;

    command = gyro_dlpf_tmp | (accel_dlpf_tmp << 4);

    error = 0;
    now_mode = 0;
    while(command != now_mode){

        Write(ICM42688P::BANK0::GYRO_ACCEL_CONFIG0, &command, 1);
        Read(ICM42688P::BANK0::GYRO_ACCEL_CONFIG0, &now_mode, 1);

        error ++;
        if(error > 100){
            return 3;
        }
    }

    accel_scale_value = 16.0 / pow(2,(uint8_t)accel_scale);

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

        Write(ICM42688P::BANK0::PWR_MGMT0, &command, 1);
        Read(ICM42688P::BANK0::PWR_MGMT0, &now_mode, 1);

        error ++;
        if(error > 100){
            return 1;
        }
    }

    command = (uint8_t)gyro_scale << 5 | (uint8_t)gyro_odr;

    now_mode = 0;
    error = 0;
    while(command != now_mode){

        Write(ICM42688P::BANK0::GYRO_CONFIG0, &command, 1);
        Read(ICM42688P::BANK0::GYRO_CONFIG0, &now_mode, 1);

        error ++;
        if(error > 100){
            return 2;
        }
    }

    gyro_dlpf_tmp = (uint8_t)gyro_dlpf;

    command = gyro_dlpf_tmp | (accel_dlpf_tmp << 4);

    error = 0;
    now_mode = 0;
    while(command != now_mode){

        Write(ICM42688P::BANK0::GYRO_ACCEL_CONFIG0, &command, 1);
        Read(ICM42688P::BANK0::GYRO_ACCEL_CONFIG0, &now_mode, 1);

        error ++;
        if(error > 100){
            return 3;
        }
    }

    gyro_scale_value = 2000.0 / pow(2, (uint8_t)gyro_scale);

    return 0;
}

/* @brief 加速度センサーとジャイロセンサーからデータを取得
 *
 * xyzの順番で配列に値がはいってきます
 * 200回実行に失敗する（2000ms程度）とReturn 1する
 * ODRに対して実行が早い場合は同じ値を何度も取得することになります
 *
 * @param [out]int16_t Accel_Data[3] 加速度データを入れる配列
 * @param [out]int16_t Gyro_Data[3]  角速度データを入れる配列
 *
 * @return uint8_t 成功: 0、失敗: 1

 */
uint8_t ICM42688P::GetRawData(int16_t accel_buffer[3], int16_t gyro_buffer[3]){

    uint8_t raw_data[12] = {};
    uint8_t error = 0;
    while(raw_data[1] == 0 && raw_data[3] == 0 && raw_data[5] == 0){

        Read(ICM42688P::BANK0::ACCEL_DATA_X1, raw_data, 12);

        error ++;
        if(error > 200){
            return 1;
        }
    }

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

    if(GetRawData(accel_buffer, gyro_buffer) == 1){
        return 1;
    }

    for(uint8_t i = 0; i < 3; i++){
        accel_data[i] = accel_buffer[i] * g * accel_scale_value / 32768.0;
        gyro_data[i]  = gyro_buffer[i] * gyro_scale_value / 32768.0;
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
uint8_t ICM42688P::Calibration(uint16_t count){

    int16_t accel[3] = {};
    int16_t gyro[3] = {};

    int32_t accel_tmp[3] = {};
    int32_t gyro_tmp[3] = {};

	//空取得を行う（センサーの待機用）
	int16_t dummy[3] = {};
	for(int16_t i = 0; i < 100; i++){

		ICM42688P::GetRawData(dummy, dummy);
	}

    for(int16_t i = 0; i < count; i++){

        if(ICM42688P::GetRawData(accel, gyro) == 1){
            return 1;
        }

        for(uint8_t j = 0; j < 3; j++){
            accel_tmp[j] += accel[j];
            gyro_tmp[j] += gyro[j];
        }

        for(uint32_t k = 0; k < 25000; k++);
    }

    for(uint8_t k = 0; k < 3; k++){
        accel_offset[k] = accel_tmp[k] / count;
        gyro_offset[k] = gyro_tmp[k] / count;
    }

    accel_offset[2] -= 32768 / accel_scale_value;

    return 0;
}
