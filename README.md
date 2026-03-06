# STM32 ICM42688P

STM32のHALライブラリを用いてICM42688Pの6軸センサーデータを取得するためのコードです

### サンプルコード

ICM42688Pの6軸センサーを使用するには、通信関数とログ関数をコンストラクタに渡します。

以下はSTM32 HAL I2Cを用いたサンプルコードです：

```cpp
#include "wrapper.hpp"
#include "stdio.h"
#include "i2c.h"
#include "ICM42688P.h"

// I2C通信の書き込み関数
static uint8_t icm42688p_write(uint8_t reg_addr, uint8_t* tx_buffer, uint8_t len){

	return HAL_I2C_Mem_Write(&hi2c1, 0x68 << 1, (uint16_t)reg_addr, I2C_MEMADD_SIZE_8BIT, tx_buffer, len, 100);
}

// I2C通信の読み込み関数
static uint8_t icm42688p_read(uint8_t reg_addr, uint8_t* rx_buffer, uint8_t len){

	return HAL_I2C_Mem_Read(&hi2c1, 0x68 << 1, (uint16_t)reg_addr, I2C_MEMADD_SIZE_8BIT, rx_buffer, len, 100);
}

// ログ出力関数
static void icm42688p_log(char* msg){
	
	printf("%s", msg);
}

// ICM42688Pオブジェクト生成（通信関数とログ関数をコンストラクタに渡す）
ICM42688P icm(icm42688p_write, icm42688p_read, icm42688p_log);

// データ格納用変数
float accel_data[3] = {};
float gyro_data[3] = {};

void init(){

	printf("program start\n");

	// 通信チェック
	if(icm.Connection()){

		printf("ICM42688p Not Found\n");
	}

	// センサーの設定
	icm.AccelConfig(icm.ACCEL_Mode::LowNoize, icm.ACCEL_SCALE::SCALE02g, icm.ACCEL_ODR::ODR01000hz, icm.ACCEL_DLPF::ODR40);
	icm.GyroConfig(icm.GYRO_MODE::LowNoize, icm.GYRO_SCALE::Dps0250, icm.GYRO_ODR::ODR01000hz, icm.GYRO_DLPF::ODR40);

	HAL_Delay(1000);

	// 静止キャリブレーション
	icm.Calibration(100);
}

void loop(){

	icm.GetData(accel_data, gyro_data);

	printf("Accel[m/s^2]: %+4.4f %+4.4f %+4.4f\n", accel_data[0], accel_data[1], accel_data[2]);
	printf("Gyro[deg/s]:  %+4.4f %+4.4f %+4.4f\n", gyro_data[0], gyro_data[1], gyro_data[2]);

	HAL_Delay(50);
}
```

## ほかの通信方法での例

### STM32 HAL SPI

外部から定義が必要：
```cpp
#define ICM42688P_SPI_TIMEOUT 10  // milliseconds
#define ICM42688P_CS_PORT  GPIOA
#define ICM42688P_CS_PIN   GPIO_PIN_4
```

実装：
```cpp
// SPI通信の書き込み関数
static uint8_t icm42688p_write(uint8_t reg_addr, uint8_t* tx_buffer, uint8_t len){
	uint8_t tx_tmp[2] = {0};

	for(uint8_t i = 0; i < len; i++){
		tx_tmp[0] = (reg_addr + i) & 0x7F;  // Write bit (MSB = 0)
		tx_tmp[1] = tx_buffer[i];

		HAL_GPIO_WritePin(ICM42688P_CS_PORT, ICM42688P_CS_PIN, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, tx_tmp, 2, ICM42688P_SPI_TIMEOUT);
		HAL_GPIO_WritePin(ICM42688P_CS_PORT, ICM42688P_CS_PIN, GPIO_PIN_SET);
	}
	return 0;
}

// SPI通信の読み込み関数
static uint8_t icm42688p_read(uint8_t reg_addr, uint8_t* rx_buffer, uint8_t len){
	uint8_t tx_tmp[2] = {0};
	uint8_t rx_tmp[2] = {0};

	for(uint8_t i = 0; i < len; i++){
		tx_tmp[0] = (reg_addr + i) | 0x80;  // Read bit (MSB = 1)
		tx_tmp[1] = 0x00;

		HAL_GPIO_WritePin(ICM42688P_CS_PORT, ICM42688P_CS_PIN, GPIO_PIN_RESET);
		HAL_SPI_TransmitReceive(&hspi1, tx_tmp, rx_tmp, 2, ICM42688P_SPI_TIMEOUT);
		HAL_GPIO_WritePin(ICM42688P_CS_PORT, ICM42688P_CS_PIN, GPIO_PIN_SET);

		rx_buffer[i] = rx_tmp[1];
	}
	return 0;
}

// ログ出力関数
static void icm42688p_log(char* msg){
	printf("%s", msg);
}

// ICM42688Pオブジェクト生成
ICM42688P icm(icm42688p_write, icm42688p_read, icm42688p_log);
```

### Arduino Wire (I2C)
```cpp
#include <Wire.h>

// I2C通信の書き込み関数
static uint8_t icm42688p_write(uint8_t reg_addr, uint8_t* tx_buffer, uint8_t len){
	Wire.beginTransmission(0x68);  // ICM42688P I2C address
	Wire.write(reg_addr);
	Wire.write(tx_buffer, len);
	return Wire.endTransmission();
}

// I2C通信の読み込み関数
static uint8_t icm42688p_read(uint8_t reg_addr, uint8_t* rx_buffer, uint8_t len){
	Wire.beginTransmission(0x68);
	Wire.write(reg_addr);
	Wire.endTransmission(false);
	Wire.requestFrom(0x68, len);
	for(uint8_t i = 0; i < len && Wire.available(); i++){
		rx_buffer[i] = Wire.read();
	}
	return 0;
}

// ログ出力関数
static void icm42688p_log(char* msg){
	Serial.print(msg);
}

// ICM42688Pオブジェクト生成
ICM42688P icm(icm42688p_write, icm42688p_read, icm42688p_log);
```

### Arduino SPI
```cpp
#include <SPI.h>

#define CS_PIN 10  // Chip Select pin

// SPI通信の書き込み関数
static uint8_t icm42688p_write(uint8_t reg_addr, uint8_t* tx_buffer, uint8_t len){
	digitalWrite(CS_PIN, LOW);
	SPI.transfer(reg_addr & 0x7F);  // Write bit (MSB = 0)
	SPI.transfer(tx_buffer, len);
	digitalWrite(CS_PIN, HIGH);
	return 0;
}

// SPI通信の読み込み関数
static uint8_t icm42688p_read(uint8_t reg_addr, uint8_t* rx_buffer, uint8_t len){
	digitalWrite(CS_PIN, LOW);
	SPI.transfer(reg_addr | 0x80);  // Read bit (MSB = 1)
	for(uint8_t i = 0; i < len; i++){
		rx_buffer[i] = SPI.transfer(0x00);
	}
	digitalWrite(CS_PIN, HIGH);
	return 0;
}

// ログ出力関数
static void icm42688p_log(char* msg){
	Serial.print(msg);
}

// ICM42688Pオブジェクト生成
ICM42688P icm(icm42688p_write, icm42688p_read, icm42688p_log);
```

## 設定項目について

- 加速度センサーとジャイロセンサーの設定はそれぞれ以下の関数で行います

- 各引数の詳細は以下の通りです
```cpp
icm.AccelConfig(Accel_Mode mode, ACCEL_SCALE scale, ACCEL_ODR odr, ACCEL_DLPF dlpff);
icm.GyroConfig(GYRO_MODE mode, GYRO_SCALE scale, GYRO_ODR odr, GYRO_DLPF dlpff);
``` 

## ジャイロセンサー設定

#### GYRO_MODE（ジャイロセンサーの動作モード）
| 値 | 説明 |
|---|---|
| OFF (0x00) | 動作停止 |
| Standby (0x01) | スタンバイモード |
| LowNoize (0x03) | 低ノイズモード|

#### GYRO_SCALE（ジャイロセンサーの測定範囲）
| 値 | 測定範囲 | 感度（LSB/dps）|
|---|---|---|
| Dps2000 (0x00) | ±2000 dps | 16.4 |
| Dps1000 (0x01) | ±1000 dps | 32.8 |
| Dps0500 (0x02) | ±500 dps | 65.5 |
| Dps0250 (0x03) | ±250 dps | 131.1 |
| Dps0125 (0x04) | ±125 dps | 262.1 |
| Dps0062 (0x05) | ±62.5 dps | 524.3 |
| Dps0031 (0x06) | ±31.25 dps | 1048.6 |
| Dps0015 (0x07) | ±15.625 dps | 2097.2 |

#### GYRO_ODR（ジャイロセンサーの出力データレート）

- サンプリングレートと精度はトレードオフの関係にあるので、用途に応じて適切な値を選択してください

| 値 | サンプリングレート |
|---|---|
| ODR32000hz (0x01) | 32000 Hz |
| ODR16000hz (0x02) | 16000 Hz |
| ODR08000hz (0x03) | 8000 Hz |
| ODR04000hz (0x04) | 4000 Hz |
| ODR02000hz (0x05) | 2000 Hz |
| ODR01000hz (0x06) | 1000 Hz|
| ODR00200hz (0x07) | 200 Hz |
| ODR00100hz (0x08) | 100 Hz |
| ODR00050hz (0x09) | 50 Hz |
| ODR00025hz (0x0A) | 25 Hz |
| ODR00012hz (0x0B) | 12.5 Hz |
| ODR00500hz (0x0F) | 500 Hz |

#### GYRO_DLPF（ジャイロセンサーのデジタルローパスフィルタ）
| 値 | カットオフ周波数 | 
|---|---|
| ODR02 (0x00) | ODR/2 |
| ODR04 (0x01) | ODR/4 |
| ODR05 (0x02) | ODR/5 | 
| ODR08 (0x03) | ODR/8 |
| ODR10 (0x04) | ODR/10 | 
| ODR16 (0x05) | ODR/16 | 
| ODR20 (0x06) | ODR/20 |
| ODR40 (0x07) | ODR/40 |


## 加速度センサー設定

#### ACCEL_Mode（加速度センサーの動作モード）
| 値 | 説明 |
|---|---|
| OFF (0x00) | 動作停止 |
| LowPower (0x02) | 低消費電力モード |
| LowNoize (0x03) | 低ノイズモード|

#### ACCEL_SCALE（加速度センサーの測定範囲）
| 値 | 測定範囲 | 感度（LSB/g）|
|---|---|---|
| SCALE16g (0x00) | ±16 g | 2048 |
| SCALE08g (0x01) | ±8 g | 4096 |
| SCALE04g (0x02) | ±4 g | 8192 |
| SCALE02g (0x03) | ±2 g | 16384 |

#### ACCEL_ODR（加速度センサーの出力データレート）

- サンプリングレートと精度はトレードオフの関係にあるので、用途に応じて適切な値を選択してください

| 値 | サンプリングレート |
|---|---|
| ODR32000hz (0x01) | 32000 Hz |
| ODR16000hz (0x02) | 16000 Hz |
| ODR08000hz (0x03) | 8000 Hz |
| ODR04000hz (0x04) | 4000 Hz |
| ODR02000hz (0x05) | 2000 Hz |
| ODR01000hz (0x06) | 1000 Hz|
| ODR00200hz (0x07) | 200 Hz |
| ODR00100hz (0x08) | 100 Hz |
| ODR00050hz (0x09) | 50 Hz |
| ODR00025hz (0x0A) | 25 Hz |
| ODR00012hz (0x0B) | 12.5 Hz |
| ODR00006hz (0x0C) | 6.25 Hz |
| ODR00003hz (0x0D) | 3.125 Hz |
| ODR00001hz (0x0E) | 1.5625 Hz |
| ODR00500hz (0x0F) | 500 Hz |

#### ACCEL_DLPF（加速度センサーのデジタルローパスフィルタ）
| 値 | カットオフ周波数 |
|---|---|
| ODR02 (0x00) | ODR/2 | 
| ODR04 (0x01) | ODR/4 |
| ODR05 (0x02) | ODR/5 | 
| ODR08 (0x03) | ODR/8 | 
| ODR10 (0x04) | ODR/10 |
| ODR16 (0x05) | ODR/16 | 
| ODR20 (0x06) | ODR/20 | 
| ODR40 (0x07) | ODR/40 | 
