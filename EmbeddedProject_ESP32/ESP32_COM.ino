#include <WiFi.h>
#include <WebSocketsServer.h>
#include <SPI.h>
#include <ArduinoJson.h>

// Wi-Fi 설정
const char* ssid = "ESP32_SSID";       // ESP32의 Wi-Fi SSID
const char* password = "password123";  // ESP32의 Wi-Fi 비밀번호

//SPI 설정
#define VSPI_MISO MISO //19
#define VSPI_MOSI MOSI //23
#define VSPI_SCLK SCK  //18
#define VSPI_SS   SS   //5

static const int spiClk = 1000000;  // 1 MHz
SPIClass *vspi = NULL;
uint32_t txBuf1 = 0;  // 초기값 0으로 설정
uint32_t rxBuf1 = 0;
unsigned long lastSPISendTime = 0;  // 마지막 SPI 송신 시간
const unsigned long spiInterval = 1000; // 주기 (ms)

// WebSocket 서버 포트
WebSocketsServer webSocket(81);

// 32비트 변수에 ID, signal_num, value 값 저장
void encodeData(uint16_t id, uint8_t signal_num, uint8_t value) {
  txBuf1 = ((id & 0xFFF) << 16) | ((signal_num & 0xF) << 8) | (value & 0xFF);

  // 디버깅 출력
  Serial.printf("인코딩된 txBuf1: 0x%08X\n", txBuf1);
}

// SPI 데이터 송신
void spiCommand(SPIClass *spi) {
  spi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(spi->pinSS(), LOW);
  rxBuf1 = spi->transfer32(txBuf1);
  digitalWrite(spi->pinSS(), HIGH);
  spi->endTransaction();
}

// WebSocket 이벤트 처리 함수
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  if (type == WStype_TEXT) {
    // JSON 메시지 파싱 준비
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, payload, length);
    if (error) {
      Serial.printf("JSON 파싱 실패: %s\n", error.c_str());
      return;
    }

    // JSON 데이터 추출
    uint16_t id = doc["id"];                   // CAN 메시지 ID (16진수 정수)
    int signal_num = doc["signal_num"];        // 신호 고유 번호
    int value = doc["value"];                 // CAN 데이터 값

    // 디버깅 출력
    Serial.printf("수신 메시지 - ID: 0x%X, Signal Num: %d, Value: %d\n", id, signal_num, value);

    // txBuf1 업데이트
    encodeData(id, signal_num, value);

    // SPI 송신
    spiCommand(vspi);
  }
}

void setup() {
  Serial.begin(115200);

  // Wi-Fi SoftAP 모드 시작
  WiFi.softAP(ssid, password);
  Serial.print("SoftAP IP 주소: ");
  Serial.println(WiFi.softAPIP());

  // WebSocket 서버 시작
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket 서버가 시작되었습니다.");

  //SPI 설정
  vspi = new SPIClass(VSPI);
  vspi->begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI, VSPI_SS);
  pinMode(vspi->pinSS(), OUTPUT);
  digitalWrite(VSPI_SS, HIGH);

  // 초기 SPI 데이터 송신
  spiCommand(vspi);
}

void loop() {
  // WebSocket 클라이언트 관리
  webSocket.loop();

  // 주기적으로 SPI 데이터 송신
  if (millis() - lastSPISendTime >= spiInterval) {
    lastSPISendTime = millis();

    // SPI 송신
    spiCommand(vspi);

    // txBuf1을 초기화 (0으로 채움)
    txBuf1 = 0;
  }
}