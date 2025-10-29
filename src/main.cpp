/*
 * Mini Security System - ESP32 Controller dengan Webcam Laptop
 * 
 * ESP32 sebagai controller utama yang mengontrol sensor PIR, LED, switch, dan OLED.
 * Saat PIR mendeteksi gerakan, ESP32 mengirim HTTP trigger ke server Python
 * yang akan mengambil foto dari webcam laptop.
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
#define LED_INDICATOR_PIN 4
#define SWITCH_PIN        12

// WiFi credentials
const char* ssid = "Adhyasta";
const char* password = "juarasatu";

// Server URL - GANTI dengan IP laptop Anda!
// Jalankan: hostname -I di laptop
const char* captureURL = "http://10.137.208.149:5000/capture";  // GANTI IP INI!

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

// Function declarations
void initWiFi();
void initPins();
void initOLED();
void updateOLED(String line1, String line2 = "", String line3 = "", String line4 = "");
bool triggerCameraCapture();
void handleMotionDetection();
void handleSystemSwitch();
void displayStatus();

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("========================================");
  Serial.println("  Security System - ESP32 + Webcam");
  Serial.println("========================================");
  
  initPins();
  initOLED();
  
  updateOLED("System Starting", "Please wait...");
  delay(1000);
  
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

void initOLED() {
  Serial.println("[INIT] Menginisialisasi OLED display...");
  Wire.begin();
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("[WARNING] OLED tidak ditemukan! Lanjutkan tanpa OLED...");
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

void updateOLED(String line1, String line2, String line3, String line4) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.println(line1);
  
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

void initWiFi() {
  Serial.println("[INIT] Menghubungkan ke WiFi...");
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
    updateOLED("WiFi ERROR", "Check SSID/Pass", "Use 2.4GHz", "");
    delay(2000);
  }
}

bool triggerCameraCapture() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WARNING] WiFi tidak terhubung");
    return false;
  }
  
  Serial.println("[CAMERA] Mengirim trigger ke server...");
  HTTPClient http;
  http.begin(captureURL);
  http.addHeader("Content-Type", "application/json");
  
  // Kirim POST request kosong atau dengan data sederhana
  String jsonData = "{\"source\":\"esp32\",\"motion\":true}";
  int httpResponseCode = http.POST(jsonData);
  
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
      
      // Trigger webcam capture via HTTP
      bool success = triggerCameraCapture();
      
      if (success) {
        Serial.println("[SUCCESS] Capture triggered!");
        imageCount++;
        updateOLED("Photo Taken!", String("Count: ") + String(imageCount), "Check gallery", "");
        delay(1500);
      } else {
        Serial.println("[ERROR] Gagal trigger capture");
        updateOLED("SERVER ERROR", "Check connection", "", "");
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
  
  String line1 = systemArmed ? "ARMED" : "DISARMED";
  String line2 = WiFi.status() == WL_CONNECTED ? "WiFi: OK" : "WiFi: FAIL";
  String line3 = String("Photos: ") + String(imageCount);
  String line4 = "";
  
  updateOLED(line1, line2, line3, line4);
}
