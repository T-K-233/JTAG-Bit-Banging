
#include <Arduino.h>
#include <Adafruit_TinyUSB.h> // for Serial

// pin map
#define TDO 10
#define TDI 11
#define TCK 12
#define TMS 13

#define CLK_DELAY 1  // this is 1/4 of clk period


#define X      0



void setup() {
  pinMode(TDO, INPUT);
  pinMode(TDI, OUTPUT);
  pinMode(TCK, OUTPUT);
  pinMode(TMS, OUTPUT);

  digitalWrite(TDI, LOW);
  digitalWrite(TCK, LOW);
  digitalWrite(TMS, LOW);

  Serial.begin(115200);

  while (!Serial) {}

  Serial.println("ready");
}

uint32_t clockJTAG(uint8_t TMS_val, uint8_t TDI_val) {
  uint32_t tdo_data = digitalRead(TDO);
  digitalWrite(TMS, TMS_val);
  digitalWrite(TDI, TDI_val);
  delay(CLK_DELAY);

  // TCK rising edge; target will sample TMS and TDI
  digitalWrite(TCK, HIGH);
  
  delay(CLK_DELAY);
  delay(CLK_DELAY);

  // TCK falling edge; target will set TDO
  digitalWrite(TCK, LOW);
  
  delay(CLK_DELAY);

  return tdo_data;
}

void resetJTAG() {
  clockJTAG(1, X);
  clockJTAG(1, X);
  clockJTAG(1, X);
  clockJTAG(1, X);
  clockJTAG(1, X);
}


void test_readIDCODE() {
  // assume start from idle
  // to DR scan
  clockJTAG(1, X);

  // to capture DR
  clockJTAG(0, X);
  
  // to shift DR
  clockJTAG(0, X);

  uint32_t id_code = 0;

  for (int i=0; i<32; i+=1) {
    id_code |= clockJTAG(0, 0) << i;
  }
  Serial.print(id_code, HEX); Serial.print("\t");

  id_code = 0;
  for (int i=0; i<32; i+=1) {
    id_code |= clockJTAG(0, 0) << i;
  }
  Serial.print(id_code, HEX); Serial.print("\t");
  
  id_code = 0;
  for (int i=0; i<32; i+=1) {
    id_code |= clockJTAG(0, 0) << i;
  }
  Serial.print(id_code, HEX); Serial.print("\t");

  Serial.println("");


  // to exit 1 DR
  clockJTAG(1, X);
  
  // to update DR
  clockJTAG(1, X);
  
  // to idle
  clockJTAG(0, X);
}

void test_testDRWidth() {
  // assume start from idle
  // to DR scan
  clockJTAG(1, X);

  // to capture DR
  clockJTAG(0, X);
  
  // to shift DR
  clockJTAG(0, X);

  // write a bunch of zeros to clear out all DR
  for (int i=0; i<200; i+=1) {
    clockJTAG(0, 0);
  }
  
  // shift in one 1
  clockJTAG(0, 1);
  
  
  uint32_t readout = 0;
  uint32_t counter = 0;

  while (!readout) {
    readout = clockJTAG(0, 0);
    counter += 1;
  }
  
  
  Serial.print("DR width: "); Serial.print(counter);

  Serial.println("");


  // to exit 1 DR
  clockJTAG(1, X);
  
  // to update DR
  clockJTAG(1, X);
  
  // to idle
  clockJTAG(0, X);  
}

void test_testIRWidth() {
  // assume start from idle
  // to DR scan
  clockJTAG(1, X);
  
  // to IR scan
  clockJTAG(1, X);

  // to capture IR
  clockJTAG(0, X);
  
  // to shift IR
  clockJTAG(0, X);

  // write a bunch of zeros to clear out all IR
  for (int i=0; i<200; i+=1) {
    clockJTAG(0, 0);
  }
  
  // shift in one 1
  clockJTAG(0, 1);
  
  
  uint32_t readout = 0;
  uint32_t counter = 0;

  while (!readout) {
    readout = clockJTAG(0, 0);
    counter += 1;
  }
  
  Serial.print("IR width: "); Serial.print(counter);

  Serial.println("");


  // to exit 1 IR
  clockJTAG(1, X);
  
  // to update IR
  clockJTAG(1, X);
  
  // to idle
  clockJTAG(0, X);  

}


void test_BYPASS() {
  // assume start from idle
  // to DR scan
  clockJTAG(1, X);
  
  // to IR scan
  clockJTAG(1, X);

  // to capture IR
  clockJTAG(0, X);
  
  // to shift IR
  clockJTAG(0, X);

  // write a bunch of ones to go in bypass
  for (int i=0; i<200; i+=1) {
    clockJTAG(0, 1);
  }
  
  // to exit 1 IR
  clockJTAG(1, X);
  
  // to update IR
  clockJTAG(1, X);
  
  // to idle
  clockJTAG(0, X);  
  
  // assume start from idle
  // to DR scan
  clockJTAG(1, X);

  // to capture DR
  clockJTAG(0, X);
  
  // to shift DR
  clockJTAG(0, X);

  // shift in a 1
  clockJTAG(0, 1);


  uint32_t readout;
  
  // do we get a 1?
  readout = clockJTAG(0, 0);
  Serial.print(readout); Serial.print(" ");
  
  // do we get a 1?
  readout = clockJTAG(0, 0);
  Serial.print(readout); Serial.print(" ");
  
  // do we get a 1?
  readout = clockJTAG(0, 0);
  Serial.print(readout); Serial.print(" ");
  
  // do we get a 1?
  readout = clockJTAG(0, 0);
  Serial.print(readout); Serial.print(" ");

  Serial.println("");

  // to exit 1 DR
  clockJTAG(1, X);
  
  // to update DR
  clockJTAG(1, X);
  
  // to idle
  clockJTAG(0, X);
}

void loop() {
  Serial.println("================================");
  resetJTAG();
  // to idle state
  clockJTAG(0, X);

  // begin test

//  test_readIDCODE();

//  test_testDRWidth();
  test_testIRWidth();

//  test_BYPASS();
  
}
