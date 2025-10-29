# 🔐 Mini Security System - ESP32 + Webcam

> **Sistem keamanan pintar** menggunakan ESP32 untuk deteksi gerakan dan webcam laptop untuk capture foto otomatis.

[![PlatformIO](https://img.shields.io/badge/PlatformIO-ESP32-orange.svg)](https://platformio.org/)
[![Python](https://img.shields.io/badge/Python-3.x-blue.svg)](https://www.python.org/)
[![Flask](https://img.shields.io/badge/Flask-2.0+-green.svg)](https://flask.palletsprojects.com/)
[![OpenCV](https://img.shields.io/badge/OpenCV-4.8-red.svg)](https://opencv.org/)

---

## 📐 Arsitektur Sistem

```
╔════════════════════════════════════════════════════════════╗
║              ESP32 DevKit (Controller)                     ║
║  ✅ Sensor PIR - Deteksi gerakan                           ║
║  ✅ LED Indikator - Visual feedback                        ║
║  ✅ Switch - Armed/Disarmed mode                           ║
║  ✅ OLED Display - Real-time status                        ║
║  ✅ WiFi - HTTP communication                              ║
╚═════════════════════╦══════════════════════════════════════╝
                      ║
                      ║ HTTP POST (WiFi)
                      ║ Trigger: Motion Detected
                      ║
╔═════════════════════╩══════════════════════════════════════╗
║         Python Flask Server (Laptop)                       ║
║  ✅ OpenCV - Webcam capture                                ║
║  ✅ Flask API - HTTP endpoints                             ║
║  ✅ Web Gallery - View captured images                     ║
║  ✅ Auto-save - Timestamp filenames                        ║
╚════════════════════════════════════════════════════════════╝
```

### **Kenapa Webcam Laptop?**

| Sebelum (ESP32-CAM) | Sekarang (Webcam)      |
| ------------------- | ---------------------- |
| ❌ Upload rumit     | ✅ Tidak perlu upload  |
| ❌ Perlu jumper     | ✅ Plug & play         |
| ❌ Resolusi rendah  | ✅ Kualitas lebih baik |
| ❌ Serial2 ribet    | ✅ HTTP simpel         |
| ❌ Debugging susah  | ✅ Log terpisah        |


---

## ✨ Fitur Utama

### **ESP32 Controller:**

- 🔍 **Deteksi Gerakan** - Sensor PIR HC-SR501
- 💡 **LED Indikator** - Visual feedback saat motion detected
- 🔘 **Armed/Disarmed Switch** - Kontrol aktivasi sistem
- 📟 **OLED Display** - Status real-time (WiFi, IP, jumlah foto)
- 📡 **WiFi Connectivity** - HTTP trigger ke server

### **Python Camera Server:**

- 📸 **Webcam Capture** - OpenCV integration
- 🌐 **Web Gallery** - HTML interface untuk lihat foto
- 🔄 **REST API** - Endpoints untuk trigger & status
- 💾 **Auto-save** - Timestamp filenames (`motion_YYYYMMDD_HHMMSS.jpg`)
- 📊 **Status Monitor** - Real-time webcam & server info

---

## 🛠️ Komponen yang Dibutuhkan

### **Hardware:**

| Komponen             | Qty | Keterangan           |
| -------------------- | --- | -------------------- |
| ESP32 DevKit         | 1   | Main controller      |
| Sensor PIR HC-SR501  | 1   | Deteksi gerakan      |
| LED 5mm              | 1   | Indikator            |
| Resistor 220Ω        | 1   | Untuk LED (optional) |
| Push Button / Switch | 1   | Armed/Disarmed       |
| OLED Display 128x64  | 1   | I2C display (SSD1306)|
| Kabel Jumper         | ~10 | Koneksi              |
| USB Cable            | 1   | Programming & power  |
| **Laptop dengan Webcam** | 1 | Untuk capture foto |

### **Software:**

#### **ESP32 Development:**

- PlatformIO IDE (VS Code extension)
- Arduino Framework for ESP32
- Libraries: WiFi, HTTPClient, Adafruit_GFX, Adafruit_SSD1306

#### **Python Server:**

- Python 3.x
- OpenCV (`opencv-python`)
- Flask

---

## 🔌 Wiring Diagram

### **ESP32 DevKit Connections:**

```
┌─────────────┬──────────────────────────────────┐
│  ESP32 Pin  │  Connect To                      │
├─────────────┼──────────────────────────────────┤
│  GPIO 13    │  PIR Sensor OUT                  │
│  GPIO 33    │  LED (+) [via 220Ω resistor]     │
│  GPIO 12    │  Switch (one pin)                │
│  GPIO 21    │  OLED SDA                        │
│  GPIO 22    │  OLED SCL                        │
│  3.3V       │  PIR VCC, OLED VCC               │
│  GND        │  PIR GND, LED (-), Switch, OLED  │
└─────────────┴──────────────────────────────────┘
```

**📸 Webcam:** Tidak perlu wiring - langsung pakai webcam laptop!

**💡 Note:** LED bisa langsung tanpa resistor jika menggunakan LED built-in ESP32 (GPIO 2).

---

## 🚀 Quick Start (3 Langkah!)

### **Step 1: Setup Python Server**

```bash
cd /path/to/FinalProject_SIC

# Install dependencies
pip3 install opencv-python flask

# Atau pakai requirements.txt
pip3 install -r requirements.txt

# Start server
./start_camera_server.sh
```

**Output server akan menampilkan:**

```
================================================
  Security System - Camera Server dengan Webcam
================================================

[SETUP] Memeriksa dependencies...
[INFO] Dependencies OK!

======================================
  Server akan berjalan di:
  http://192.168.1.100:5000
======================================

📌 PENTING: Catat IP address ini!
```

### **Step 2: Konfigurasi ESP32**

Edit file `src/main.cpp`:

```cpp
// WiFi Settings
const char* ssid = "YOUR_WIFI_SSID";         // Ganti!
const char* password = "YOUR_WIFI_PASSWORD"; // Ganti!

// Server URL - GANTI dengan IP dari Step 1!
const char* captureURL = "http://192.168.1.100:5000/capture";
```

### **Step 3: Upload ke ESP32**

```bash
# Compile & upload
platformio run --target upload

# Monitor serial (optional)
platformio device monitor
```

**✅ Sistem siap!** Gerakkan tangan di depan PIR sensor untuk test.

---

## ⚙️ Konfigurasi Lanjutan

### **WiFi Settings**

Edit `src/main.cpp` baris 27-28:

```cpp
const char* ssid = "NamaWiFi";          // 2.4GHz only!
const char* password = "PasswordWiFi";  // Case-sensitive
```

### **Server URL**

Edit `src/main.cpp` baris 35:

```cpp
const char* captureURL = "http://IP_LAPTOP:5000/capture";
```

Cara cek IP laptop:

```bash
# Linux/Mac
hostname -I

# Windows
ipconfig
```

### **PIR Cooldown**

Edit `src/main.cpp` baris 44:

```cpp
const unsigned long pirDebounceDelay = 5000;  // 5 detik (default)
// Ubah jadi 3000 untuk 3 detik
```

### **Resolusi Webcam**

Edit `camera_server.py` baris 26-27:

```python
camera.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)   # Default: 640
camera.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)   # Default: 480
```

### **Port Server**

Edit `camera_server.py` baris terakhir:

```python
app.run(host='0.0.0.0', port=5000, debug=False)  # Ganti port jika perlu
```

Dan update di `src/main.cpp`:

```cpp
const char* captureURL = "http://IP:PORT/capture";  // Sesuaikan port
```

---

## 📊 Serial Monitor Output

```

---

## 📊 Serial Monitor Output

### **ESP32 Controller:**

```
========================================
  Security System - ESP32 + Webcam
========================================
[INIT] Menginisialisasi pin GPIO...
[INIT] Pin GPIO berhasil diinisialisasi
[INIT] Menginisialisasi OLED display...
[INIT] OLED berhasil diinisialisasi
[INIT] Menghubungkan ke WiFi...
..........
[INIT] WiFi berhasil terhubung!
[INIT] IP Address: 192.168.1.105

========================================
  Sistem Siap!
========================================
[STATUS] System Armed - Monitoring aktif
[STATUS] WiFi Connected - IP: 192.168.1.105
[STATUS] Images captured: 0

[STATUS] Monitoring...

========================================
[ALERT] Motion Detected!
========================================
[ACTION] LED dihidupkan
[CAMERA] Mengirim trigger ke server...
[HTTP] Response code: 200
[HTTP] Response: {"success":true,"filename":"motion_20251029_143052.jpg"}
[SUCCESS] Photo Taken! Count: 1
[ACTION] LED dimatikan
========================================
```

### **Python Server:**

```
[INIT] Webcam berhasil diinisialisasi!
[SERVER] Starting Flask server...
 * Running on http://0.0.0.0:5000

[CAPTURE] Menerima request dari ESP32
[CAPTURE] Mengambil foto dari webcam...
[CAPTURE] Foto disimpan: captures/motion_20251029_143052.jpg
[CAPTURE] Mengirim response ke ESP32
192.168.1.105 - - [29/Oct/2025 14:30:52] "POST /capture HTTP/1.1" 200 -
```

---

## 🌐 Web Gallery

Akses web gallery untuk melihat foto-foto hasil capture:

### **Dari Laptop:**

```
http://localhost:5000/
```

### **Dari HP/Device Lain (Same WiFi):**

```
http://IP_LAPTOP:5000/
```

### **Fitur Gallery:**

- 📊 Total foto tertangkap
- 🔍 Grid view dengan thumbnail
- 🕒 Timestamp pada setiap foto
- 🔄 Tombol refresh
- 📸 Tombol test capture manual
- ✅ Status webcam real-time

---

## 🐛 Troubleshooting

### **1. Webcam Error: "Tidak dapat membuka webcam"**

**Penyebab:**

- Webcam sedang digunakan aplikasi lain (Zoom, Skype, dll)
- Permission webcam tidak diizinkan
- Driver webcam bermasalah

**Solusi:**

```bash
# Tutup semua aplikasi yang pakai webcam
# Restart server
./start_camera_server.sh

# Test webcam
python3 -c "import cv2; print('OK' if cv2.VideoCapture(0).isOpened() else 'ERROR')"
```

### **2. ESP32 Error: "HTTP POST gagal"**

**Penyebab:**

- IP address server salah
- Server Python tidak berjalan
- ESP32 dan laptop tidak satu WiFi

**Solusi:**

```bash
# 1. Cek IP laptop
hostname -I

# 2. Pastikan server berjalan
./start_camera_server.sh

# 3. Test dari browser
curl http://localhost:5000/status

# 4. Update IP di src/main.cpp
# 5. Re-upload ke ESP32
```

### **3. OLED Tidak Muncul**

**Penyebab:**

- OLED tidak terhubung
- Alamat I2C salah (0x3C vs 0x3D)

**Solusi:**

```cpp
// Sistem akan tetap jalan tanpa OLED
// Cek wiring: SDA→GPIO21, SCL→GPIO22

// Jika alamat berbeda, edit src/main.cpp:
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
// Tambahkan:
display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // Ubah 0x3C→0x3D jika perlu
```

### **4. PIR Tidak Mendeteksi**

**Penyebab:**

- PIR warming up (tunggu 30-60 detik)
- Koneksi VCC/GND/OUT salah
- Sensitivity terlalu rendah

**Solusi:**

- Tunggu 1 menit setelah power on
- Cek wiring: VCC→3.3V, GND→GND, OUT→GPIO13
- Putar potentiometer sensitivity di PIR sensor
- Test dengan gerakan tangan 1-3 meter dari sensor

### **5. WiFi Gagal Terhubung**

**Penyebab:**

- SSID/password salah (case-sensitive!)
- WiFi 5GHz (ESP32 hanya support 2.4GHz)
- Signal lemah

**Solusi:**

```cpp
// Edit src/main.cpp - cek typo
const char* ssid = "NamaWiFi";        // Exact match!
const char* password = "Password123"; // Exact match!

// Pastikan WiFi 2.4GHz, bukan 5GHz
// Dekatkan ESP32 ke router
```

### **6. Foto Tidak Muncul di Gallery**

**Penyebab:**

- Folder `captures/` tidak ada
- Permission folder salah

**Solusi:**

```bash
# Buat folder manual
mkdir -p captures

# Cek isi folder
ls -lh captures/

# Test capture manual via browser
# Buka: http://localhost:5000/
# Klik tombol "📸 Test Capture"
```

---

## 📡 REST API Documentation

### **POST /capture**

Trigger capture foto dari webcam.

**Request:**

```http
POST /capture HTTP/1.1
Host: localhost:5000
Content-Type: application/json

{
  "source": "esp32",
  "motion": true
}
```

**Response (Success):**

```json
{
  "success": true,
  "message": "Gambar berhasil di-capture",
  "filename": "motion_20251029_143052.jpg",
  "timestamp": "2025-10-29T14:30:52.123456"
}
```

**Response (Error):**

```json
{
  "success": false,
  "error": "Tidak dapat membuka webcam"
}
```

### **GET /status**

Cek status server dan webcam.

**Response:**

```json
{
  "webcam": "OK",
  "total_images": 15,
  "server": "running",
  "captures_folder": "captures/"
}
```

### **GET /api/images**

List semua gambar yang tersimpan.

**Response:**

```json
{
  "total": 15,
  "images": [
    "motion_20251029_143052.jpg",
    "motion_20251029_142830.jpg",
    "motion_20251029_142615.jpg"
  ]
}
```

### **GET /**

Web gallery interface (HTML).

---

## 📁 File Structure

```
FinalProject_SIC/
│
├── platformio.ini               # PlatformIO config (esp32dev)
├── requirements.txt             # Python dependencies
├── README.md                    # 📖 Dokumentasi utama (file ini)
├── README_WEBCAM.md             # 📚 Panduan detail webcam
├── .gitignore                   # Git ignore rules
│
├── src/
│   ├── main.cpp                 # 🔥 ESP32 Controller (ACTIVE)
│   └── esp32cam_slave.cpp.example  # Template ESP32-CAM (deprecated)
│
├── camera_server.py             # 🐍 Python Flask server
├── start_camera_server.sh       # 🚀 Script startup server
│
├── captures/                    # 📸 Folder foto hasil capture (auto-generated)
│   ├── motion_20251029_143052.jpg
│   ├── motion_20251029_142830.jpg
│   └── ...
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

### **Setup:**

- [ ] Python 3.x terinstall
- [ ] OpenCV dan Flask terinstall (`pip install -r requirements.txt`)
- [ ] PlatformIO extension terinstall di VS Code
- [ ] ESP32 drivers terinstall (CH340/CP2102)

### **Hardware:**

- [ ] PIR Sensor terhubung ke GPIO 13
- [ ] LED terhubung ke GPIO 33
- [ ] Switch terhubung ke GPIO 12
- [ ] OLED terhubung ke GPIO 21 (SDA) & GPIO 22 (SCL)
- [ ] ESP32 terhubung via USB

### **Software:**

- [ ] WiFi credentials sudah dikonfigurasi di `src/main.cpp`
- [ ] Server IP sudah dikonfigurasi di `src/main.cpp`
- [ ] Python server sudah running (`./start_camera_server.sh`)
- [ ] ESP32 sudah di-upload dengan kode terbaru

### **Testing:**

- [ ] Serial Monitor menunjukkan "WiFi berhasil terhubung"
- [ ] OLED menampilkan IP address ESP32
- [ ] Python server menunjukkan "[INIT] Webcam berhasil diinisialisasi"
- [ ] Web gallery bisa diakses di browser
- [ ] Motion detection berhasil trigger foto

---

## 🔧 Development Commands

### **PlatformIO (ESP32):**

```bash
# Build project
platformio run

# Upload ke ESP32
platformio run --target upload

# Upload + Monitor
platformio run --target upload && platformio device monitor

# Monitor serial saja
platformio device monitor

# Clean build
platformio run --target clean
```

### **Python Server:**

```bash
# Start server
./start_camera_server.sh

# Manual start
python3 camera_server.py

# Install dependencies
pip3 install -r requirements.txt

# Test webcam
python3 -c "import cv2; cam = cv2.VideoCapture(0); print('OK' if cam.isOpened() else 'ERROR')"
```

### **Git Commands:**

```bash
# Status
git status

# Add all changes
git add .

# Commit
git commit -m "Update code"

# Push to GitHub
git push origin main
```

---

## 📚 Dokumentasi Lengkap

- **[README_WEBCAM.md](./README_WEBCAM.md)** - 📖 Panduan detail sistem webcam, troubleshooting, dan API
- **[PlatformIO Docs](https://docs.platformio.org/)** - Dokumentasi PlatformIO
- **[ESP32 Pinout](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)** - Referensi GPIO ESP32
- **[Flask Documentation](https://flask.palletsprojects.com/)** - Flask framework docs
- **[OpenCV Python](https://docs.opencv.org/4.x/d6/d00/tutorial_py_root.html)** - OpenCV tutorial

---

## 💡 Tips & Best Practices

### **Performance:**

- Gunakan WiFi 2.4GHz yang stabil (jangan 5GHz!)
- Pastikan ESP32 dan laptop dekat dengan router
- Tutup aplikasi webcam lain saat server berjalan
- Gunakan resolusi webcam 640x480 untuk response cepat

### **Security:**

- Ganti default WiFi password dengan yang kuat
- Jangan expose server ke public internet (local only)
- Backup folder `captures/` secara berkala

### **Maintenance:**

- Bersihkan folder `captures/` jika penuh
- Restart server jika webcam hang
- Update library PlatformIO secara berkala: `pio lib update`

---

## 🤝 Contributing

Contributions welcome! Silakan fork repository ini dan submit pull request.

### **Development Roadmap:**

- [ ] Mobile app untuk monitoring
- [ ] Notifikasi Telegram/WhatsApp saat motion detected
- [ ] Face recognition dengan OpenCV
- [ ] Multiple camera support
- [ ] Cloud storage integration

---

## 📄 License

This project is open source and available under the [MIT License](LICENSE).

---

## 👨‍💻 Author

**Mini Security System**  
Final Project - Sistem Instrumentasi dan Kontrol  
October 2025

---

## 🙏 Acknowledgments

- ESP32 & Arduino Community
- PlatformIO Team
- Flask & OpenCV Developers
- Stack Overflow Contributors

---

## 📞 Support

Jika ada pertanyaan atau masalah:

1. Cek [README_WEBCAM.md](./README_WEBCAM.md) untuk troubleshooting detail
2. Lihat [Issues](../../issues) di GitHub
3. Buat issue baru jika belum ada solusi

---

**Happy Coding! 🚀**

---

<div align="center">

Made with ❤️ using ESP32, Python, and Coffee ☕

**[⬆ Back to Top](#-mini-security-system---esp32--webcam)**

</div>
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
