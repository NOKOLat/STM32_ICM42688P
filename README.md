# STM32 ICM42688P

STM32のHALライブラリを用いてICM42688Pの6軸センサーデータを取得するためのコードです

I2CMasterモード、SPIMasterモードに対応しています

## I2C通信

- FastPulsMode(~1Mbps)まで対応しています

ピンの設定と通信速度の設定のみ行ってください

### プロジェクトフォルダに追加するファイル
```
ICM42688P.h / ICM42688P.cpp
ICM42688P_HAL_I2C.h / ICM42688P_HAL_I2C.cpp
```

### ピンの設定

一般的なI2C通信です

### サンプルコード

```cpp
#include "ICM42688P_HAL_I2C.h"

//使用するI2Cの番号を入力
ICM42688P_HAL_I2C icm(&hi2c1);

float GyroData[3] = {};
float GyroData[3] = {};

int main(void){

	icm.Connection();
  icm.AccelConfig(icm.ACCEL_Mode::LowNoize, icm.ACCEL_SCALE::SCALE02g, icm.ACCEL_ODR::ODR01000hz, icm.ACCEL_DLPF::ODR40);
	icm.GyroConfig(icm.GYRO_MODE::LowNoize, icm.GYRO_SCALE::Dps0250, icm.GYRO_ODR::ODR01000hz, icm.GYRO_DLPF::ODR40);
  HAL_Delay(100);
  icm.Calibration(1000);

  while(1){

    icm.GetData(AccelData, GyroData)
  }
}
```

## SPI通信

- 4本線通信
- 24Mbpsまで対応しています

### プロジェクトフォルダに追加するファイル
```
ICM42688P.h / ICM42688P.cpp
ICM42688P_HAL_SPI.h / ICM42688P_HAL_SPI.cpp
```

### ピンの設定

GPIO_OutputでSPIのデバイス選択を行っています

| 設定するピン | ICM42688Pのピン |
| ---- | ---- | 
| SPIx_SCK | SCLK |
| SPIx_MISO | SDIO / SDI|
| SPIx_MOSI | SDO |
| GPIO_Output | CS|

### GPIO
![スクリーンショット (524)](https://github.com/user-attachments/assets/e1aaffec-eda2-4d0e-bdeb-c1da288f8b3b)


### サンプルコード

```cpp
#include "ICM42688P_HAL_SPI.h"

//使用するSPIとGPIOのピンを入力
ICM42688P_HAL_SPI icm(&hspi1, GPIOB, GPIO_PIN_6);

float AccelData[3] = {};
float GyroData[3] = {};

int main(void){

	icm.Connection();
  icm.AccelConfig(icm.ACCEL_Mode::LowNoize, icm.ACCEL_SCALE::SCALE02g, icm.ACCEL_ODR::ODR01000hz, icm.ACCEL_DLPF::ODR40);
	icm.GyroConfig(icm.GYRO_MODE::LowNoize, icm.GYRO_SCALE::Dps0250, icm.GYRO_ODR::ODR01000hz, icm.GYRO_DLPF::ODR40);
  HAL_Delay(100);
  icm.Calibration(1000);

  while(1){

    icm.GetData(AccelData, GyroData)
  }
}
```
## その他

- サンプルコードは、起動時に実行される部分とメインループで実行する部分にわけて書いてあります

    使う環境に合わせてください

- Config関数の中身はenum classで定義してあります

    設定を変えたい際は、```icm.Mode::```などと入力すると候補が出てきます

![スクリーンショット (493)](https://github.com/user-attachments/assets/1dff5040-14f2-4d4b-9303-17537af35aec)

- 各関数には戻り値が設定されています

  特にConnection関数で通信確立を確認する場合はエラーハンドリングを行うようにしてください

  (return 1を判定できるように書いてください）
