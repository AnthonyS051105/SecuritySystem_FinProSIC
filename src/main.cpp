/*
 * Mini Security System - ESP32 Controller (Main Board)
 * 
 * ESP32 sebagai controller utama yang mengontrol sensor PIR, LED, switch,
 * dan berkomunikasi dengan ESP32-CAM untuk pengambilan gambar.
 * Dilengkapi OLED display untuk menampilkan status sistem.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin definitions
#define PIR_SENSOR_PIN    13
#define LED_INDICATOR_PIN 33
#define SWITCH_PIN        12
#define RXD2 16
#define TXD2 17

// WiFi credentials
const char* ssid = "Adhyasta";
const char* password = "juarasatu";
// ⚠️ GANTI IP INI dengan IP komputer yang menjalankan server!
// Jalankan: hostname -I
// Contoh: "http://192.168.1.100:5000/upload"
const char* serverURL = "http://10.137.208.149:5000/upload";  // GANTI IP INI!

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
void initOLED();
void updateOLED(String line1, String line2 = "", String line3 = "", String line4 = "");
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
  initOLED();  // Inisialisasi OLED
  
  updateOLED("System Starting", "Please wait...");
  delay(1000);
  
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
      updateOLED("Status:", "Monitoring...", 
                 WiFi.status() == WL_CONNECTED ? "WiFi: OK" : "WiFi: NO",
                 String("Images: ") + String(imageCount));
      lastMonitorTime = millis();
    }
  }
  
  delay(100);
}

/**
 * Inisialisasi OLED Display
 */
void initOLED() {
  Serial.println("[INIT] Menginisialisasi OLED display...");
  
  // Inisialisasi I2C untuk OLED (SDA=21, SCL=22 default ESP32)
  Wire.begin();
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("[ERROR] OLED tidak ditemukan!");
    Serial.println("[ERROR] Cek wiring: SDA=GPIO21, SCL=GPIO22, VCC=3.3V, GND=GND");
    // Lanjutkan tanpa OLED
    return;
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Security System");
  display.println("Initializing...");
  display.display();
  
  Serial.println("[INIT] OLED berhasil diinisialisasi");
}

/**
 * Update tampilan OLED
 */
void updateOLED(String line1, String line2, String line3, String line4) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Line 1 (Besar)
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.println(line1);
  
  // Line 2-4 (Kecil)
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.println(line2);
  display.setCursor(0, 35);
  display.println(line3);
  display.setCursor(0, 50);
  display.println(line4);
  
  display.display();
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
  updateOLED("Init CAM", "Connecting...");
  
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  delay(1000);
  
  Serial2.println("TEST");
  delay(500);
  
  if (Serial2.available()) {
    String response = Serial2.readStringUntil('\n');
    if (response.indexOf("READY") >= 0) {
      Serial.println("[INIT] ESP32-CAM berhasil terhubung!");
      updateOLED("CAM", "Connected!", "", "");
      delay(1000);
    } else {
      Serial.println("[WARNING] ESP32-CAM merespons tapi status tidak jelas");
      updateOLED("CAM", "Unknown status", "", "");
      delay(1000);
    }
  } else {
    Serial.println("[WARNING] ESP32-CAM tidak merespons!");
    Serial.println("[WARNING] Pastikan ESP32-CAM sudah diprogram dan terhubung");
    updateOLED("CAM ERROR", "Not responding", "Check wiring", "");
    delay(2000);
  }
}

void initWiFi() {
  Serial.println("[INIT] Menghubungkan ke WiFi...");
  Serial.print("[INIT] SSID: ");
  Serial.println(ssid);
  
  updateOLED("WiFi", String("SSID: ") + String(ssid), "Connecting...");
  
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
    
    updateOLED("WiFi OK", WiFi.localIP().toString(), "", "");
    delay(1500);
  } else {
    Serial.println("[ERROR] WiFi gagal terhubung!");
    Serial.println("[ERROR] Periksa SSID dan password");
    Serial.println("[ERROR] Pastikan WiFi 2.4GHz (bukan 5GHz)");
    
    updateOLED("WiFi ERROR", "Check SSID/Pass", "Use 2.4GHz", "");
    delay(2000);
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
      
      updateOLED("MOTION!", "Detected", "Capturing...", "");
      
      digitalWrite(LED_INDICATOR_PIN, HIGH);
      Serial.println("[ACTION] LED dihidupkan");
      
      bool success = requestImageFromCAM();
      
      if (success) {
        Serial.println("[SUCCESS] Image Captured");
        uploadImageToServer(imageBuffer, imageSize);
        updateOLED("Image Sent", String("Count: ") + String(imageCount), "", "");
        delay(1500);
      } else {
        Serial.println("[ERROR] Gagal mengambil gambar");
        updateOLED("CAM ERROR", "Capture Failed", "", "");
        delay(2000);
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
      
      // Update OLED with system status
      if (systemArmed) {
        updateOLED("SYSTEM", "ARMED", "Ready to Detect", "");
      } else {
        updateOLED("SYSTEM", "DISARMED", "Standby Mode", "");
      }
      delay(1500);
      
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
  
  // Update OLED with comprehensive status
  String line1 = systemArmed ? "ARMED" : "DISARMED";
  String line2 = WiFi.status() == WL_CONNECTED ? "WiFi: OK" : "WiFi: FAIL";
  String line3 = String("Images: ") + String(imageCount);
  String line4 = "";
  
  updateOLED(line1, line2, line3, line4);
}
