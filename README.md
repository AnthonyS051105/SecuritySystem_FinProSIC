# 🔐 Mini Security System dengan ESP32 & ESP32-CAM

Sistem keamanan sederhana menggunakan ESP32 sebagai controller utama dan ESP32-CAM sebagai modul kamera terpisah, dilengkapi sensor PIR untuk deteksi gerakan.

---

## 📐 Arsitektur Sistem

```
╔════════════════════════════════════════════════════════════╗
║           ESP32 Dev Board (Main Controller)                ║
║  • Sensor PIR untuk deteksi gerakan                        ║
║  • LED indikator status                                    ║
║  • Switch untuk armed/disarmed                             ║
║  • WiFi untuk upload ke server                             ║
║  • Serial communication dengan ESP32-CAM                   ║
╚════════════════════╦═══════════════════════════════════════╝
                     ║ Serial UART
                     ║ TX2 (GPIO17) ──→ RX (GPIO3)
                     ║ RX2 (GPIO16) ←── TX (GPIO1)
                     ║ GND ←──────────→ GND
╔════════════════════╩═══════════════════════════════════════╗
║           ESP32-CAM (Camera Module)                        ║
║  • Camera untuk capture image                              ║
║  • SD Card untuk backup (optional)                         ║
║  • Menerima perintah via Serial                            ║
╚════════════════════════════════════════════════════════════╝
```

---

## ✨ Fitur Utama

- ✅ **Deteksi Gerakan:** Sensor PIR mendeteksi pergerakan
- ✅ **Capture Foto:** ESP32-CAM mengambil foto berkualitas tinggi
- ✅ **LED Indikator:** Visual feedback saat mendeteksi gerakan
- ✅ **Armed/Disarmed Mode:** Switch untuk mengaktifkan/menonaktifkan sistem
- ✅ **Upload ke Server:** Kirim foto via HTTP POST
- ✅ **SD Card Backup:** Simpan foto di ESP32-CAM (optional)
- ✅ **Serial Monitoring:** Status real-time di Serial Monitor
- ✅ **Modular Design:** Controller dan kamera terpisah untuk fleksibilitas

---

## 🛠️ Komponen yang Dibutuhkan

### Hardware:

| Komponen             | Qty        | Keterangan             |
| -------------------- | ---------- | ---------------------- |
| ESP32 Dev Board      | 1          | Board utama/controller |
| ESP32-CAM AI Thinker | 1          | Modul kamera           |
| Sensor PIR HC-SR501  | 1          | Deteksi gerakan        |
| LED 5mm              | 1          | Indikator              |
| Resistor 220Ω        | 1          | Untuk LED              |
| Push Button / Switch | 1          | Armed/Disarmed         |
| MicroSD Card         | 1          | Optional, untuk backup |
| Kabel Jumper         | secukupnya | Koneksi                |
| USB Cable            | 2          | Programming            |
| Power Supply 5V 2A   | 1-2        | Power board            |

### Software:

- PlatformIO IDE (VS Code extension)
- Arduino Framework
- Library: esp_camera, WiFi, HTTPClient, SD_MMC

---

## 🔌 Wiring Diagram

### ESP32 Dev Board Connections:

```
┌─────────────┬─────────────────────────────┐
│  ESP32 Pin  │  Connect To                 │
├─────────────┼─────────────────────────────┤
│  GPIO 13    │  PIR Sensor OUT             │
│  GPIO 33    │  LED (+) via 220Ω resistor  │
│  GPIO 12    │  Switch (one side)          │
│  GPIO 17    │  ESP32-CAM RX (GPIO3)       │
│  GPIO 16    │  ESP32-CAM TX (GPIO1)       │
│  GND        │  PIR GND, LED (-), Switch,  │
│             │  ESP32-CAM GND              │
│  5V/VIN     │  PIR VCC                    │
└─────────────┴─────────────────────────────┘
```

### ESP32-CAM Connections:

```
┌──────────────┬────────────────────────┐
│  ESP32-CAM   │  Connect To            │
├──────────────┼────────────────────────┤
│  GPIO 1 (TX) │  ESP32 GPIO 16 (RX2)   │
│  GPIO 3 (RX) │  ESP32 GPIO 17 (TX2)   │
│  GND         │  ESP32 GND (IMPORTANT!)│
│  5V          │  Power Supply 5V       │
└──────────────┴────────────────────────┘
```

**⚠️ CRITICAL:** Pastikan GND kedua board terhubung untuk komunikasi Serial yang stabil!

---

## 🚀 Quick Start Guide

### Option 1: Menggunakan Helper Script (Recommended)

```bash
# Masuk ke direktori project
cd /home/anthony05/Coding/FinalProject_SIC

# Jalankan helper script
./setup.sh

# Pilih mode:
# 1 = Setup untuk programming ESP32-CAM
# 2 = Setup untuk programming ESP32 Controller
# 3 = Lihat status project
```

### Option 2: Manual Setup

#### Step 1: Upload ke ESP32-CAM

```bash
# Setup mode ESP32-CAM
./setup.sh esp32cam

# Atau manual:
cp src/esp32cam_slave.cpp.example src/main.cpp
# Edit platformio.ini -> board = esp32cam

# Hubungkan ESP32-CAM via FTDI (GPIO0 ke GND)
pio run --target upload

# Lepas GPIO0, tekan RESET di ESP32-CAM
```

#### Step 2: Upload ke ESP32 Controller

```bash
# Setup mode ESP32 Controller
./setup.sh esp32dev

# Edit WiFi credentials di src/main.cpp
# Edit server URL

# Hubungkan ESP32 via USB
pio run --target upload
```

#### Step 3: Wiring & Test

```bash
# Hubungkan hardware sesuai diagram
# Power on kedua board
# Monitor serial
pio device monitor
```

---

## ⚙️ Konfigurasi

### WiFi Settings (di ESP32 Controller):

Edit file `src/main.cpp`:

```cpp
const char* ssid = "YOUR_WIFI_SSID";           // Ganti!
const char* password = "YOUR_WIFI_PASSWORD";   // Ganti!
const char* serverURL = "http://192.168.1.100:8080/upload"; // Ganti!
```

### Camera Settings (di ESP32-CAM):

Edit file `src/esp32cam_slave.cpp.example`:

```cpp
// Ubah resolusi
config.frame_size = FRAMESIZE_SVGA;   // 800x600 (recommended untuk Serial)
// atau
config.frame_size = FRAMESIZE_VGA;    // 640x480 (lebih cepat)

// Ubah kualitas JPEG (0-63, semakin kecil = kualitas lebih baik)
config.jpeg_quality = 12;
```

### PIR Cooldown (di ESP32 Controller):

```cpp
const unsigned long pirDebounceDelay = 5000; // 5 detik antara deteksi
```

---

## 📊 Serial Monitor Output

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
[HTTP] Response: {"status":"success"}
[ACTION] LED indikator dimatikan
========================================
```

---

## 🐛 Troubleshooting

### Problem: ESP32-CAM tidak merespons

**Solution:**

- Cek koneksi TX/RX (mungkin terbalik)
- Pastikan GND terhubung antara kedua board
- Cek power supply ESP32-CAM (min 5V 2A)
- Re-upload program ke ESP32-CAM

### Problem: Timeout menerima data gambar

**Solution:**

- Ukuran gambar terlalu besar untuk Serial
- Edit ESP32-CAM: ubah `FRAMESIZE_SVGA` → `FRAMESIZE_VGA`
- Atau edit buffer size di controller

### Problem: WiFi gagal terhubung

**Solution:**

- Cek SSID dan password (case-sensitive)
- Pastikan WiFi 2.4GHz (ESP32 tidak support 5GHz)
- Dekatkan ESP32 ke router
- Restart router

### Problem: PIR tidak mendeteksi

**Solution:**

- Tunggu 30-60 detik setelah power on (warming up)
- Cek koneksi VCC, GND, OUT
- Adjust sensitivity potentiometer di PIR sensor
- Test dengan gerakan tangan di depan sensor

### Problem: LED tidak menyala

**Solution:**

- Cek polaritas LED (kaki panjang = anode/+)
- Pastikan resistor 220Ω terpasang
- Test LED dengan multimeter
- Cek koneksi ke GPIO 33

---

## 📡 Protokol Komunikasi

### Command Protocol:

| Command     | Response            | Description                  |
| ----------- | ------------------- | ---------------------------- |
| `TEST\n`    | `READY\n`           | Test koneksi ESP32-CAM       |
| `CAPTURE\n` | `OK\n` + image data | Request capture & send image |

### Image Data Format:

```
OK\n
SIZE:12345\n
[binary JPEG data 12345 bytes]
```

---

## 📁 File Structure

```
FinalProject_SIC/
│
├── platformio.ini                      # PlatformIO configuration
├── setup.sh                            # Helper script untuk switching mode
├── SETUP_GUIDE.md                      # Detailed setup guide
├── README.md                           # This file
│
├── src/
│   ├── main.cpp                        # Active code (ESP32 atau ESP32-CAM)
│   ├── esp32cam_slave.cpp.example      # ESP32-CAM slave code
│   ├── main_esp32_controller.cpp.backup # Backup ESP32 controller
│   └── main_esp32cam.cpp.backup        # Backup ESP32-CAM
│
├── include/
│   └── README
│
├── lib/
│   └── README
│
└── test/
    └── README
```

---

## 🎯 Usage Checklist

Sebelum menjalankan sistem, pastikan:

- [ ] ESP32-CAM sudah diprogram dengan kode slave
- [ ] ESP32 Controller sudah diprogram dengan kode controller
- [ ] WiFi credentials sudah dikonfigurasi
- [ ] Server URL sudah dikonfigurasi
- [ ] Koneksi Serial TX/RX sudah benar (TX→RX, RX→TX)
- [ ] GND kedua board terhubung
- [ ] PIR Sensor terhubung ke ESP32 (GPIO 13)
- [ ] LED terhubung ke ESP32 (GPIO 33)
- [ ] Switch terhubung ke ESP32 (GPIO 12)
- [ ] Power supply mencukupi (min 5V 2A untuk ESP32-CAM)
- [ ] Serial Monitor menunjukkan "ESP32-CAM berhasil terhubung"
- [ ] Test motion detection berhasil

---

## 🔧 Development Commands

```bash
# Build project
pio run

# Upload ke board
pio run --target upload

# Upload dan langsung monitor
pio run --target upload --target monitor

# Monitor serial saja
pio device monitor

# Clean build
pio run --target clean

# Check status dengan helper script
./setup.sh status
```

---

## 📚 Documentation

- [SETUP_GUIDE.md](./SETUP_GUIDE.md) - Detailed setup instructions
- [PlatformIO Docs](https://docs.platformio.org/)
- [ESP32-CAM Pinout](https://randomnerdtutorials.com/esp32-cam-ai-thinker-pinout/)
- [ESP32 Pinout](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)

---

## 🤝 Contributing

Feel free to fork this project and submit pull requests for improvements!

---

## 📄 License

This project is open source and available under the MIT License.

---

## 👨‍💻 Author

**Security System Project**  
Final Project SIC  
October 2025

---

## 🙏 Acknowledgments

- ESP32 Community
- Arduino Framework
- PlatformIO

---

**Happy Coding! 🚀**
