// Master
#include <Wire.h>

void setup() {
 Serial.begin(9600); // begin serial for debug //
 Wire.begin(D1, D2); // join i2c bus with SDA=D1 and SCL=D2 of NodeMCU //
}

void loop() {
 Wire.beginTransmission(8); // begin with device address 8 //
 Wire.write("Hello Arduino");  // sends hello string //
 Wire.endTransmission();    // stop transmitting //

 Wire.requestFrom(8, 5); // request & read data of size 13 from slave //
 while(Wire.available()){
    char c = Wire.read();
  Serial.print(c);
 }
 Serial.println();
 delay(1000);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/* #include <Wire.h>

void setup() {
 Wire.begin(8);                // join i2c bus with address 8 //
 Wire.onReceive(receiveEvent); // register receive event //
 Wire.onRequest(requestEvent); // register request event //
 Serial.begin(9600);           // start serial for debug //
}

void loop() {
 delay(100);
}

// function that executes whenever data is received from master
void receiveEvent(int howMany) {
 while (0 <Wire.available()) {
    char c = Wire.read();      // receive byte as a character //
    Serial.print(c);           // print the character //
  }
 Serial.println();             // to newline //
}

// function that executes whenever data is requested from master
void requestEvent() {
 Wire.write("Hello Arduino");  //send string on request //
} */
