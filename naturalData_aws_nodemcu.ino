// Master
#include <Wire.h>
#include <string.h>

// Example of the different modes of the X.509 validation options
// in the WiFiClientBearSSL object
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <time.h>

const char *ssid = "STARTUPSQUARE(1)_2.4G";                              // �������� �̸�
const char *pass = "inustartup";                                         // �������� ��й�ȣ
const char *thingId = "Mtest";                                           // �繰 �̸� (thing ID)
const char *host = "apfljqnba95p0-ats.iot.ap-northeast-2.amazonaws.com"; // AWS IoT Core �ּ�

// �繰 ������ (���� �̸�: xxxxxxxxxx-certificate.pem.crt)
const char cert_str[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUVrh3Dt1gjxBaSl9mjiO9zjsOBtMwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIwMTAyNjAxMjgw
OVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL/xd38wWA91KJgExPpI
qAjw4Up5C2Sf3ceZEcoBDJfoiZ1B5+zPEEnUG2XIQ8hpf84Dkm/ov6YZk/dGNg8g
1ZuplddHoEY1pcU49eT8F6E43UWduwWGGjmsAQyYe3IlYSd12HMCu/9mliIX/ty/
KBBFLxvQXfKABXoOlN+8ZFLZBwkSaRfhQ/TKXHNu3KseWeaETq0lh+85vKQAuvW0
SY/nfsmAvM21zUCW6PIN9DvfZ+j9H8o6uvuy+aRg6hBtAzlswbTMIPmjmjz1HsSP
tmxJ6KR/f0rb/zl41hwDxD3zC1k2mqMAYV1hfZoIWhKbVZKOX6+9ck0ICwQF949W
MxkCAwEAAaNgMF4wHwYDVR0jBBgwFoAU9f8Oa8UsilcxT1/xJbEe5GhfX0cwHQYD
VR0OBBYEFHSkwJeQXvixCKn0W5/EFD1Y6PMVMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAM63AJErGtuW5CGjsBfUBSix0r
/Dj/ZATMMoM4TthFfyJBrj0v8DG6fxQBwSYbB45Wqd7DJ/OWy3qJ8uBo3oStFz6B
NmfRe9Xq3l+nkBgOMZmtPYcbJ5XNj+iYshHKURKNVrJ+LmV5Xf5aQWCJ9vgN58ci
nvFDrHoYkc0bX8YtyLukuAy2Kgg1/MS1gDSp9bpgWI84iq/bByDfwWPy4g4VLz+7
3R3BRyFm2XpJnwblAX14K2H1g0h2cjPOELePyRVHZ78SHy8Un3FktZFp+iqf9y8Q
1szykt8qiweJ50cfcU9Wna00Og6N7kVM/Q07eUfiaxzX2rGaSzLW+Bt50NNn
-----END CERTIFICATE-----
)EOF";
// �繰 ������ �����̺� Ű (���� �̸�: xxxxxxxxxx-private.pem.key)
const char key_str[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAv/F3fzBYD3UomATE+kioCPDhSnkLZJ/dx5kRygEMl+iJnUHn
7M8QSdQbZchDyGl/zgOSb+i/phmT90Y2DyDVm6mV10egRjWlxTj15PwXoTjdRZ27
BYYaOawBDJh7ciVhJ3XYcwK7/2aWIhf+3L8oEEUvG9Bd8oAFeg6U37xkUtkHCRJp
F+FD9Mpcc27cqx5Z5oROrSWH7zm8pAC69bRJj+d+yYC8zbXNQJbo8g30O99n6P0f
yjq6+7L5pGDqEG0DOWzBtMwg+aOaPPUexI+2bEnopH9/Stv/OXjWHAPEPfMLWTaa
owBhXWF9mghaEptVko5fr71yTQgLBAX3j1YzGQIDAQABAoIBAQCfiIwgq8bVejQ2
PeiWeiMn40XthzP/+5vzFPejJUIZVI5kpW2l9uh75I6tB0nHXmB6k5934kTAQowo
PzQZWNoSGl7PYtoE4PwaM2C1ZQ4i00eeYB4c1jwf6SWqGWapFgZtLNatqP0mp1Zr
mfQb98nN1CL2hDZBo0RqEYqNd2EPNaAyDeNmXGDLmXhFWrElfKScd0XCaVRydB3c
/8TCisPaBkrX5fJmo/MQrKv0+ZMpS794DDuKmonaa0+k3hQyOVsGE1MC2FiJZigI
uqbZaDFHczUbtwQJkz+NxCziSVBc+NWYGqxxEs3tlJ9LuFYPTPjuvwY+Z9Fm/d9e
CcmFfHaBAoGBAOseo9IE5LNdntTPVNfcPeDzPwheRsizMhhTaoTS/DNM2BkEQZYh
mSXlHsDSxo5boaBQnyNMinxffVcb4i+ue4psKDcJ+ud+x3v3KqB2kTccsW1XWT2I
pEYF+YDjDuanlKr4+fPA9nV7gd/XpFiu5GHvmuGwy+1Vbp6+HmJJuVOpAoGBAND9
OdQgnyDdDdZi2bqDfm44YYYVzPa9Um1uOtSoHA2vlzwodGSiv5YEWfLPVWveZQ57
geO8+F6VMSfecM1qRyuufzGyDZ+4Lcw/mld8BfUQlqkEJJlZjFi0sw9alkQXnZJC
ZGso5U0MPTjCmC7dsRg/t1XfQeehQRW0QQV48YnxAoGBAJgVwbHgKG89Z7gQRLk8
ZWpU7bjkArVf9E935bb1It2sHzxPXH1YTlAM3M+th3IeI3YnsEiNAidhPRpTlwNw
wcjQHVM8yWdkwnSVdKVHDLexf7NShQ8gi+Ifizyror/gNnJBiqz5bHKh6Wh5fH1A
48nVy0GhssdaM6vLHqxFM/PBAoGAR36ZjewrA/8bgMAIZDofwm0fT9OdbruNu0n+
bkmkbzomrrnYN6VFjXinnN05B9HpGG9AHDzFavX2/wCl7l/xQQxNDx8ucfl8EWeg
0VToZNHr+J4mNx/fV2lZ6GLBDUJW8XYtDEvNRfJt70EC9uV4dCc3/PSHg0ozDc3A
gLLDTsECgYAF1CLGR6EP4Ptec3tlK+VHHj6pBTdLJSM+CIdcEmteVn2eQIirGo/D
+J96hMi/vqdkqvjd4BbOmxn+hbN6/fFELciF6ZQ0EqCQ04ogiCANVmQyUZMKeK5T
Y9KZ7tsxiufEWvnnrX6D1nBb73Ud27Onua+p/UW6gwgRaowh8HzJkw==
-----END RSA PRIVATE KEY-----
)EOF";
// Amazon Trust Services(ATS) ��������Ʈ CA ������ (�������� > "RSA 2048��Ʈ Ű: Amazon Root CA 1" �ٿ�ε�)
const char ca_str[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

// I2C Data Type
#define DST "DST" // �̼����� ������
#define UTW "UTW" // �����ļ��� ������
#define TPH "TPH" // �½������� ������
#define ILM "ILM" // �������� ������
#define GAS "GAS" // �������� ������
#define PSS "PSS" // ���м��� ������

#define DATASIZE 1 // I2C ��Ž� datasize ��û
#define DATA 2     // I2C ��Ž� data ��û

String buf = "";  // ��/���� ������ ���� ����
int dataSize = 0; // ���� ������ ������ ����
bool update = false;

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

X509List ca(ca_str);
X509List cert(cert_str);
PrivateKey key(key_str);
WiFiClientSecure wifiClient;
PubSubClient client(host, 8883, callback, wifiClient); // MQTT ��Ʈ��ȣ�� 8883���� ����, callback �Լ� ���, wificlient ��ü ����

long lastMsg = 0;
char msg[50];
int value = 0;
String pub = "";
char *cpub = NULL;

void setup()
{
  Serial.begin(9600); // begin serial for debug //
  Wire.begin(D1, D2); // join i2c bus with SDA=D1 and SCL=D2 of NodeMCU
  Serial.setDebugOutput(true);
  /* u8x8.begin();
    u8x8.setPowerSave(0); */
  Serial.println("Start, NodeMcu !");

  delay(1000);
  // Salve�� ������ �� �Ǿ����� Ȯ��
  Wire.beginTransmission(8); // begin with device address 8
  Wire.write("Connect...");  // ���� �׽�Ʈ ���ڿ� ����
  Serial.println("Connect...");
  Wire.endTransmission(); // stop transmitting //
  delay(1000);
  Wire.requestFrom(8, 15); // request & read data of size 13 from slave //

  readTransferData(); // ���۵� �����͸� �а� ���ۿ� ����
  if (buf == "Connect Success")
    Serial.println("Connect Complete !");
  else
    Serial.println("Connect Fail");
  buf = "";

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  wifiClient.setTrustAnchors(&ca);
  wifiClient.setClientRSACert(&cert, &key);
  Serial.println("Certifications and key are set");

  setClock();
  //client.setServer(host, 8883);
  client.setCallback(callback);
}

void loop()
{
  delay(2000);
  Wire.beginTransmission(8);   // begin with device address 8
  Wire.write("Update Status"); // ���������� ������Ʈ ���� Ȯ��
  Serial.println("Update Status...");
  Wire.endTransmission(); // stop transmitting
  //delay(1000);
  Wire.requestFrom(8, 2); // TR/FS
  readTransferData();     // ���۵� �����͸� �а� ���ۿ� ����
  Serial.println("Update Status : " + buf);
  if (buf == "TR")
  {
    update = true;
    Serial.println("Getting Data...");
  }
  else
  {
    if (buf == "FS")
      Serial.println("Wait...");
    else
      Serial.println("Reconnecting...");
    update = false;
    delay(12000);
  }
  buf = "";

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  if (update)
  {

    getSensorData(TPH, DATASIZE);
    delay(2000);
    getSensorData(TPH, DATA);
    cpub = new char[pub.length() + 1];
    strcpy(cpub, pub.c_str());
    client.publish("outTopic", cpub); // ���� �Խ�
    Serial.println("Publish[TPH Data] : Complete !");
    delete[] cpub;
    delay(2000);

    getSensorData(UTW, DATASIZE);
    delay(2000);
    getSensorData(UTW, DATA);
    cpub = new char[pub.length() + 1];
    strcpy(cpub, pub.c_str());
    client.publish("outTopic", cpub); // ���� �Խ�
    Serial.println("Publish[UTW Data] : Complete !");
    delete[] cpub;
    delay(2000);

    getSensorData(DST, DATASIZE);
    delay(2000);
    getSensorData(DST, DATA);
    cpub = new char[pub.length() + 1];
    strcpy(cpub, pub.c_str());
    client.publish("outTopic", cpub); // ���� �Խ�
    Serial.println("Publish[DST Data] : Complete !");
    delete[] cpub;
    delay(2000);

    getSensorData(ILM, DATASIZE);
    delay(2000);
    getSensorData(ILM, DATA);
    cpub = new char[pub.length() + 1];
    strcpy(cpub, pub.c_str());
    client.publish("outTopic", cpub); // ���� �Խ�
    Serial.println("Publish[ILM Data] : Complete !");
    delete[] cpub;
    delay(2000);

    getSensorData(GAS, DATASIZE);
    delay(2000);
    getSensorData(GAS, DATA);
    cpub = new char[pub.length() + 1];
    strcpy(cpub, pub.c_str());
    client.publish("outTopic", cpub); // ���� �Խ�
    Serial.println("Publish[GAS Data] : Complete !");
    delete[] cpub;
    delay(2000);

    getSensorData(PSS, DATASIZE);
    delay(2000);
    getSensorData(PSS, DATA);
    cpub = new char[pub.length() + 1];
    strcpy(cpub, pub.c_str());
    client.publish("outTopic", cpub); // ���� �Խ�
    Serial.println("Publish[PSS Data] : Complete !");
    delete[] cpub;
    delay(2000);

    Serial.println("Data Update Complete !");
    buf = "";
    update = false;
    delay(5000); // Slave�� ��ſ��� ������ ���� ���ð� ����

    /* if (!client.connected())
        {
            reconnect();
        }
        client.loop();

        long now = millis();
        if (now - lastMsg > 5000)
        {
            lastMsg = now;
            ++value;
            snprintf(msg, 75, "SensorData Example #%ld", value);
            Serial.print("Publish message: ");
            Serial.println(msg);
            client.publish("outTopic", msg); // ���� �Խ�
            Serial.print("Heap: ");
            Serial.println(ESP.getFreeHeap()); //Low heap can cause problems
        } */
  }
}

void readTransferData() // ���۵� �����͸� �а� ���ۿ� ����
{
  while (Wire.available())
  {
    char c = Wire.read();
    //Serial.print(c);
    buf += c;
  }
  //Serial.println();
}

void getSensorData(String dataType, int seq)
{

  if (dataType == TPH) // �½�������
  {
    if (seq == DATASIZE)
    {
      Wire.beginTransmission(8);      // begin with device address 8
      Wire.write("GET TPH DataSize"); // �½������� ������ ������ ������û
      Serial.println("Transmission GET TPH DataSize...");
      Wire.endTransmission(); // stop transmitting
      delay(2000);
      Wire.requestFrom(8, 2); // �½��������� ��� ����+�µ� ������ �Բ� ���۵Ǳ� ������ 2�ڸ���
      readTransferData();     // ���۵� �����͸� �а� ���ۿ� ����
      dataSize = buf.toInt();
      Serial.println("TPH DataSize : " + buf);
    }
    else if (seq == DATA && dataSize != 0)
    {
      Wire.beginTransmission(8); // begin with device address 8
      Wire.write("GET TPH");     // �½������� ������ ��û
      Serial.println("Transmission GET TPH...");
      Wire.endTransmission(); // stop transmitting
      delay(2000);
      Wire.requestFrom(8, dataSize); // request & read data
      readTransferData();            // ���۵� �����͸� �а� ���ۿ� ����
      Serial.println("Humi : " + buf.substring(0, 5) + "%");
      Serial.println("Temp : " + buf.substring(5) + "C");
      // AWS IOT ���� �Խø� ���� ���ڿ� ����
      pub = "Humi : " + buf.substring(0, 5) + "%\nTemp : " + buf.substring(5) + "C";
    }
  }
  else if (dataType == UTW) // �����ļ���
  {
    if (seq == DATASIZE)
    {
      Wire.beginTransmission(8);      // begin with device address 8
      Wire.write("GET UTW DataSize"); // �����ļ��� ������ ������ ������û
      Serial.println("Transmission GET UTW DataSize...");
      Wire.endTransmission(); // stop transmitting
      delay(2000);
      Wire.requestFrom(8, 1); // ������ �����ŭ ��û
      readTransferData();     // ���۵� �����͸� �а� ���ۿ� ����
      dataSize = buf.toInt();
      Serial.println("UTW DataSize : " + buf);
    }
    else if (seq == DATA && dataSize != 0)
    {
      Wire.beginTransmission(8); // begin with device address 8
      Wire.write("GET UTW");     // �����ļ��� ������ ��û
      Serial.println("Transmission GET UTW...");
      Wire.endTransmission(); // stop transmitting
      delay(2000);
      Wire.requestFrom(8, dataSize); // request & read data
      readTransferData();            // ���۵� �����͸� �а� ���ۿ� ����
      Serial.println("Distance : " + buf + "CM");
      // AWS IOT ���� �Խø� ���� ���ڿ� ����
      pub = "Distance : " + buf + "CM";
    }
  }
  else if (dataType == GAS) // ��������
  {
    if (seq == DATASIZE)
    {
      Wire.beginTransmission(8);      // begin with device address 8
      Wire.write("GET GAS DataSize"); // �������� ������ ������ ������û
      Serial.println("Transmission GET GAS DataSize...");
      Wire.endTransmission(); // stop transmitting
      delay(2000);
      Wire.requestFrom(8, 1); // ������ �����ŭ ��û
      readTransferData();     // ���۵� �����͸� �а� ���ۿ� ����
      dataSize = buf.toInt();
      Serial.println("GAS DataSize : " + buf);
    }
    else if (seq == DATA && dataSize != 0)
    {
      Wire.beginTransmission(8); // begin with device address 8
      Wire.write("GET GAS");     // �ϻ�ȭź�� ������ ��û
      Serial.println("Transmission GET GAS...");
      Wire.endTransmission(); // stop transmitting
      delay(2000);
      Wire.requestFrom(8, dataSize); // request & read data
      readTransferData();            // ���۵� �����͸� �а� ���ۿ� ����
      Serial.println("CarbonMonoxide : " + buf + "ppm");
      // AWS IOT ���� �Խø� ���� ���ڿ� ����
      pub = "CarbonMonoxide : " + buf + "ppm";
    }
  }
  else if (dataType == DST) // �̼���������
  {
    if (seq == DATASIZE)
    {
      Wire.beginTransmission(8);      // begin with device address 8
      Wire.write("GET DST DataSize"); // �̼����� ������ ������ ������û
      Serial.println("Transmission GET DST DataSize...");
      Wire.endTransmission(); // stop transmitting
      delay(2000);
      Wire.requestFrom(8, 1); // ������ �����ŭ ��û
      readTransferData();     // ���۵� �����͸� �а� ���ۿ� ����
      dataSize = buf.toInt();
      Serial.println("DST DataSize : " + buf);
    }
    else if (seq == DATA && dataSize != 0)
    {
      Wire.beginTransmission(8); // begin with device address 8
      Wire.write("GET DST");     // �̼��������� ������ ��û
      Serial.println("Transmission GET DST...");
      Wire.endTransmission(); // stop transmitting
      delay(2000);
      Wire.requestFrom(8, dataSize); // request & read data
      readTransferData();            // ���۵� �����͸� �а� ���ۿ� ����
      Serial.println("MicroDust : " + buf + "ug/m^3");
      // AWS IOT ���� �Խø� ���� ���ڿ� ����
      pub = "MicroDust : " + buf + "ug/m^3";
    }
  }
  else if (dataType == ILM) // ��������
  {
    if (seq == DATASIZE)
    {
      Wire.beginTransmission(8);      // begin with device address 8
      Wire.write("GET ILM DataSize"); // �������� ������ ������ ������û
      Serial.println("Transmission GET ILM DataSize...");
      Wire.endTransmission(); // stop transmitting
      delay(2000);
      Wire.requestFrom(8, 1); // ������ �����ŭ ��û
      readTransferData();     // ���۵� �����͸� �а� ���ۿ� ����
      dataSize = buf.toInt();
      Serial.println("ILM DataSize : " + buf);
    }
    else if (seq == DATA && dataSize != 0)
    {
      Wire.beginTransmission(8); // begin with device address 8
      Wire.write("GET ILM");     // ���� ������ ��û
      Serial.println("Transmission GET ILM...");
      Wire.endTransmission(); // stop transmitting
      delay(2000);
      Wire.requestFrom(8, dataSize); // request & read data
      readTransferData();            // ���۵� �����͸� �а� ���ۿ� ����
      Serial.println("Illumination : " + buf + "LX");
      // AWS IOT ���� �Խø� ���� ���ڿ� ����
      pub = "Illumination : " + buf + "LX";
    }
  }
  else if (dataType == PSS) // ���м���
  {
    if (seq == DATASIZE)
    {
      Wire.beginTransmission(8);      // begin with device address 8
      Wire.write("GET PSS DataSize"); // ���м��� ������ ������ ������û
      Serial.println("Transmission GET PSS DataSize...");
      Wire.endTransmission(); // stop transmitting
      delay(2000);
      Wire.requestFrom(8, 2); // ������ �����ŭ ��û / ���м����� ��� �µ�, ���, �� �����͸� �ѹ��� ����
      readTransferData();     // ���۵� �����͸� �а� ���ۿ� ����
      dataSize = buf.toInt();
      Serial.println("PSS DataSize : " + buf);
    }
    else if (seq == DATA && dataSize != 0)
    {
      Wire.beginTransmission(8); // begin with device address 8
      Wire.write("GET PSS");     // �µ�, ���, �� ������ ��û
      Serial.println("Transmission GET PSS...");
      Wire.endTransmission(); // stop transmitting
      delay(2000);
      Wire.requestFrom(8, dataSize);                            // request & read data
      readTransferData();                                       // ���۵� �����͸� �а� ���ۿ� ����
      Serial.println("Temp : " + buf.substring(0, 5) + "C");    // �µ����
      Serial.println("Press : " + buf.substring(5, 14) + "PA"); // ������
      Serial.println("Alti : " + buf.substring(14));            // �����
      // AWS IOT ���� �Խø� ���� ���ڿ� ����
      pub = "Temp : " + buf.substring(0, 5) + "C\nPress : " + buf.substring(5, 14) + "PA\nAlti : " + buf.substring(14);
    }
  }
  buf = "";
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(thingId))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      char buf[256];
      wifiClient.getLastSSLError(buf, 256);
      Serial.print("WiFiClientSecure SSL error: ");
      Serial.println(buf);

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setClock()
{
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}