// Master
#include <Wire.h>
#include <string>

// I2C Data Type
#define DST "DST"             // 미센먼지 데이터
#define UTW "UTW"            // 초음파센서 데이터
#define TPH "TPH"           // 온습도센서 데이터
#define ILM "ILM"             // 조도센서 데이터
#define GAS "GAS"             // 가스센서 데이터
#define PSS "PSS"             // 대기압센서 데이터

boolean i2c_setup = true;   // I2C 연결 테스트
String buf = "";            // 송/수신 데이터 저장 버퍼
int dataSize = 0;           // 센서 데이터 사이즈 정보

void setup() {
  Serial.begin(9600); // begin serial for debug //
  Wire.begin(D1, D2); // join i2c bus with SDA=D1 and SCL=D2 of NodeMCU //
}

void loop() 
{
  if(i2c_setup == true)   // 처음 1회 실행
  {
    delay(3000);
    Wire.beginTransmission(8); // begin with device address 8
    Wire.write("Connect...");  // 연결 테스트 문자열 전송
    Serial.println("Connect...");
    Wire.endTransmission();    // stop transmitting //
    Wire.requestFrom(8, 15); // request & read data of size 13 from slave //

    readTransferData();   // 전송된 데이터를 읽고 버퍼에 저장
    if(buf == "Connect Success")
      Serial.println("Connect Complete !");
    else
      Serial.println("Connect Fail");
    
    i2c_setup = false;
    buf = "";
  }
  delay(2000);
  Wire.beginTransmission(8); // begin with device address 8
  Wire.write("GET TPH DataSize");      // 온습도센서 데이터 사이즈 정보요청
  Wire.endTransmission();    // stop transmitting
  Wire.requestFrom(8, 1); // request & read data
  readTransferData();   // 전송된 데이터를 읽고 버퍼에 저장
  dataSize = buf.toInt();
  Serial.println("TPH DataSize : " + buf);
  buf = "";

  //if(dataSize != 0 && dataSize < 10)
  if(true)
  {
    Wire.beginTransmission(8); // begin with device address 8
    Wire.write("GET TPH");      // 온습도센서 데이터 사이즈 정보요청
    Wire.endTransmission();    // stop transmitting
    Wire.requestFrom(8, dataSize); // request & read data
    readTransferData();   // 전송된 데이터를 읽고 버퍼에 저장
    Serial.println("Temp : " + buf);
    buf = "";
  }

 /*  Wire.requestFrom(8, 13); // request & read data of size 13 from slave //
  while(Wire.available())
  {
    char c = Wire.read();
    Serial.print(c);
    str += c;
  }
  Serial.println();
  if(str == "Hello NodeMCU")
  {
    Serial.println("Successful !!");
  } */
  delay(1000);
}

void readTransferData()       // 전송된 데이터를 읽고 버퍼에 저장
{
  while(Wire.available())
  {
    char c = Wire.read();
    Serial.print(c);
    buf += c;
  }
  Serial.println();
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