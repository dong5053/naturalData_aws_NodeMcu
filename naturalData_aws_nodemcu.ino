// Master
#include <Wire.h>
#include <string>

// I2C Data Type
#define DST "DST"             // �̼����� ������
#define UTW "UTW"            // �����ļ��� ������
#define TPH "TPH"           // �½������� ������
#define ILM "ILM"             // �������� ������
#define GAS "GAS"             // �������� ������
#define PSS "PSS"             // ���м��� ������

boolean i2c_setup = true;   // I2C ���� �׽�Ʈ
String buf = "";            // ��/���� ������ ���� ����
int dataSize = 0;           // ���� ������ ������ ����

void setup() {
  Serial.begin(9600); // begin serial for debug //
  Wire.begin(D1, D2); // join i2c bus with SDA=D1 and SCL=D2 of NodeMCU //
}

void loop() 
{
  if(i2c_setup == true)   // ó�� 1ȸ ����
  {
    delay(3000);
    Wire.beginTransmission(8); // begin with device address 8
    Wire.write("Connect...");  // ���� �׽�Ʈ ���ڿ� ����
    Serial.println("Connect...");
    Wire.endTransmission();    // stop transmitting //
    Wire.requestFrom(8, 15); // request & read data of size 13 from slave //

    readTransferData();   // ���۵� �����͸� �а� ���ۿ� ����
    if(buf == "Connect Success")
      Serial.println("Connect Complete !");
    else
      Serial.println("Connect Fail");
    
    i2c_setup = false;
    buf = "";
  }
  delay(2000);
  Wire.beginTransmission(8); // begin with device address 8
  Wire.write("GET TPH DataSize");      // �½������� ������ ������ ������û
  Wire.endTransmission();    // stop transmitting
  Wire.requestFrom(8, 1); // request & read data
  readTransferData();   // ���۵� �����͸� �а� ���ۿ� ����
  dataSize = buf.toInt();
  Serial.println("TPH DataSize : " + buf);
  buf = "";

  //if(dataSize != 0 && dataSize < 10)
  if(true)
  {
    Wire.beginTransmission(8); // begin with device address 8
    Wire.write("GET TPH");      // �½������� ������ ������ ������û
    Wire.endTransmission();    // stop transmitting
    Wire.requestFrom(8, dataSize); // request & read data
    readTransferData();   // ���۵� �����͸� �а� ���ۿ� ����
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

void readTransferData()       // ���۵� �����͸� �а� ���ۿ� ����
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