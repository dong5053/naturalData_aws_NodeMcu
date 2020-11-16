// Master
#include <Wire.h>
#include <string.h>

// Example of the different modes of the X.509 validation options
// in the WiFiClientBearSSL object
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <time.h>

const char *ssid = "Your_SSID";                              // 와이파이 이름
const char *pass = "PASSWORD";                               // 와이파이 비밀번호
const char *thingId = "사물이름";                            // 사물 이름 (thing ID)
const char *host = "Your_HTTPS_URL-ats.iot.ap-northeast-2.amazonaws.com"; // AWS IoT Core 주소

// 사물 인증서 (파일 이름: xxxxxxxxxx-certificate.pem.crt)
const char cert_str[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
// 여기에 인증서 내용 복사
-----END CERTIFICATE-----
)EOF";
// 사물 인증서 프라이빗 키 (파일 이름: xxxxxxxxxx-private.pem.key)
const char key_str[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
// 여기에 인증서 내용 복사
-----END RSA PRIVATE KEY-----
)EOF";
// Amazon Trust Services(ATS) 엔드포인트 CA 인증서 (서버인증 > "RSA 2048비트 키: Amazon Root CA 1" 다운로드)
const char ca_str[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
// 여기에 인증서 내용 복사
-----END CERTIFICATE-----
)EOF";

// I2C Data Type
#define DST "DST" // 미센먼지 데이터
#define UTW "UTW" // 초음파센서 데이터
#define TPH "TPH" // 온습도센서 데이터
#define ILM "ILM" // 조도센서 데이터
#define GAS "GAS" // 가스센서 데이터
#define PSS "PSS" // 대기압센서 데이터

#define DATASIZE 1      // I2C 통신시 datasize 요청
#define DATA 2          // I2C 통신시 data 요청

String buf = "";         // 송/수신 데이터 저장 버퍼
int dataSize = 0;        // 센서 데이터 사이즈 정보
bool update = false;     // 슬레이브로부터 센서 데이터 업데이트 완료 유무

void callback(char *topic, byte *payload, unsigned int length)  // AWS IoT Core 주제게시시 호출될 함수
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++)
        Serial.print((char)payload[i]);
    
    Serial.println();
}
// AWS IoT 사물 인증서
X509List ca(ca_str);
X509List cert(cert_str);
PrivateKey key(key_str);
WiFiClientSecure wifiClient;
PubSubClient client(host, 8883, callback, wifiClient); // MQTT 포트번호를 8883으로 셋팅, callback 함수 등록, wificlient 객체 연결

String pub = "";        // AWS IoT 데이터 전송을 위한 임시 저장소
char * cpub = NULL;     // AWS IoT 데아터 전송을 위한 변수

void setup()
{
    Serial.begin(9600); // 시리얼 통신속도 9600으로 설정
    Wire.begin(D1, D2); // Wire.begin(SDA, SCL) I2C 통신을 위한 핀 지정, NodeMcu의 경우 D1 = SCL, D2 = SDA(반대로 연결)
    Serial.setDebugOutput(true);
    Serial.println("Start, NodeMcu !");

    delay(1000);
    // Salve와 연결이 잘 되었는지 확인
    Wire.beginTransmission(8); // 8번 슬레이브를 데이터 전송할 슬레이브로 지정
    Wire.write("Connect...");  // 연결 테스트 문자열 전송
    Serial.println("Connect...");
    Wire.endTransmission(); // 전송 종료
    delay(1000);
    Wire.requestFrom(8, 15); // 8번 슬레이브로 15바이트의 데이터 요청

    readTransferData(); // 전송된 데이터를 읽고 버퍼에 저장
    if (buf == "Connect Success")
        Serial.println("Connect Complete !");
    else
        Serial.println("Connect Fail");
    buf = "";

    // WiFi 연결
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);        // Station mode, Stand-alone mode로 설정
    WiFi.begin(ssid, pass);     // WiFi 연결 시도

    while (WiFi.status() != WL_CONNECTED)       // WiFi 연결이 될 때까지 . 출력
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());     // 연결된 IP 주소 출력

    wifiClient.setTrustAnchors(&ca);                // CA 인증서 등록
    wifiClient.setClientRSACert(&cert, &key);       // 사물 인증서 등록
    Serial.println("Certifications and key are set");

    setClock();                 // 현재 시간 동기화
    client.setCallback(callback);   // callback 함수 등록
}

void loop()
{
    delay(2000);
    Wire.beginTransmission(8);   // 8번 슬레이브를 데이터 전송할 슬레이브로 지정
    Wire.write("Update Status"); // 센서데이터 업데이트 유무 확인
    Serial.println("Update Status...");
    Wire.endTransmission(); // 전송 종료
    Wire.requestFrom(8, 2); // 8번 슬레이브에게 2바이트 데이터 요청 (TR/FS)
    readTransferData();     // 전송된 데이터를 읽고 버퍼에 저장
    Serial.println("Update Status : " + buf);
    if (buf == "TR")        // 슬레이브 센서 데이터 업데이트 완료시
    {
        update = true;
        Serial.println("Getting Data...");
    }
    else
    {
        if(buf == "FS")     // 슬레이브 센서 데이터 업데이트 미완료시 대기
            Serial.println("Wait...");
        else
            Serial.println("Reconnecting...");
        update = false;
        delay(12000);
    }
    buf = "";

    if (!client.connected())        // AWS IoT Service에 연결 (MQTT 사용)
        reconnect();
    client.loop();

    if (update)         // 슬레이브가 센서 데이터를 업데이트 완료하면 마스터로 데이터 가져오기
    {
        getSensorData(TPH, DATASIZE);       // 온습도센서 데이터 사이즈 요청
        delay(2000);
        getSensorData(TPH, DATA);           // 온습도센서 데이터 요청
        cpub = new char[pub.length() + 1];
        strcpy(cpub, pub.c_str());
        client.publish("outTopic", cpub); // 주제 게시
        Serial.println("Publish[TPH Data] : Complete !");
        delete [] cpub;
        delay(2000);

        getSensorData(UTW, DATASIZE);       // 초음파센서 데이터 사이즈 요청
        delay(2000);
        getSensorData(UTW, DATA);           // 초음파센서 데이터 요청
        cpub = new char[pub.length() + 1];
        strcpy(cpub, pub.c_str());
        client.publish("outTopic", cpub); // 주제 게시
        Serial.println("Publish[UTW Data] : Complete !");
        delete [] cpub;
        delay(2000);

        getSensorData(DST, DATASIZE);       // 미세먼지센서 데이터 사이즈 요청
        delay(2000);
        getSensorData(DST, DATA);           // 미세먼지센서 데이터 요청
        cpub = new char[pub.length() + 1];
        strcpy(cpub, pub.c_str());
        client.publish("outTopic", cpub); // 주제 게시
        Serial.println("Publish[DST Data] : Complete !");
        delete [] cpub;
        delay(2000);

        getSensorData(ILM, DATASIZE);       // 조도센서 데이터 사이즈 요청
        delay(2000);
        getSensorData(ILM, DATA);           // 조도센서 데이터 요청
        cpub = new char[pub.length() + 1];
        strcpy(cpub, pub.c_str());
        client.publish("outTopic", cpub); // 주제 게시
        Serial.println("Publish[ILM Data] : Complete !");
        delete [] cpub;
        delay(2000);

        getSensorData(GAS, DATASIZE);       // 가스센서 데이터 사이즈 요청
        delay(2000);
        getSensorData(GAS, DATA);           // 가스센서 데이터 요청
        cpub = new char[pub.length() + 1];
        strcpy(cpub, pub.c_str());
        client.publish("outTopic", cpub); // 주제 게시
        Serial.println("Publish[GAS Data] : Complete !");
        delete [] cpub;
        delay(2000);

        getSensorData(PSS, DATASIZE);       // 대기압센서 데이터 사이즈 요청
        delay(2000);
        getSensorData(PSS, DATA);           // 대기압센서 데이터 요청
        cpub = new char[pub.length() + 1];
        strcpy(cpub, pub.c_str());
        client.publish("outTopic", cpub); // 주제 게시
        Serial.println("Publish[PSS Data] : Complete !");
        delete [] cpub;
        delay(2000);

        Serial.println("Data Update Complete !");
        buf = "";
        update = false;
        delay(5000); // Slave와 통신오류 방지를 위해 대기시간 설정
    }
}

void readTransferData() // 전송된 데이터를 읽고 버퍼에 저장
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
    
    if (dataType == TPH) // 온습도센서
    {
        if (seq == DATASIZE)
        {
            Wire.beginTransmission(8);      // 8번 슬레이브를 데이터 전송할 슬레이브로 지정
            Wire.write("GET TPH DataSize"); // 온습도센서 데이터 사이즈 정보요청
            Serial.println("Transmission GET TPH DataSize...");
            Wire.endTransmission(); // 전송 종료
            delay(2000);
            Wire.requestFrom(8, 2); // 온습도센서의 경우 습도+온도 정보가 함께 전송되기 때문에 2자리수
            readTransferData();     // 전송된 데이터를 읽고 버퍼에 저장
            dataSize = buf.toInt();
            Serial.println("TPH DataSize : " + buf);
        }
        else if (seq == DATA && dataSize != 0)
        {
            Wire.beginTransmission(8); // 8번 슬레이브를 데이터 전송할 슬레이브로 지정
            Wire.write("GET TPH");     // 온습도센서 데이터 요청
            Serial.println("Transmission GET TPH...");
            Wire.endTransmission(); // 전송 종료
            delay(2000);
            Wire.requestFrom(8, dataSize); // 8번 슬레이브에게 온습도센서 데이터 요청
            readTransferData();            // 전송된 데이터를 읽고 버퍼에 저장
            Serial.println("Humi : " + buf.substring(0, 5) + "%");
            Serial.println("Temp : " + buf.substring(5) + "C");
            // AWS IOT 주제 게시를 위한 문자열 조합
            pub = "Humi : " + buf.substring(0, 5) + "%\nTemp : " + buf.substring(5) + "C";
        }
    }
    else if (dataType == UTW) // 초음파센서
    {
        if (seq == DATASIZE)
        {
            Wire.beginTransmission(8);      // 8번 슬레이브를 데이터 전송할 슬레이브로 지정
            Wire.write("GET UTW DataSize"); // 초음파센서 데이터 사이즈 정보요청
            Serial.println("Transmission GET UTW DataSize...");
            Wire.endTransmission(); // 전송 종료
            delay(2000);
            Wire.requestFrom(8, 1); // 8번 슬레이브에게 1바이트 데이터 요청
            readTransferData();     // 전송된 데이터를 읽고 버퍼에 저장
            dataSize = buf.toInt();
            Serial.println("UTW DataSize : " + buf);
        }
        else if (seq == DATA && dataSize != 0)
        {
            Wire.beginTransmission(8); // 8번 슬레이브를 데이터 전송할 슬레이브로 지정
            Wire.write("GET UTW");     // 초음파센서 데이터 요청
            Serial.println("Transmission GET UTW...");
            Wire.endTransmission(); // 전송 종료
            delay(2000);
            Wire.requestFrom(8, dataSize); // 8번 슬레이브에게 초음파센서 데이터 요청
            readTransferData();            // 전송된 데이터를 읽고 버퍼에 저장
            Serial.println("Distance : " + buf + "CM");
            // AWS IOT 주제 게시를 위한 문자열 조합
            pub = "Distance : " + buf + "CM";
        }
    }
    else if (dataType == GAS) // 가스센서
    {
        if (seq == DATASIZE)
        {
            Wire.beginTransmission(8);      // 8번 슬레이브를 데이터 전송할 슬레이브로 지정
            Wire.write("GET GAS DataSize"); // 가스센서 데이터 사이즈 정보요청
            Serial.println("Transmission GET GAS DataSize...");
            Wire.endTransmission(); // 전송 종료
            delay(2000);
            Wire.requestFrom(8, 1); // 8번 슬레이브에게 1바이트 데이터 요청
            readTransferData();     // 전송된 데이터를 읽고 버퍼에 저장
            dataSize = buf.toInt();
            Serial.println("GAS DataSize : " + buf);
        }
        else if (seq == DATA && dataSize != 0)
        {
            Wire.beginTransmission(8); // 8번 슬레이브를 데이터 전송할 슬레이브로 지정
            Wire.write("GET GAS");     // 일산화탄소 데이터 요청
            Serial.println("Transmission GET GAS...");
            Wire.endTransmission(); // 전송 종료
            delay(2000);
            Wire.requestFrom(8, dataSize); // 8번 슬레이브에게 일산화탄소 데이터 요청
            readTransferData();            // 전송된 데이터를 읽고 버퍼에 저장
            Serial.println("CarbonMonoxide : " + buf + "ppm");
            // AWS IOT 주제 게시를 위한 문자열 조합
            pub = "CarbonMonoxide : " + buf + "ppm";
        }
    }
    else if (dataType == DST) // 미세먼지센서
    {
        if (seq == DATASIZE)
        {
            Wire.beginTransmission(8);      // 8번 슬레이브를 데이터 전송할 슬레이브로 지정
            Wire.write("GET DST DataSize"); // 미세먼지 데이터 사이즈 정보요청
            Serial.println("Transmission GET DST DataSize...");
            Wire.endTransmission(); // 전송 종료
            delay(2000);
            Wire.requestFrom(8, 1); // 8번 슬레이브에게 1바이트 데이터 요청
            readTransferData();     // 전송된 데이터를 읽고 버퍼에 저장
            dataSize = buf.toInt();
            Serial.println("DST DataSize : " + buf);
        }
        else if (seq == DATA && dataSize != 0)
        {
            Wire.beginTransmission(8); // 8번 슬레이브를 데이터 전송할 슬레이브로 지정
            Wire.write("GET DST");     // 미세먼지센서 데이터 요청
            Serial.println("Transmission GET DST...");
            Wire.endTransmission(); // 전송 종료
            delay(2000);
            Wire.requestFrom(8, dataSize); // 8번 슬레이브에게 미세먼지센서 데이터 요청
            readTransferData();            // 전송된 데이터를 읽고 버퍼에 저장
            Serial.println("MicroDust : " + buf + "ug/m^3");
            // AWS IOT 주제 게시를 위한 문자열 조합
            pub = "MicroDust : " + buf + "ug/m^3";
        }
    }
    else if (dataType == ILM) // 조도센서
    {
        if (seq == DATASIZE)
        {
            Wire.beginTransmission(8);      // 8번 슬레이브를 데이터 전송할 슬레이브로 지정
            Wire.write("GET ILM DataSize"); // 조도센서 데이터 사이즈 정보요청
            Serial.println("Transmission GET ILM DataSize...");
            Wire.endTransmission(); // 전송 종료
            delay(2000);
            Wire.requestFrom(8, 1); // 8번 슬레이브에게 1바이트 데이터 요청
            readTransferData();     // 전송된 데이터를 읽고 버퍼에 저장
            dataSize = buf.toInt();
            Serial.println("ILM DataSize : " + buf);
        }
        else if (seq == DATA && dataSize != 0)
        {
            Wire.beginTransmission(8); // 8번 슬레이브를 데이터 전송할 슬레이브로 지정
            Wire.write("GET ILM");     // 조도 데이터 요청
            Serial.println("Transmission GET ILM...");
            Wire.endTransmission(); // 전송 종료
            delay(2000);
            Wire.requestFrom(8, dataSize); // 8번 슬레이브에게 조도 데이터 요청
            readTransferData();            // 전송된 데이터를 읽고 버퍼에 저장
            Serial.println("Illumination : " + buf + "LX");
            // AWS IOT 주제 게시를 위한 문자열 조합
            pub = "Illumination : " + buf + "LX";
        }
    }
    else if (dataType == PSS) // 대기압센서
    {
        if (seq == DATASIZE)
        {
            Wire.beginTransmission(8);      // 8번 슬레이브를 데이터 전송할 슬레이브로 지정
            Wire.write("GET PSS DataSize"); // 대기압센서 데이터 사이즈 정보요청
            Serial.println("Transmission GET PSS DataSize...");
            Wire.endTransmission(); // 전송 종료
            delay(2000);
            Wire.requestFrom(8, 2); // 8번 슬레이브에게 2바이트 데이터 요청 / 대기압센서의 경우 온도, 기압, 고도 데이터를 한번에 전송
            readTransferData();     // 전송된 데이터를 읽고 버퍼에 저장
            dataSize = buf.toInt();
            Serial.println("PSS DataSize : " + buf);
        }
        else if (seq == DATA && dataSize != 0)
        {
            Wire.beginTransmission(8); // 8번 슬레이브를 데이터 전송할 슬레이브로 지정
            Wire.write("GET PSS");     // 온도, 기압, 고도 데이터 요청
            Serial.println("Transmission GET PSS...");
            Wire.endTransmission(); // 전송 종료
            delay(2000);
            Wire.requestFrom(8, dataSize);                            // 8번 슬레이브에게 온도, 기압, 고도 데이터 요청
            readTransferData();                                       // 전송된 데이터를 읽고 버퍼에 저장
            Serial.println("Temp : " + buf.substring(0, 5) + "C");    // 온도출력
            Serial.println("Press : " + buf.substring(5, 14) + "PA"); // 기압출력
            Serial.println("Alti : " + buf.substring(14));            // 고도출력
            // AWS IOT 주제 게시를 위한 문자열 조합
            pub = "Temp : " + buf.substring(0, 5) + "C\nPress : " + buf.substring(5, 14) + "PA\nAlti : " + buf.substring(14);
        }
    }
    buf = "";
}

void reconnect()
{
    // 다시 연결될 때까지 반복
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // 연결 시도
        if (client.connect(thingId))
        {
            Serial.println("connected");
            // 연결되면 Topic 게시
            client.publish("outTopic", "hello world");
            // 해당 Topic 구독
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

            // 5초 대기 후 재연결
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

