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
 * @param [in]ICM42688P::ACCEL_Mode   加速度センサーのモード
 * @param [in]ICM42688P::ACCEL_SCALE  センサーの測定スケール
 * @param [in]ICM42688P::ACCEL_ODR    出力レート
 * @param [in]ICM42688P::ACCEL_DLPF   DLPFの設定
 *
 * @return uint8_t 0: 設定完了 1: PWR_MGMT0 設定失敗 2: ACCEL_CONFIG0　設定失敗 3: GYRO_ACCEL_CONFIG0 設定失敗
 */
uint8_t ICM42688P::AccelConfig(ICM42688P::ACCEL_Mode Mode, ICM42688P::ACCEL_SCALE Scale, ICM42688P::ACCEL_ODR ODR, ICM42688P::ACCEL_DLPF DLPF){

	//現在のモードを保存
	AccelModeTmp = (uint8_t)Mode;

	//Mode設定を作成
	uint8_t Command = (GyroModeTmp << 2) | AccelModeTmp;

	//値を書き込み
	uint8_t Error = 0;
	uint8_t NowMode = 0;
	while(Command != NowMode){

		Write(ICM42688P::BANK0::PWR_MGMT0, &Command, 1);
		Read(ICM42688P::BANK0::PWR_MGMT0, &NowMode, 1);

		//エラー判定
		Error ++;
		if(Error > 100){
			return 1;
		}
	}

	//ScaleとODRの設定を作成
	Command = (uint8_t)Scale << 5 | (uint8_t)ODR;

	//値を書き込み
	Error = 0;
	NowMode = 0;
	while(Command != NowMode){

		Write(ICM42688P::BANK0::ACCEL_CONFIG0, &Command, 1);
		Read(ICM42688P::BANK0::ACCEL_CONFIG0, &NowMode, 1);

		//エラー判定
		Error ++;
		if(Error > 100){
			return 2;
		}
	}

	//DLFPの値を保存
	AccelDLPFTmp = (uint8_t)DLPF;

	//DLPFの設定を作成
	Command = GyroDLPFTmp | (AccelDLPFTmp << 4);

	Error = 0;
	NowMode = 0;
	while(Command != NowMode){

		Write(ICM42688P::BANK0::GYRO_ACCEL_CONFIG0, &Command, 1);
		Read(ICM42688P::BANK0::GYRO_ACCEL_CONFIG0, &NowMode, 1);

		//エラー判定
		Error ++;
		if(Error > 100){
			return 3;
		}
	}

	//Scaleの値を保存
	AccelScaleValue = 16.0 / pow(2,(uint8_t)Scale);

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
uint8_t ICM42688P::GyroConfig(ICM42688P::GYRO_MODE Mode, ICM42688P::GYRO_SCALE Scale, ICM42688P::GYRO_ODR ODR, ICM42688P::GYRO_DLPF DLPF){

	//現在のモードを保存
	GyroModeTmp = (uint8_t)Mode;

	//Mode設定を作成
	uint8_t Command = (GyroModeTmp << 2) | AccelModeTmp;

	//値を書き込み
	uint8_t Error = 0;
	uint8_t NowMode = 0;
	while(Command != NowMode){

		Write(ICM42688P::BANK0::PWR_MGMT0, &Command, 1);
		Read(ICM42688P::BANK0::PWR_MGMT0, &NowMode, 1);

		//エラー判定
		Error ++;
		if(Error > 100){
			return 1;
		}
	}

	//ScaleとODRの設定を書き込み
	Command = (uint8_t)Scale << 5 | (uint8_t)ODR;

	//値を書き込み
	NowMode = 0;
	Error = 0;
	while(Command != NowMode){

		Write(ICM42688P::BANK0::GYRO_CONFIG0, &Command, 1);
		Read(ICM42688P::BANK0::GYRO_CONFIG0, &NowMode, 1);

		//エラー判定
		Error ++;
		if(Error > 100){
			return 2;
		}
	}

	//DLFPの値を保存
	GyroDLPFTmp = (uint8_t)DLPF;

	//DLPFの設定を作成
	Command = GyroDLPFTmp | (AccelDLPFTmp << 4);

	Error = 0;
	NowMode = 0;
	while(Command != NowMode){

		Write(ICM42688P::BANK0::GYRO_ACCEL_CONFIG0, &Command, 1);
		Read(ICM42688P::BANK0::GYRO_ACCEL_CONFIG0, &NowMode, 1);

		//エラー判定
		Error ++;
		if(Error > 100){
			return 3;
		}
	}

	//Scaleの値を保存
	GyroScaleValue = 2000.0 / pow(2, (uint8_t)Scale);

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
uint8_t ICM42688P::GetRawData(int16_t AccelData[3], int16_t GyroData[3]){

	//値を取得
	uint8_t RawData[12] = {};
	uint8_t Error = 0;
	while(RawData[1] == 0 && RawData[3] == 0 && RawData[5] == 0){

		Read(ICM42688P::BANK0::ACCEL_DATA_X1, RawData, 12);

		Error ++;
		if(Error > 200){

			return 1;
		}
	}

	//値を処理
	AccelData[0]  = (int16_t)(RawData[1] | (RawData[0] << 8)) - AccelOffset[0];
	AccelData[1]  = (int16_t)(RawData[3] | (RawData[2] << 8)) - AccelOffset[1];
	AccelData[2]  = (int16_t)(RawData[5] | (RawData[4] << 8)) - AccelOffset[2];

	GyroData[0]  = (int16_t)(RawData[7]  | RawData[6]  << 8) - GyroOffset[0];
	GyroData[1]  = (int16_t)(RawData[9]  | RawData[8]  << 8) - GyroOffset[1];
	GyroData[2]  = (int16_t)(RawData[11] | RawData[10] << 8) - GyroOffset[2];

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
uint8_t ICM42688P::GetData(float AccelData[3], float GyroData[3]){

	int16_t AccelBuffer[3] = {};
	int16_t GyroBuffer[3] = {};

	//データの取得
	if(GetRawData(AccelBuffer, GyroBuffer) == 1){

		//取得失敗
		return 1;
	}

	//単位を変換
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
 * @param  [in]Count キャリブレーションのデータ取得回数
 *
 * @return uint8_t 成功: 0、失敗: 1
 */
uint8_t ICM42688P::Calibration(uint16_t Count){

	int16_t Accel[3] = {};
	int16_t Gyro[3] = {};

	int32_t AccelTmp[3] = {};
	int32_t GyroTmp[3] = {};

	for(int16_t i=0; i < Count; i++){

		if(ICM42688P::GetRawData(Accel, Gyro) == 1){

			return 1;
		}

		for(uint8_t j=0; j<3; j++){

			AccelTmp[j] += Accel[j];
			GyroTmp[j] += Gyro[j];
		}

		for(uint32_t i=0; i<25000; i++);
	}

	for(uint8_t k=0; k<3; k++){

		AccelOffset[k] = AccelTmp[k] / Count;
		GyroOffset[k] = GyroTmp[k] / Count;
	}

	//重量加速度をオフセットに含める
	AccelOffset[2] -= 32768 / AccelScaleValue;

	return 0;
}
