
#include <Arduino.h>
#include <Adafruit_TinyUSB.h> // for Serial
#include <SoftwareSerial.h>

// pin map
#define TDO 10
#define TDI 11
#define TCK 12
#define TMS 13

#define HALF_CLOCK_PERIOD 10


uint8_t clockJTAG(uint8_t TMS_level, uint8_t TDI_level) {
  digitalWrite(TMS, TMS_level);
  digitalWrite(TDI, TDI_level);
  
  // JTAG samples at rising edge, and changes TDO at falling edge
  
  digitalWrite(TCK, 1);
  
  delay(HALF_CLOCK_PERIOD);
  
  digitalWrite(TCK, 0);
  
  uint8_t sample = digitalRead(TDO); 
  delay(HALF_CLOCK_PERIOD);

  return sample;
}

void resetJTAG() {
  // going to reset state
  // no matter what state you are, if TMS stays at "1" for five clocks, a TAP controller goes back to the state "Test-Logic-Reset"
  clockJTAG(1, 0);
  clockJTAG(1, 0);
  clockJTAG(1, 0);
  clockJTAG(1, 0);
  clockJTAG(1, 0);
}

uint32_t readJTAGWord() {
  uint32_t val = 0;

  for (int i=0; i<32; i+=1) {
    val |= (clockJTAG(0, 0) & 0b1) << i;  // stay in shift-DR mode
  }
  
  return val;
}



void setup() {
  Serial.begin(115200);

  pinMode(TDO, INPUT);
  pinMode(TDI, OUTPUT);
  pinMode(TCK, OUTPUT);
  pinMode(TMS, OUTPUT);
  pinMode(9, OUTPUT);

  while (!Serial) {}  // wait for Serial

  Serial.println("ready");
}


void TEST_justReset() {  // just reset
  uint8_t val = clockJTAG(1, 0);
  Serial.println(val);
}

//
void TEST_readIDCode() {
  resetJTAG();

  // go to Shift-DR
  clockJTAG(0, 0);
  clockJTAG(1, 0);
  clockJTAG(0, 0);
//  clockJTAG(TMS_0);  

  Serial.println("================");

  uint32_t val = 0;

  val = readJTAGWord();

  Serial.print("0x");
  Serial.print(val, HEX);
  Serial.println("");
  
//  val = readID();
//
//  Serial.print("0x");
//  Serial.print(val, HEX);
//  Serial.println("");
//  
//  val = readID();
//
//  Serial.print("0x");
//  Serial.print(val, HEX);
//  Serial.println("");
//  
  
  //delay(1000);
}

void TEST_captureIR() {
  
  // going to reset state
  // no matter what state you are, if TMS stays at "1" for five clocks, a TAP controller goes back to the state "Test-Logic-Reset"
  resetJTAG();
  
  // to test idle
  clockJTAG(0, 0);

  // to DR scan
  clockJTAG(1, 0);

  // to IR scan
  clockJTAG(1, 0);

  // to capture IR
  clockJTAG(0, 0);
  
  // to shift IR
  clockJTAG(0, 0);

  // to shift IR
  Serial.print(clockJTAG(0, 0)); Serial.print(" ");
  Serial.print(clockJTAG(0, 0)); Serial.print(" ");
  Serial.print(clockJTAG(0, 0)); Serial.print(" ");
  Serial.print(clockJTAG(0, 0)); Serial.print(" ");
  Serial.print(clockJTAG(0, 0)); Serial.print(" ");
  Serial.print(clockJTAG(0, 0)); Serial.print(" ");
  Serial.println();
}



void TEST_scanDevices() {  // scan devices
  resetJTAG();

  // go to Shift-IR
  clockJTAG(0, 0);
  clockJTAG(1, 0);
  clockJTAG(1, 0);
  clockJTAG(0, 0);
  clockJTAG(0, 0);

  Serial.println("================");

  int i;
  
  // Send plenty of ones into the IR registers
  // That makes sure all devices are in BYPASS!
  for(i=0; i<1000; i+=1) {
    clockJTAG(0, 1);
  }
  
  clockJTAG(1, 1);  // last bit needs to have TMS active, to exit shift-IR

  // we are in Exit1-IR, go to Shift-DR
  clockJTAG(1, 0);
  clockJTAG(1, 0);
  clockJTAG(0, 0);
  clockJTAG(0, 0);

  // Send plenty of zeros into the DR registers to flush them
  for(i=0; i<1000; i+=1) {
    clockJTAG(0, 0);
  }

  // now send ones until we receive one back
  for(i=0; i<1000; i+=1) {
    if(clockJTAG(0, 1)) {
      break;
    }
  }

  int nbDevices = i;
  Serial.print("There are ");
  Serial.print(nbDevices);
  Serial.print(" device(s) in the JTAG chain");
  

  
  Serial.println("");

  
  delay(1000);
}


void loop() {
//  TEST_justReset();

//  TEST_captureIR();

//  TEST_readIDCode();
  
  TEST_scanDevices();
}
