#include "ICM42688P_WIRE_I2C.h"

ICM42688P_WIRE_I2C::ICM42688P_WIRE_I2C(uint8_t SDA_pin, uint8_t SCL_pin) {

	this->SDA_pin = SDA_pin;
  this->SCL_pin = SCL_pin;

  Wire.begin(SDA_pin, SCL_pin);
}

void ICM42688P_WIRE_I2C::Write(ICM42688P::BANK0 reg_addr, uint8_t* tx_buffer, uint8_t len){

  //読み取り回数分繰り返す
  for(uint8_t i=0; i<len; i++){

    //デバイス選択
    Wire.beginTransmission(i2c_addr);  

    //レジスタアドレス
    Wire.write((uint8_t)reg_addr + i);

    //値の書き込み 
    Wire.write(tx_buffer[i]);

    //送信の終了
    Wire.endTransmission();
  }
}

void ICM42688P_WIRE_I2C::Read(ICM42688P::BANK0 reg_addr, uint8_t* rx_buffer, uint8_t len){

  //読み取り回数分繰り返す
  for(uint8_t i=0; i<len; i++){

    //デバイス選択
    Wire.beginTransmission(i2c_addr);

    //レジスタアドレス
    Wire.write((uint8_t)reg_addr + i);

    //通信継続
    Wire.endTransmission(false);

    //データの取得
    Wire.requestFrom(i2c_addr, single_byte);

    //データの読み取り
    rx_buffer[i] = Wire.read();
  }
}