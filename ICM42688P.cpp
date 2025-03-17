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

	SelectBank(0);

	uint8_t ProductId = 0x00;
	uint8_t Error = 0;

	while(ProductId != 0x47){

		Read(ICM42688P::BANK0::WHO_AM_I, &ProductId, 1);
		Error ++;

		if(Error > 100){

			return 1;//接続失敗
		}
	}
	return 0;//接続成功
}

/* @brief 加速度センサーの設定
 *
 * 詳しい説明はデータシート参照
 * @param ICM42688P::ACCEL_Mode   加速度センサーのモード
 * @param ICM42688P::ACCEL_SCALE  センサーの測定スケール
 * @param ICM42688P::ACCEL_ODR    出力レート
 * @param ICM42688P::ACCEL_DLPF   DLPFの設定
 *
 * @return uint8_t 0: 設定完了
 */
uint8_t ICM42688P::AccelConfig(ICM42688P::ACCEL_Mode Mode, ICM42688P::ACCEL_SCALE Scale, ICM42688P::ACCEL_ODR ODR, ICM42688P::ACCEL_DLPF DLPF){

	//現在のモードを取得
	uint8_t NowMode = 0x00;
	Read(ICM42688P::BANK0::PWR_MGMT0, &NowMode, 1);

	//Mode設定を上書き＋書き込み
	uint8_t Command = (NowMode & 0xFC) | (uint8_t)Mode;
	Write(ICM42688P::BANK0::PWR_MGMT0, &Command, 1);

	//ScaleとODRの設定を書き込み
	Command = (uint8_t)Scale << 5 | (uint8_t)ODR;
	Write(ICM42688P::BANK0::ACCEL_CONFIG0, &Command, 1);

	//現在のDLFPの値を取得
	Read(ICM42688P::BANK0::GYRO_ACCEL_CONFIG0, &NowMode, 1);

	//DLPFの設定を上書き
	Command = (NowMode & 0x0F) | (uint8_t)Mode << 4;
	Write(ICM42688P::BANK0::GYRO_ACCEL_CONFIG0, &Command, 1);

	//Scaleの値を保存
	AccelScaleValue = 16.0 / pow(2,(uint8_t)Scale);

	return 0;
}

/* @brief ジャイロセンサーの設定
 *
 * 詳しい説明はデータシート参照
 * @param ICM42688P::GYRO_Mode   ジャイロセンサーのモード
 * @param ICM42688P::GYRO_SCALE  センサーの測定スケール
 * @param ICM42688P::GYRO_ODR    出力レート
 * @param ICM42688P::GYRO_DLPF   DLPFの設定
 *
 * @return uint8_t 0: 設定完了
 */
uint8_t ICM42688P::GyroConfig(ICM42688P::GYRO_MODE Mode, ICM42688P::GYRO_SCALE Scale, ICM42688P::GYRO_ODR ODR, ICM42688P::GYRO_DLPF DLPF){

	//現在のモードを取得
	uint8_t NowMode = 0x00;
	Read(ICM42688P::BANK0::PWR_MGMT0, &NowMode, 1);

	//Mode設定を上書き＋書き込み
	uint8_t Command = (NowMode & 0xF3) | (uint8_t)Mode << 2;
	Write(ICM42688P::BANK0::PWR_MGMT0, &Command, 1);

	//ScaleとODRの設定を書き込み
	Command = (uint8_t)Scale << 5 | (uint8_t)ODR;
	Write(ICM42688P::BANK0::GYRO_CONFIG0, &Command, 1);

	//現在のDLFPの値を取得
	Read(ICM42688P::BANK0::GYRO_ACCEL_CONFIG0, &NowMode, 1);

	//DLPFの設定を上書き
	Command = (NowMode & 0xF0) | (uint8_t)Mode;
	Write(ICM42688P::BANK0::GYRO_ACCEL_CONFIG0, &Command, 1);

	GyroScaleValue = 2000.0 / pow(2, (uint8_t)Scale);
	return 0;
}

/* @brief 加速度センサーとジャイロセンサーからデータを取得
 *
 * -32768 ~ +32768の値が返ってきます
 *
 * @param int16_t Accel_Data[3] 加速度データを入れる配列
 * @param int16_t Gyro_Data[3]  角速度データを入れる配列
 *
 * @return uint8_t 常に0を返します。取得失敗時は前回と同じ値を返します。
 */
uint8_t ICM42688P::GetRawData(int16_t AccelData[3], int16_t GyroData[3]){

	Read(ICM42688P::BANK0::ACCEL_DATA_X1, RawData, 12);

	AccelData[0]  = (int16_t)(RawData[1] | RawData[0] << 8) - AccelOffset[0];
	AccelData[1]  = (int16_t)(RawData[3] | RawData[2] << 8) - AccelOffset[1];
	AccelData[2]  = (int16_t)(RawData[5] | RawData[4] << 8) - AccelOffset[2];

	GyroData[0]  = (int16_t)(RawData[7]  | RawData[6]  << 8) - GyroOffset[0];
	GyroData[1]  = (int16_t)(RawData[9]  | RawData[8]  << 8) - GyroOffset[1];
	GyroData[2]  = (int16_t)(RawData[11] | RawData[10] << 8) - GyroOffset[2];

	return 0;
}

/* @brief 加速度センサーとジャイロセンサーからデータを取得
 *
 * 加速度(m/s^2)、加速度(dps)
 *
 * @param float Accel_Data[3] 加速度データを入れる配列
 * @param float Gyro_Data[3]  角速度データを入れる配列
 *
 * @return uint8_t 常に0を返します。取得失敗時は前回と同じ値を返します。
 */
uint8_t ICM42688P::GetData(float AccelData[3], float GyroData[3]){

	int16_t AccelBuffer[3] = {};
	int16_t GyroBuffer[3] = {};

	GetRawData(AccelBuffer, GyroBuffer);

	for(uint8_t i=0; i<3; i++){

		AccelData[i] = AccelBuffer[i] * G * AccelScaleValue / 32768.0;
		GyroData[i]  = GyroBuffer[i] * GyroScaleValue / 32768.0;
	}

	return 0;
}

/* @brief キャリブレーション
 *
 * GetRawData関数を使用して、指定回数値を取得し
 * その値をオフセットとして登録します
 *
 * @param  Count キャリブレーションのデータ取得回数
 *
 * @return uint8_t 0: 設定完了
 */
uint8_t ICM42688P::Calibration(uint16_t Count){

	int16_t Accel[3] = {};
	int16_t Gyro[3] = {};

	int32_t AccelTmp[3] = {};
	int32_t GyroTmp[3] = {};

	SelectBank(0);

	for(uint16_t i=0; i < Count; i++){

		GetRawData(Accel, Gyro);

		for(uint8_t j=0; j<3; j++){

			AccelTmp[j] += Accel[j];
			GyroTmp[j] += Gyro[j];
		}
	}

	for(uint8_t k=0; k<3; k++){

		AccelOffset[k] = AccelTmp[k] / Count;
		GyroOffset[k] = GyroTmp[k] / Count;
	}

	AccelOffset[2] -= 16384;

	return 0;
}

/* @brief BANKの変更
 *
 * @param uint8_t BANK 選択したいBANKの番号を入力
 *
 * @return None
 */
void ICM42688P::SelectBank(uint8_t Bank){

	Write(BANK0::REG_BANK_SEL, &Bank, 1);
}
