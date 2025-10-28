# 📘 Setup Guide - Mini Security System

## ESP32 Controller + ESP32-CAM Architecture

---

## 🏗️ Arsitektur Sistem

```
┌─────────────────────────────────────────────────────────────┐
│                    ESP32 Dev Board (Controller)              │
│  - Sensor PIR (GPIO 13)                                      │
│  - LED Indikator (GPIO 33)                                   │
│  - Switch Armed/Disarmed (GPIO 12)                           │
│  - WiFi Connection                                           │
│  - HTTP Upload ke Server                                     │
│  - Serial Communication (TX2/RX2)                            │
└────────────────────┬────────────────────────────────────────┘
                     │ Serial Communication
                     │ TX2(GPIO17) → RX(GPIO3)
                     │ RX2(GPIO16) ← TX(GPIO1)
                     │ GND ↔ GND
┌────────────────────┴────────────────────────────────────────┐
│                    ESP32-CAM (Slave Module)                  │
│  - Camera Module                                             │
│  - SD Card (Optional backup)                                 │
│  - Receives commands via Serial                              │
│  - Captures & sends image data                               │
└─────────────────────────────────────────────────────────────┘
```

---

## 📋 Daftar Komponen

### Hardware:

1. **ESP32 Dev Board** (board utama/controller)
2. **ESP32-CAM AI Thinker** (modul kamera)
3. **Sensor PIR HC-SR501**
4. **LED** (+ resistor 220Ω)
5. **Switch/Button**
6. **MicroSD Card** (opsional, untuk backup di ESP32-CAM)
7. **Kabel jumper**
8. **Power supply** (5V untuk kedua board)

---

## 🔌 Diagram Koneksi

### ESP32 Dev Board (Controller):

```
ESP32 Pin      →   Komponen
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
GPIO 13        →   PIR Sensor (OUT)
GPIO 33        →   LED (+) [via resistor 220Ω]
GPIO 12        →   Switch (satu sisi)
GND            →   PIR (GND), LED (-), Switch (sisi lain)
5V/VIN         →   PIR (VCC)
GPIO 17 (TX2)  →   ESP32-CAM RX (GPIO3)
GPIO 16 (RX2)  →   ESP32-CAM TX (GPIO1)
GND            →   ESP32-CAM GND (penting!)
```

### ESP32-CAM:

```
ESP32-CAM Pin  →   Koneksi
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
GPIO 1 (TX)    →   ESP32 RX2 (GPIO16)
GPIO 3 (RX)    →   ESP32 TX2 (GPIO17)
GND            →   ESP32 GND (common ground!)
5V             →   Power supply 5V
SD Card        →   Masukkan microSD (opsional)
```

**⚠️ PENTING:** Pastikan GND kedua board terhubung untuk komunikasi Serial yang stabil!

---

## 🚀 Langkah-langkah Setup

### **FASE 1: Upload Program ke ESP32-CAM**

1. **Persiapkan file untuk ESP32-CAM:**

   ```bash
   # Rename file example menjadi main.cpp
   cd /home/anthony05/Coding/FinalProject_SIC/src
   mv main.cpp main_esp32_controller.cpp.backup
   mv esp32cam_slave.cpp.example main.cpp
   ```

2. **Edit `platformio.ini` untuk ESP32-CAM:**

   ```ini
   [env:esp32cam]
   platform = espressif32
   board = esp32cam
   framework = arduino
   monitor_speed = 115200
   ```

3. **Hubungkan ESP32-CAM ke komputer:**

   - Gunakan USB-to-Serial adapter (FTDI)
   - Koneksi:
     - FTDI TX → ESP32-CAM RX (GPIO3)
     - FTDI RX → ESP32-CAM TX (GPIO1)
     - FTDI GND → ESP32-CAM GND
     - FTDI 5V → ESP32-CAM 5V
   - **Hubungkan GPIO0 ke GND** (untuk masuk mode programming)

4. **Upload ke ESP32-CAM:**

   ```bash
   pio run --target upload
   ```

5. **Test ESP32-CAM:**

   - Lepas GPIO0 dari GND
   - Tekan tombol RESET di ESP32-CAM
   - Buka Serial Monitor (115200 baud)
   - Seharusnya muncul: "ESP32-CAM Ready!" dan "READY"

6. **Lepas ESP32-CAM dari komputer**

---

### **FASE 2: Upload Program ke ESP32 Controller**

1. **Restore file untuk ESP32 Controller:**

   ```bash
   cd /home/anthony05/Coding/FinalProject_SIC/src
   mv main.cpp main_esp32cam.cpp.backup
   mv main_esp32_controller.cpp.backup main.cpp
   ```

2. **Edit `platformio.ini` untuk ESP32 Dev:**

   ```ini
   [env:esp32dev]
   platform = espressif32
   board = esp32dev
   framework = arduino
   monitor_speed = 115200
   ```

3. **Edit kredensial WiFi di `main.cpp`:**

   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";       // Ganti!
   const char* password = "YOUR_WIFI_PASSWORD"; // Ganti!
   const char* serverURL = "http://YOUR_SERVER_IP:PORT/upload"; // Ganti!
   ```

4. **Hubungkan ESP32 Dev Board ke komputer via USB**

5. **Upload ke ESP32:**

   ```bash
   pio run --target upload
   ```

6. **Buka Serial Monitor:**
   ```bash
   pio device monitor
   ```

---

### **FASE 3: Integrasi Hardware**

1. **Hubungkan ESP32-CAM ke ESP32 Controller:**

   - ESP32 TX2 (GPIO17) → ESP32-CAM RX (GPIO3)
   - ESP32 RX2 (GPIO16) → ESP32-CAM TX (GPIO1)
   - ESP32 GND → ESP32-CAM GND (WAJIB!)

2. **Hubungkan sensor dan komponen:**

   - PIR Sensor ke ESP32 (GPIO 13, 5V, GND)
   - LED ke ESP32 (GPIO 33 via resistor, GND)
   - Switch ke ESP32 (GPIO 12, GND)

3. **Power kedua board:**

   - ESP32 Controller via USB atau power supply
   - ESP32-CAM via power supply 5V terpisah

4. **Test sistem:**
   - Buka Serial Monitor ESP32 Controller
   - Seharusnya terlihat: "ESP32-CAM berhasil terhubung!"
   - Gerakkan tangan di depan PIR
   - LED akan menyala, foto akan diambil

---

## 📡 Protokol Komunikasi Serial

### Perintah dari ESP32 → ESP32-CAM:

| Perintah    | Response      | Deskripsi   |
| ----------- | ------------- | ----------- |
| `TEST\n`    | `READY\n`     | Cek koneksi |
| `CAPTURE\n` | `OK\n` + data | Ambil foto  |

### Format Data Gambar:

```
OK\n
SIZE:12345\n
[binary JPEG data 12345 bytes]
```

---

## 🧪 Testing & Troubleshooting

### Test 1: Komunikasi Serial

```cpp
// Di Serial Monitor ESP32 Controller, ketik:
TEST
// Seharusnya ESP32-CAM merespons: READY
```

### Test 2: Capture Manual

```cpp
// Di Serial Monitor ESP32 Controller, ketik:
CAPTURE
// Seharusnya foto diambil dan data dikirim
```

### Troubleshooting:

#### ❌ "ESP32-CAM tidak merespons"

- Cek koneksi TX/RX (mungkin terbalik)
- Cek GND common ground
- Pastikan ESP32-CAM sudah diprogram
- Cek power supply ESP32-CAM (min 5V 2A)

#### ❌ "Timeout menerima data gambar"

- Ukuran gambar terlalu besar untuk Serial
- Edit `esp32cam_slave.cpp`: ubah `FRAMESIZE_SVGA` → `FRAMESIZE_VGA`
- Atau tingkatkan buffer di controller

#### ❌ "WiFi gagal terhubung"

- Cek SSID dan password
- Pastikan WiFi 2.4GHz (ESP32 tidak support 5GHz)
- Coba dekatkan ESP32 ke router

#### ❌ "PIR tidak mendeteksi"

- Tunggu 30-60 detik setelah power on (PIR warming up)
- Cek koneksi VCC, GND, OUT
- Adjust sensitivity potentiometer di PIR

---

## 📊 Monitoring Serial

### Status yang akan muncul:

```
========================================
  Mini Security System - ESP32 Controller
========================================

[INIT] Menginisialisasi pin GPIO...
[INIT] Pin GPIO berhasil diinisialisasi
[INIT] Menginisialisasi komunikasi dengan ESP32-CAM...
[INIT] ESP32-CAM berhasil terhubung!
[INIT] Menghubungkan ke WiFi...
[INIT] WiFi berhasil terhubung!
[INIT] IP Address: 192.168.1.100

========================================
  Sistem Siap!
========================================
[STATUS] System Armed - Monitoring aktif
[STATUS] WiFi Connected - IP: 192.168.1.100
[STATUS] Images captured: 0

[STATUS] Monitoring...

========================================
[ALERT] Motion Detected!
========================================
[ACTION] LED indikator dihidupkan
[CAMERA] Mengirim perintah ke ESP32-CAM untuk mengambil foto...
[CAMERA] ESP32-CAM siap mengirim data gambar
[CAMERA] Menerima data gambar dari ESP32-CAM...
[CAMERA] Ukuran gambar: 15420 bytes
[CAMERA] Berhasil menerima 15420 bytes data gambar
[SUCCESS] Image Captured
[HTTP] Mengirim foto ke server...
[HTTP] Response code: 200
[ACTION] LED indikator dimatikan
========================================
```

---

## 🔧 Konfigurasi Lanjutan

### Mengubah Resolusi Gambar:

Edit di `esp32cam_slave.cpp`:

```cpp
// Untuk gambar lebih kecil (lebih cepat via Serial):
config.frame_size = FRAMESIZE_VGA;    // 640x480

// Untuk gambar lebih besar (lebih lambat):
config.frame_size = FRAMESIZE_SVGA;   // 800x600
config.frame_size = FRAMESIZE_XGA;    // 1024x768
config.frame_size = FRAMESIZE_UXGA;   // 1600x1200
```

### Mengubah Kualitas JPEG:

```cpp
config.jpeg_quality = 10;  // 0-63, semakin kecil = kualitas lebih baik
```

### Mengubah Cooldown PIR:

Edit di `main.cpp`:

```cpp
const unsigned long pirDebounceDelay = 5000; // 5 detik
```

---

## 📦 File Structure

```
FinalProject_SIC/
├── platformio.ini                      # Konfigurasi PlatformIO
├── src/
│   ├── main.cpp                        # ESP32 Controller (yang aktif)
│   ├── esp32cam_slave.cpp.example      # ESP32-CAM Slave (backup)
│   └── (backup files...)
├── SETUP_GUIDE.md                      # Dokumentasi ini
└── README.md
```

---

## 🎯 Checklist Lengkap

- [ ] ESP32-CAM sudah diprogram dengan kode slave
- [ ] ESP32 Dev Board sudah diprogram dengan kode controller
- [ ] Koneksi Serial TX/RX sudah benar (TX→RX, RX→TX)
- [ ] GND kedua board terhubung
- [ ] PIR Sensor terhubung ke ESP32 (GPIO 13)
- [ ] LED terhubung ke ESP32 (GPIO 33)
- [ ] Switch terhubung ke ESP32 (GPIO 12)
- [ ] WiFi credentials sudah diisi
- [ ] Server URL sudah dikonfigurasi
- [ ] Power supply cukup untuk kedua board
- [ ] Serial Monitor menunjukkan "ESP32-CAM berhasil terhubung"
- [ ] Test motion detection berhasil

---

## 📞 Support

Jika ada masalah, cek:

1. Serial Monitor untuk error messages
2. Koneksi hardware (gunakan multimeter)
3. Power supply (min 5V 2A untuk ESP32-CAM)
4. WiFi signal strength

**Selamat mencoba! 🚀**
