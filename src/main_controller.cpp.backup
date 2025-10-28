/*
 * Mini Security System - ESP32 Controller (Main Board)
 * 
 * ESP32 sebagai controller utama yang mengontrol sensor PIR, LED, switch,
 * dan berkomunikasi dengan ESP32-CAM untuk pengambilan gambar.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Pin definitions
#define PIR_SENSOR_PIN    13
#define LED_INDICATOR_PIN 33
#define SWITCH_PIN        12
#define RXD2 16
#define TXD2 17

// WiFi credentials
const char* ssid = "Adhyasta";
const char* password = "juarasatu";
const char* serverURL = "http://127.0.1.1:5000/upload";

// Global variables
bool systemArmed = true;
bool motionDetected = false;
int imageCount = 0;
unsigned long lastMonitorTime = 0;
const unsigned long monitorInterval = 10000;
unsigned long lastSwitchTime = 0;
const unsigned long debounceDelay = 200;
unsigned long lastPIRTime = 0;
const unsigned long pirDebounceDelay = 5000;

#define MAX_IMAGE_SIZE 60000
uint8_t imageBuffer[MAX_IMAGE_SIZE];
size_t imageSize = 0;

// Function declarations
void initWiFi();
void initPins();
void initCameraSerial();
bool requestImageFromCAM();
bool receiveImageData();
bool uploadImageToServer(uint8_t* imageData, size_t imageLen);
void handleMotionDetection();
void handleSystemSwitch();
void displayStatus();

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("========================================");
  Serial.println("  Mini Security System - ESP32 Controller");
  Serial.println("========================================");
  
  initPins();
  initCameraSerial();
  initWiFi();
  
  Serial.println("  Sistem Siap!");
  displayStatus();
}

void loop() {
  handleSystemSwitch();
  
  if (systemArmed) {
    handleMotionDetection();
    
    if (millis() - lastMonitorTime >= monitorInterval) {
      Serial.println("[STATUS] Monitoring...");
      lastMonitorTime = millis();
    }
  }
  
  delay(100);
}

void initPins() {
  Serial.println("[INIT] Menginisialisasi pin GPIO...");
  pinMode(PIR_SENSOR_PIN, INPUT);
  pinMode(LED_INDICATOR_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  digitalWrite(LED_INDICATOR_PIN, LOW);
  Serial.println("[INIT] Pin GPIO berhasil diinisialisasi");
}

void initCameraSerial() {
  Serial.println("[INIT] Menginisialisasi komunikasi dengan ESP32-CAM...");
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  delay(1000);
  
  Serial2.println("TEST");
  delay(500);
  
  if (Serial2.available()) {
    String response = Serial2.readStringUntil('\n');
    if (response.indexOf("READY") >= 0) {
      Serial.println("[INIT] ESP32-CAM berhasil terhubung!");
    } else {
      Serial.println("[WARNING] ESP32-CAM merespons tapi status tidak jelas");
    }
  } else {
    Serial.println("[WARNING] ESP32-CAM tidak merespons!");
    Serial.println("[WARNING] Pastikan ESP32-CAM sudah diprogram dan terhubung");
  }
}

void initWiFi() {
  Serial.println("[INIT] Menghubungkan ke WiFi...");
  Serial.print("[INIT] SSID: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[INIT] WiFi berhasil terhubung!");
    Serial.print("[INIT] IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("[ERROR] WiFi gagal terhubung!");
  }
}

bool requestImageFromCAM() {
  Serial.println("[CAMERA] Mengirim perintah ke ESP32-CAM...");
  Serial2.println("CAPTURE");
  
  unsigned long startTime = millis();
  while (millis() - startTime < 5000) {
    if (Serial2.available()) {
      String response = Serial2.readStringUntil('\n');
      response.trim();
      
      if (response.indexOf("OK") >= 0) {
        Serial.println("[CAMERA] ESP32-CAM siap mengirim data");
        return receiveImageData();
      } else if (response.indexOf("ERROR") >= 0) {
        Serial.println("[ERROR] ESP32-CAM gagal mengambil foto");
        return false;
      }
    }
    delay(10);
  }
  
  Serial.println("[ERROR] Timeout menunggu response");
  return false;
}

bool receiveImageData() {
  Serial.println("[CAMERA] Menerima data gambar...");
  imageSize = 0;
  unsigned long startTime = millis();
  bool receivingData = false;
  
  while (millis() - startTime < 10000) {
    if (Serial2.available()) {
      if (!receivingData) {
        String header = Serial2.readStringUntil(':');
        if (header.indexOf("SIZE") >= 0) {
          imageSize = Serial2.parseInt();
          Serial2.read();
          
          if (imageSize > 0 && imageSize <= MAX_IMAGE_SIZE) {
            Serial.printf("[CAMERA] Ukuran gambar: %d bytes\n", imageSize);
            receivingData = true;
            startTime = millis();
          } else {
            Serial.println("[ERROR] Ukuran gambar tidak valid!");
            return false;
          }
        }
      } else {
        size_t bytesRead = Serial2.readBytes(imageBuffer, imageSize);
        
        if (bytesRead == imageSize) {
          Serial.printf("[CAMERA] Berhasil menerima %d bytes\n", bytesRead);
          imageCount++;
          return true;
        } else {
          Serial.printf("[ERROR] Data tidak lengkap! %d/%d\n", bytesRead, imageSize);
          return false;
        }
      }
    }
    delay(10);
  }
  
  Serial.println("[ERROR] Timeout menerima data");
  return false;
}

bool uploadImageToServer(uint8_t* imageData, size_t imageLen) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WARNING] WiFi tidak terhubung");
    return false;
  }
  
  Serial.println("[HTTP] Mengirim foto ke server...");
  HTTPClient http;
  http.begin(serverURL);
  http.addHeader("Content-Type", "image/jpeg");
  
  int httpResponseCode = http.POST(imageData, imageLen);
  
  if (httpResponseCode > 0) {
    Serial.printf("[HTTP] Response code: %d\n", httpResponseCode);
    String response = http.getString();
    Serial.printf("[HTTP] Response: %s\n", response.c_str());
    http.end();
    return true;
  } else {
    Serial.printf("[ERROR] HTTP POST gagal: %s\n", http.errorToString(httpResponseCode).c_str());
    http.end();
    return false;
  }
}

void handleMotionDetection() {
  int pirState = digitalRead(PIR_SENSOR_PIN);
  
  if (pirState == HIGH && !motionDetected) {
    if (millis() - lastPIRTime >= pirDebounceDelay) {
      motionDetected = true;
      lastPIRTime = millis();
      
      Serial.println();
      Serial.println("========================================");
      Serial.println("[ALERT] Motion Detected!");
      Serial.println("========================================");
      
      digitalWrite(LED_INDICATOR_PIN, HIGH);
      Serial.println("[ACTION] LED dihidupkan");
      
      bool success = requestImageFromCAM();
      
      if (success) {
        Serial.println("[SUCCESS] Image Captured");
        uploadImageToServer(imageBuffer, imageSize);
      } else {
        Serial.println("[ERROR] Gagal mengambil gambar");
      }
      
      delay(1000);
      digitalWrite(LED_INDICATOR_PIN, LOW);
      Serial.println("[ACTION] LED dimatikan");
      Serial.println("========================================");
    }
  }
  
  if (pirState == LOW && motionDetected) {
    motionDetected = false;
  }
}

void handleSystemSwitch() {
  int switchState = digitalRead(SWITCH_PIN);
  
  if (switchState == LOW) {
    if (millis() - lastSwitchTime >= debounceDelay) {
      systemArmed = !systemArmed;
      lastSwitchTime = millis();
      
      Serial.println();
      Serial.println("========================================");
      displayStatus();
      Serial.println("========================================");
      
      if (!systemArmed) {
        digitalWrite(LED_INDICATOR_PIN, LOW);
      }
      
      lastMonitorTime = millis();
    }
  }
}

void displayStatus() {
  if (systemArmed) {
    Serial.println("[STATUS] System Armed - Monitoring aktif");
  } else {
    Serial.println("[STATUS] System Disarmed - Monitoring tidak aktif");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("[STATUS] WiFi Connected - IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("[STATUS] WiFi Disconnected");
  }
  
  Serial.printf("[STATUS] Images captured: %d\n", imageCount);
}
