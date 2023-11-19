#include <Arduino.h>
#include "dacx1416.h"

#define DAC_CS 10
#define DAC_RST 28 // Optionnal
#define DAC_LDAC -1

DACX1416 dac(DAC_CS, DAC_RST, DAC_LDAC, &SPI, 8000000);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("init");

  // Read device ID
  Serial.print("DEVICE ID=");
  Serial.println(dac.read_reg(R_DEVICEID), HEX);

  int res = dac.init();
  if(res==0) Serial.println("DACX1416 init success");

  // int ref
  dac.set_int_reference(false);
  Serial.print("internal ref stat =");
  Serial.println(dac.get_int_reference());

  // power up all 16 channels
  for(int i=0; i<16; i++){
    dac.set_ch_enabled(i, true);
  }

  // check channel power status
  for(int i=0; i<16; i++){
    Serial.print("ch ");
    Serial.print(i);
    Serial.print(" power -> ");
    Serial.println(dac.get_ch_enabled(i));
  }

  // Now set range, U = Unipolar, B=Bipolar; 
  dac.set_range(0, DACX1416::U_10); //   0 -> +10V
  dac.set_range(1, DACX1416::B_5);  //  -5 -> +5V
  dac.set_range(2, DACX1416::B_2V5);//  -2.5 -> +2.5V
  dac.set_range(3, DACX1416::B_10); // -10 -> +10V
  // Other ranges are set to default (U_5)
  
  // Read range
  for(int i=0; i<16; i++){
    Serial.print("ch ");
    Serial.print(i);
    Serial.print(" range -> ");
    Serial.println(dac.get_range(i), HEX);
  }

  //dac.set_sync(1, DACX1416::SYNC_LDAC);
  //dac.set_sync(0, DACX1416::SYNC_SS);
}

uint16_t j = 0;

void loop() {
  for(int i=0; i<16; i++){
    dac.set_out(i, (i*j)%65535 );
  }

  j = (j+16)%65535;  
  delayMicroseconds(1);
}