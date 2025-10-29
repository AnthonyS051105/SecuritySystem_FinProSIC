# Security System dengan Webcam Laptop

## 🎉 Perubahan Arsitektur

**Sebelum:** ESP32 + ESP32-CAM (sulit upload)  
**Sekarang:** ESP32 + Webcam Laptop (mudah!)

### **Keuntungan:**

✅ **Tidak perlu ESP32-CAM** - Tidak perlu repot jumper GPIO0→GND  
✅ **Webcam laptop** - Langsung pakai kamera yang sudah ada  
✅ **Lebih stabil** - Python + OpenCV lebih mature  
✅ **Resolusi lebih bagus** - Webcam laptop biasanya lebih baik  
✅ **Gallery web** - Lihat semua foto dalam satu halaman

---

## 📋 Komponen yang Dibutuhkan

### **Hardware:**

1. ESP32 DevKit (main controller)
2. Sensor PIR HC-SR501
3. LED
4. Switch / Push Button
5. OLED Display 128x64 (I2C)
6. Kabel jumper
7. Laptop dengan webcam

### **Software:**

1. Python 3.x
2. OpenCV (`opencv-python`)
3. Flask
4. PlatformIO

---

## 🔌 Wiring Diagram

```
ESP32 DevKit          Komponen
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
GPIO 13         →     PIR Sensor (OUT)
GPIO 33         →     LED (+)
GPIO 12         →     Switch (satu pin)
GPIO 21 (SDA)   →     OLED SDA
GPIO 22 (SCL)   →     OLED SCL
3.3V            →     PIR VCC, OLED VCC
5V              →     (opsional untuk PIR jika 3.3V kurang)
GND             →     PIR GND, LED (-), Switch (pin lain), OLED GND
```

---

## 🚀 Cara Penggunaan

### **Langkah 1: Install Dependencies Python**

```bash
cd /home/anthony05/Coding/FinalProject_SIC

# Install OpenCV dan Flask
pip3 install opencv-python flask
```

### **Langkah 2: Jalankan Camera Server di Laptop**

```bash
# Jalankan server
./start_camera_server.sh
```

**Output:**

```
================================================
  Security System - Camera Server dengan Webcam
================================================

[SETUP] Memeriksa dependencies...
[INFO] Dependencies OK!

======================================
  Server akan berjalan di:
  http://192.168.1.XXX:5000
======================================

📌 PENTING: Ganti IP di ESP32 Controller!
   Edit src/main.cpp baris 35:
   const char* captureURL = "http://192.168.1.XXX:5000/capture";

✅ Fitur:
   • Gallery: http://192.168.1.XXX:5000/
   • Capture endpoint: POST http://192.168.1.XXX:5000/capture
   • Status: GET http://192.168.1.XXX:5000/status

Tekan Ctrl+C untuk stop server

[INIT] Webcam berhasil diinisialisasi!
[SERVER] Starting Flask server...
 * Running on http://0.0.0.0:5000
```

### **Langkah 3: Update IP Address di ESP32 Controller**

Edit file `src/main.cpp` baris 35:

```cpp
// GANTI dengan IP yang muncul di server!
const char* captureURL = "http://192.168.1.XXX:5000/capture";  // GANTI IP INI!
```

Contoh:

```cpp
const char* captureURL = "http://192.168.1.100:5000/capture";
```

### **Langkah 4: Upload Kode ke ESP32**

```bash
# Compile dan upload
platformio run --target upload

# Monitor serial
platformio device monitor
```

**Output ESP32 yang benar:**

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
  Sistem Siap!
[STATUS] System Armed - Monitoring aktif
[STATUS] WiFi Connected - IP: 192.168.1.105
[STATUS] Images captured: 0
```

### **Langkah 5: Test Sistem**

#### **A. Test Webcam dari Browser**

1. Buka browser: `http://192.168.1.XXX:5000/`
2. Klik tombol **"📸 Test Capture"**
3. Webcam akan mengambil foto dan tampil di gallery

#### **B. Test Motion Detection**

1. Gerakkan tangan di depan sensor PIR
2. LED akan menyala
3. OLED menampilkan "MOTION!" dan "Capturing..."
4. ESP32 mengirim trigger ke server Python
5. Server mengambil foto dari webcam
6. Foto disimpan di folder `captures/`
7. Foto muncul di gallery web

---

## 📱 Cara Mengakses Gallery

### **Dari Browser di Laptop:**

```
http://localhost:5000/
```

### **Dari HP/Device Lain di WiFi yang Sama:**

```
http://192.168.1.XXX:5000/
```

_(Ganti XXX dengan IP laptop Anda)_

**Gallery menampilkan:**

- Total foto tertangkap
- Status webcam
- Grid foto dengan timestamp
- Tombol test capture
- Tombol refresh

---

## 🔍 Troubleshooting

### **1. Webcam Error: "Tidak dapat membuka webcam"**

**Penyebab:**

- Webcam sedang digunakan aplikasi lain
- Permission webcam tidak diizinkan
- Driver webcam bermasalah

**Solusi:**

```bash
# Tutup aplikasi yang menggunakan webcam (Zoom, Skype, dll)
# Restart server:
./start_camera_server.sh

# Cek webcam devices:
ls /dev/video*

# Test webcam dengan Python:
python3 -c "import cv2; print('Webcam OK' if cv2.VideoCapture(0).isOpened() else 'Webcam ERROR')"
```

### **2. ESP32 Error: "HTTP POST gagal"**

**Penyebab:**

- IP address server salah
- Server Python tidak berjalan
- ESP32 dan laptop tidak satu WiFi

**Solusi:**

```bash
# Cek IP laptop:
hostname -I

# Pastikan server berjalan:
./start_camera_server.sh

# Ping dari ESP32 ke laptop (cek di router/network)
# Edit src/main.cpp dengan IP yang benar
```

### **3. OLED Tidak Muncul**

**Penyebab:**

- OLED tidak terhubung
- Alamat I2C salah

**Solusi:**

```cpp
// Sistem akan tetap jalan tanpa OLED
// Cek wiring: SDA→GPIO21, SCL→GPIO22
// Coba scan I2C address dengan i2c-tools
```

### **4. Foto Tidak Muncul di Gallery**

**Penyebab:**

- Folder `captures/` tidak ada
- Permission folder salah

**Solusi:**

```bash
# Buat folder manual:
mkdir -p captures

# Cek isi folder:
ls -l captures/

# Test capture manual:
curl -X POST http://localhost:5000/capture -H "Content-Type: application/json"
```

---

## 📊 API Endpoints

### **POST /capture**

Trigger capture foto dari webcam

**Request:**

```json
{
  "source": "esp32",
  "motion": true
}
```

**Response:**

```json
{
  "success": true,
  "message": "Gambar berhasil di-capture",
  "filename": "motion_20251028_143052.jpg",
  "timestamp": "2025-10-28T14:30:52.123456"
}
```

### **GET /status**

Cek status server dan webcam

**Response:**

```json
{
  "webcam": "OK",
  "total_images": 5,
  "server": "running"
}
```

### **GET /api/images**

List semua gambar

**Response:**

```json
{
  "total": 5,
  "images": [
    "motion_20251028_143052.jpg",
    "motion_20251028_142830.jpg",
    ...
  ]
}
```

---

## 🎯 Fitur Lengkap

### **ESP32 Controller:**

- ✅ Sensor PIR deteksi gerakan
- ✅ LED indikator
- ✅ Switch armed/disarmed
- ✅ OLED display real-time status
- ✅ WiFi connectivity
- ✅ HTTP trigger ke server

### **Python Camera Server:**

- ✅ Webcam capture via OpenCV
- ✅ Flask web server
- ✅ Gallery web interface
- ✅ REST API endpoints
- ✅ Auto timestamp filename
- ✅ Status monitoring

---

## 📸 Format Nama File

```
motion_YYYYMMDD_HHMMSS.jpg
```

Contoh:

```
motion_20251028_143052.jpg
       ↓
       2025-10-28 14:30:52
```

---

## 🔧 Konfigurasi Lanjutan

### **Ubah Resolusi Webcam:**

Edit `camera_server.py` baris 26-27:

```python
camera.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)  # Lebar
camera.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)  # Tinggi
```

### **Ubah Port Server:**

Edit `camera_server.py` baris terakhir:

```python
app.run(host='0.0.0.0', port=8080, debug=False)  # Ganti port
```

Dan edit `src/main.cpp` baris 35:

```cpp
const char* captureURL = "http://192.168.1.XXX:8080/capture";  // Ganti port
```

### **Ubah Debounce PIR:**

Edit `src/main.cpp` baris 44:

```cpp
const unsigned long pirDebounceDelay = 3000;  // 3 detik (default 5000)
```

---

## 💡 Tips

1. **Pastikan webcam tidak digunakan aplikasi lain** sebelum start server
2. **Gunakan WiFi 2.4GHz**, ESP32 tidak support 5GHz
3. **Laptop dan ESP32 harus satu network** yang sama
4. **IP address bisa berubah** setelah restart, cek dengan `hostname -I`
5. **Gallery bisa diakses dari HP** untuk monitoring jarak jauh
6. **Foto disimpan di folder `captures/`**, bisa backup manual

---

## 🗂️ Struktur Project

```
FinalProject_SIC/
├── src/
│   ├── main.cpp                          ← ESP32 Controller code
│   ├── esp32cam_slave.cpp.example        ← ESP32-CAM (tidak dipakai lagi)
│   └── main_esp32cam_slave_backup.cpp    ← Backup ESP32-CAM code
├── camera_server.py                      ← Python server untuk webcam
├── start_camera_server.sh                ← Script start server
├── captures/                             ← Folder foto hasil tangkapan
├── platformio.ini                        ← PlatformIO config
├── requirements.txt                      ← Python dependencies
└── README_WEBCAM.md                      ← File ini
```

---

## 🎉 Selesai!

Sistem sekarang menggunakan **webcam laptop** menggantikan ESP32-CAM!

**Tidak perlu lagi:**

- ❌ Upload ke ESP32-CAM dengan jumper rumit
- ❌ Expansion board atau FTDI adapter
- ❌ Serial communication Serial2
- ❌ Transfer gambar via UART

**Lebih mudah:**

- ✅ Hanya upload ke ESP32 DevKit saja
- ✅ Webcam laptop langsung bisa dipakai
- ✅ HTTP trigger via WiFi (wireless!)
- ✅ Gallery web yang bagus

**Test sekarang: Gerakkan tangan di depan sensor PIR!** 🚀
