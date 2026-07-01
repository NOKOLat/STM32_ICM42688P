/*
 * ICM42688P.cpp
 *
 *  Created on: Mar 6, 2025
 *      Author: Sezakiaoi
 */

//#include "stdio.h"
#include "ICM42688P.h"

namespace{

constexpr float STANDARD_GRAVITY = 9.80665f;
constexpr uint8_t DATA_READY_MASK = 0x08;

}

/* @brief コンストラクタ
 *
 * @param [in]Write レジスタに値を書き込む関数
 * @param [in]Read  レジスタから値を読み取る関数
 * @param [in]log   ログを出力する関数
 */
ICM42688P::ICM42688P(uint8_t (*Write)(uint8_t reg_addr, uint8_t* tx_buffer, uint8_t len), uint8_t (*Read)(uint8_t reg_addr, uint8_t* rx_buffer, uint8_t len), void (*log)(const char* msg)){

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

    accel_scale_value = (16.0 / pow(2, (uint8_t)accel_scale)) / 32768 * STANDARD_GRAVITY;

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

/* @brief 新しいセンサーデータの有無を確認
 *
 * INT_STATUSのDATA_RDY_INTを読み取ります
 *
 * @param [out]is_ready 新しいデータがある場合はtrue
 *
 * @return uint8_t 成功: 0、失敗: 1
 */
uint8_t ICM42688P::CheckDataReady(bool& is_ready){

    uint8_t int_status = 0;
    if(Read((uint8_t)ICM42688P::BANK0::INT_STATUS, &int_status, 1) != 0){

        is_ready = false;
        return 1;
    }

    is_ready = (int_status & DATA_READY_MASK) != 0;

    return 0;
}

/* @brief 加速度センサーとジャイロセンサーからデータを取得
 *
 * xyzの順番で配列に値がはいってきます
 * センサ読み取りに失敗するとReturn 1する
 *
 * @param [out]int16_t Accel_Data[3] 加速度データを入れる配列
 * @param [out]int16_t Gyro_Data[3]  角速度データを入れる配列
 *
 * @return uint8_t 成功: 0、失敗: 1
 */
uint8_t ICM42688P::GetRawData(int16_t accel_buffer[3], int16_t gyro_buffer[3]){

    if(!accel_buffer || !gyro_buffer){

        return 1;
    }

    uint8_t raw_data[12] = {};
    if(Read((uint8_t)ICM42688P::BANK0::ACCEL_DATA_X1, raw_data, 12) != 0){

        return 1;
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

    if(!accel_data || !gyro_data){

        return 1;
    }

    int16_t accel_buffer[3] = {};
    int16_t gyro_buffer[3] = {};

    if(GetRawData(accel_buffer, gyro_buffer) != 0){

        return 1;
    }

    for(uint8_t i = 0; i < 3; i++){

        accel_data[i] = (float)(accel_buffer[i] *accel_scale_value);
        gyro_data[i]  = (float)(gyro_buffer[i] * gyro_scale_value);
    }

    return 0;
}

/* @brief キャリブレーションを開始
 *
 * @param [in]required_count キャリブレーションに必要なデータ数
 *
 * @return uint8_t 成功: 0、失敗: 1
 */
uint8_t ICM42688P::StartCalibration(uint16_t required_count){

    if(required_count == 0 || !(accel_scale_value > 0.0f)){

        return 1;
    }

    calibration_sample_count = 0;
    calibration_required_count = required_count;
    calibration_complete = false;

    for(uint8_t i = 0; i < 3; i++){

        accel_offset[i] = 0;
        gyro_offset[i] = 0;
        calibration_accel_sum[i] = 0;
        calibration_gyro_sum[i] = 0;
    }

    return 0;
}

/* @brief キャリブレーション用のデータを追加
 *
 * 必要数のデータが集まると、平均値からオフセットを計算します
 *
 * @param [in]accel_raw 加速度センサーの生データ
 * @param [in]gyro_raw  ジャイロセンサーの生データ
 *
 * @return uint8_t 成功: 0、失敗: 1
 */
uint8_t ICM42688P::AddCalibrationData(const int16_t accel_raw[3], const int16_t gyro_raw[3]){

    if(!accel_raw || !gyro_raw || calibration_required_count == 0 || calibration_complete){

        return 1;
    }

    for(uint8_t i = 0; i < 3; i++){

        calibration_accel_sum[i] += accel_raw[i];
        calibration_gyro_sum[i] += gyro_raw[i];
    }

    calibration_sample_count ++;
    if(calibration_sample_count < calibration_required_count){

        return 0;
    }

    for(uint8_t i = 0; i < 3; i++){

        accel_offset[i] = calibration_accel_sum[i] / calibration_required_count;
        gyro_offset[i] = calibration_gyro_sum[i] / calibration_required_count;
    }

    int16_t gravity_count = (int16_t)(STANDARD_GRAVITY / accel_scale_value);
    accel_offset[2] -= gravity_count;
    calibration_complete = true;

    return 0;
}

/* @brief キャリブレーション完了状態を取得
 *
 * @return bool 必要数のデータが集まった場合はtrue
 */
bool ICM42688P::IsCalibrationComplete() const{

    return calibration_complete;
}
