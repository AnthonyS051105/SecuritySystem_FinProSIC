# 📘 PANDUAN LENGKAP UPLOAD & SETUP

## Mini Security System - ESP32 + ESP32-CAM

---

## ✅ PERUBAHAN TERBARU

Kode sudah diupdate untuk sesuai dengan wiring Anda:

| ESP32-CAM        | ESP32 DevKit     | Keterangan                  |
| ---------------- | ---------------- | --------------------------- |
| 5V               | 5V               | Power ✅                    |
| GND              | GND              | Ground bersama ✅           |
| **GPIO14 (TX2)** | **GPIO16 (RX2)** | TX dari CAM ke RX DevKit ✅ |
| **GPIO15 (RX2)** | **GPIO17 (TX2)** | RX dari CAM ke TX DevKit ✅ |

**Kode ESP32-CAM** sudah diupdate menggunakan **Serial2 (GPIO14/15)**  
**Kode ESP32 Controller** sudah benar menggunakan **Serial2 (GPIO16/17)**

---

## 🎯 LANGKAH-LANGKAH LENGKAP

### **📝 PERSIAPAN AWAL**

#### 1️⃣ Install Tools (Skip jika sudah ada)

```bash
# Cek apakah PlatformIO sudah terinstall
pio --version

# Jika belum, install via:
pip install platformio
# atau
curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core/master/scripts/get-platformio.py -o get-platformio.py
python3 get-platformio.py
```

#### 2️⃣ Setup Server untuk Menerima Foto

**OPSI A: Server Python Sederhana (Recommended)**

Buat file `server.py` di folder terpisah:

```python
from flask import Flask, request
import os
from datetime import datetime

app = Flask(__name__)
UPLOAD_FOLDER = 'uploads'
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

@app.route('/upload', methods=['POST'])
def upload_image():
    try:
        image_data = request.get_data()
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"motion_{timestamp}.jpg"
        filepath = os.path.join(UPLOAD_FOLDER, filename)

        with open(filepath, 'wb') as f:
            f.write(image_data)

        print(f"✅ Foto diterima: {filename} ({len(image_data)} bytes)")
        return {"status": "success", "filename": filename}, 200

    except Exception as e:
        print(f"❌ Error: {str(e)}")
        return {"status": "error", "message": str(e)}, 500

if __name__ == '__main__':
    print("🚀 Server running at http://0.0.0.0:5000")
    print("📁 Foto akan disimpan di folder 'uploads'")
    app.run(host='0.0.0.0', port=5000, debug=True)
```

Jalankan:

```bash
# Install Flask
pip install flask

# Jalankan server (biarkan running)
python server.py
```

**Dapatkan IP Server:**

```bash
# Di komputer yang menjalankan server
hostname -I
# Contoh output: 192.168.1.100

# Server URL Anda: http://192.168.1.100:5000/upload
```

**OPSI B: Webhook.site (Testing tanpa coding)**

1. Buka https://webhook.site
2. Copy URL yang muncul
3. Gunakan sebagai serverURL

---

### **⚡ FASE 1: UPLOAD KE ESP32-CAM (Sekali Saja)**

#### Langkah 1: Persiapan File

```bash
cd /home/anthony05/Coding/FinalProject_SIC

# Backup file main.cpp saat ini (ESP32 Controller)
cp src/main.cpp src/main_controller.cpp.backup

# Copy kode ESP32-CAM menjadi main.cpp
cp src/esp32cam_slave.cpp.example src/main.cpp
```

#### Langkah 2: Edit platformio.ini

Edit file `platformio.ini`:

```ini
[env:esp32cam]
platform = espressif32
board = esp32cam
framework = arduino
monitor_speed = 115200
```

**Atau jalankan script otomatis:**

```bash
cat > platformio.ini << 'EOF'
[env:esp32cam]
platform = espressif32
board = esp32cam
framework = arduino
monitor_speed = 115200
EOF
```

#### Langkah 3: Hubungkan ESP32-CAM ke Komputer

**MENGGUNAKAN USB-to-TTL (FTDI):**

```
FTDI/USB-TTL  →  ESP32-CAM
━━━━━━━━━━━━━━━━━━━━━━━━━━
5V            →  5V
GND           →  GND
TX            →  U0R (GPIO3)
RX            →  U0T (GPIO1)
```

**⚠️ PENTING: Masuk Mode Programming**

- Hubungkan **GPIO0 ke GND** (gunakan jumper)
- Ini membuat ESP32-CAM masuk mode flash/upload

#### Langkah 4: Upload ke ESP32-CAM

```bash
# Build & Upload
pio run -t upload

# Atau jika port tidak terdeteksi otomatis:
pio run -t upload --upload-port /dev/ttyUSB0
# Ganti /dev/ttyUSB0 dengan port yang terdeteksi

# Cek port yang tersedia:
ls /dev/ttyUSB*
# atau
ls /dev/ttyACM*
```

**Monitor progress:**

```
Connecting........_____....._____....._____
Writing at 0x00001000... (10%)
Writing at 0x00005000... (20%)
...
Writing at 0x000d0000... (100%)
Wrote 1234567 bytes (789012 compressed) at 0x00010000 in 45.6 seconds
Hash of data verified.

Leaving...
Hard resetting via RTS pin...
```

#### Langkah 5: Test ESP32-CAM

```bash
# PENTING: Lepas GPIO0 dari GND!
# Tekan tombol RESET di ESP32-CAM

# Monitor Serial
pio device monitor -b 115200
```

**Output yang diharapkan:**

```
========================================
  ESP32-CAM Slave Module
========================================

[INIT] Menginisialisasi kamera...
[INIT] Kamera berhasil diinisialisasi
[INIT] Menginisialisasi SD Card...
[INIT] SD Card berhasil diinisialisasi
[INIT] Serial2 initialized (GPIO14/15)

========================================
  ESP32-CAM Ready!
========================================
```

**✅ ESP32-CAM SIAP! Lepas dari komputer.**

---

### **🎮 FASE 2: UPLOAD KE ESP32 CONTROLLER**

#### Langkah 1: Restore File Controller

```bash
cd /home/anthony05/Coding/FinalProject_SIC

# Backup kode ESP32-CAM yang baru di-upload
cp src/main.cpp src/main_esp32cam.cpp.backup

# Restore kode ESP32 Controller
cp src/main_controller.cpp.backup src/main.cpp
```

#### Langkah 2: Edit platformio.ini

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
```

**Atau script otomatis:**

```bash
cat > platformio.ini << 'EOF'
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
EOF
```

#### Langkah 3: Update WiFi & Server URL

Edit `src/main.cpp` baris 20-22:

```cpp
// WiFi credentials
const char* ssid = "Adhyasta";                      // ✅ Sudah benar
const char* password = "juarasatu";                 // ✅ Sudah benar
const char* serverURL = "http://192.168.1.100:5000/upload"; // ⚠️ GANTI!
```

**Ganti dengan IP server Anda!**

#### Langkah 4: Hubungkan ESP32 DevKit

**Via USB Langsung:**

- Colokkan ESP32 DevKit ke USB komputer
- Tidak perlu GPIO0 ke GND (auto-reset)

#### Langkah 5: Upload ke ESP32 Controller

```bash
# Build & Upload
pio run -t upload

# Monitor langsung setelah upload
pio device monitor -b 115200
```

**Output yang diharapkan:**

```
========================================
  Mini Security System - ESP32 Controller
========================================

[INIT] Menginisialisasi pin GPIO...
[INIT] Pin GPIO berhasil diinisialisasi
[INIT] Menginisialisasi komunikasi dengan ESP32-CAM...
[WARNING] ESP32-CAM tidak merespons!
[WARNING] Pastikan ESP32-CAM sudah diprogram dan terhubung
[INIT] Menghubungkan ke WiFi...
[INIT] SSID: Adhyasta
..........
[INIT] WiFi berhasil terhubung!
[INIT] IP Address: 192.168.1.XXX
  Sistem Siap!
[STATUS] System Armed - Monitoring aktif
[STATUS] WiFi Connected - IP: 192.168.1.XXX
[STATUS] Images captured: 0
```

**⚠️ Normal jika ESP32-CAM tidak merespons karena belum dihubungkan secara fisik!**

---

### **🔌 FASE 3: WIRING HARDWARE**

#### Langkah 1: Hubungkan ESP32-CAM ↔ ESP32 DevKit

Sesuai tabel wiring Anda:

```
ESP32-CAM     →  ESP32 DevKit
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
5V            →  5V atau VIN
GND           →  GND
GPIO14 (TX2)  →  GPIO16 (RX2)
GPIO15 (RX2)  →  GPIO17 (TX2)
```

**Tips:**

- Gunakan kabel jumper yang baik
- Pastikan koneksi kuat (tidak longgar)
- **GND WAJIB TERHUBUNG!**

#### Langkah 2: Hubungkan Sensor & Aktuator

**PIR Sensor ke ESP32 DevKit:**

```
PIR Sensor    →  ESP32 DevKit
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
VCC           →  5V
GND           →  GND
OUT           →  GPIO13
```

**LED ke ESP32 DevKit:**

```
LED           →  ESP32 DevKit
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Anode (+)     →  GPIO33 → Resistor 220Ω
Cathode (-)   →  GND
```

**Switch ke ESP32 DevKit:**

```
Switch        →  ESP32 DevKit
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Pin 1         →  GPIO12
Pin 2         →  GND
```

#### Langkah 3: Power Supply

**OPSI A: USB Power (Testing)**

- ESP32 DevKit via USB komputer
- ESP32-CAM via pin 5V dari DevKit

**OPSI B: External Power (Recommended)**

- ESP32 DevKit via USB charger 5V 2A
- ESP32-CAM via pin 5V dari DevKit
- Atau power ESP32-CAM dari power supply terpisah 5V 2A

---

### **🧪 FASE 4: TESTING SISTEM**

#### Langkah 1: Power On

```bash
# Hubungkan ESP32 DevKit ke USB
# Monitor Serial
pio device monitor -b 115200
```

#### Langkah 2: Cek Komunikasi ESP32-CAM

**Output yang diharapkan:**

```
========================================
  Mini Security System - ESP32 Controller
========================================

[INIT] Menginisialisasi pin GPIO...
[INIT] Pin GPIO berhasil diinisialisasi
[INIT] Menginisialisasi komunikasi dengan ESP32-CAM...
[INIT] ESP32-CAM berhasil terhubung!  ← HARUS MUNCUL INI!
[INIT] Menghubungkan ke WiFi...
[INIT] WiFi berhasil terhubung!
[INIT] IP Address: 192.168.1.XXX
```

**✅ Jika muncul "ESP32-CAM berhasil terhubung!" = WIRING BENAR!**

**❌ Jika muncul "ESP32-CAM tidak merespons" = CEK WIRING:**

- Apakah GPIO14/15/GND terhubung dengan benar?
- Apakah ESP32-CAM mendapat power (LED menyala)?
- Coba swap TX/RX jika masih error

#### Langkah 3: Test Motion Detection

1. **Tunggu PIR warming up** (30-60 detik)
2. **Gerakkan tangan di depan PIR sensor**
3. **Perhatikan LED** - harus menyala!

**Output di Serial Monitor:**

```
[STATUS] Monitoring...

========================================
[ALERT] Motion Detected!
========================================
[ACTION] LED dihidupkan
[CAMERA] Mengirim perintah ke ESP32-CAM...
[CAMERA] ESP32-CAM siap mengirim data
[CAMERA] Menerima data gambar...
[CAMERA] Ukuran gambar: 15420 bytes
[CAMERA] Berhasil menerima 15420 bytes
[SUCCESS] Image Captured
[HTTP] Mengirim foto ke server...
[HTTP] Response code: 200
[HTTP] Response: {"status":"success","filename":"motion_20251028_143052.jpg"}
[ACTION] LED dimatikan
========================================
```

#### Langkah 4: Cek Foto di Server

**Jika menggunakan Python Flask:**

```bash
# Lihat folder uploads/
ls -lh uploads/
# Seharusnya ada file: motion_YYYYMMDD_HHMMSS.jpg

# Buka foto untuk verifikasi
xdg-open uploads/motion_*.jpg
```

**Jika menggunakan Webhook.site:**

- Refresh halaman webhook.site
- Klik request terbaru
- Lihat Raw Content → Download as image

#### Langkah 5: Test Armed/Disarmed Switch

1. **Tekan switch** (GPIO12 ke GND)
2. **Sistem akan disarmed**

**Output:**

```
========================================
[STATUS] System Disarmed - Monitoring tidak aktif
[STATUS] WiFi Connected - IP: 192.168.1.XXX
[STATUS] Images captured: 1
========================================
```

3. **Gerakkan tangan di depan PIR** - tidak ada response (normal)
4. **Tekan switch lagi** - sistem kembali armed
5. **Gerakkan tangan** - LED menyala, foto diambil

---

## 🎯 RANGKUMAN LENGKAP

### ✅ Checklist Upload ESP32-CAM:

- [ ] File `esp32cam_slave.cpp.example` dijadikan `main.cpp`
- [ ] `platformio.ini` board = `esp32cam`
- [ ] GPIO0 dihubungkan ke GND saat upload
- [ ] Upload berhasil
- [ ] GPIO0 dilepas, ESP32-CAM di-RESET
- [ ] Serial monitor menampilkan "ESP32-CAM Ready!"

### ✅ Checklist Upload ESP32 Controller:

- [ ] File `main_controller.cpp.backup` dikembalikan ke `main.cpp`
- [ ] `platformio.ini` board = `esp32dev`
- [ ] WiFi SSID & password sudah benar
- [ ] Server URL sudah diisi dengan IP yang benar
- [ ] Upload berhasil
- [ ] Serial monitor menampilkan "Sistem Siap!"

### ✅ Checklist Wiring:

- [ ] ESP32-CAM 5V ↔ ESP32 5V
- [ ] ESP32-CAM GND ↔ ESP32 GND
- [ ] ESP32-CAM GPIO14 → ESP32 GPIO16
- [ ] ESP32-CAM GPIO15 ← ESP32 GPIO17
- [ ] PIR VCC → 5V, GND → GND, OUT → GPIO13
- [ ] LED + → GPIO33 (via 220Ω), - → GND
- [ ] Switch → GPIO12 dan GND

### ✅ Checklist Testing:

- [ ] Server berjalan dan accessible
- [ ] ESP32-CAM terhubung (tampil di serial)
- [ ] WiFi connected
- [ ] PIR mendeteksi gerakan
- [ ] LED menyala saat deteksi
- [ ] Foto terkirim ke server
- [ ] Switch armed/disarmed berfungsi

---

## 🆘 TROUBLESHOOTING

### ❌ "ESP32-CAM tidak merespons"

**Solusi:**

1. Cek wiring GPIO14/15/GND
2. Pastikan ESP32-CAM mendapat power (coba power terpisah)
3. Coba swap TX/RX (GPIO14 ↔ GPIO15)
4. Upload ulang ke ESP32-CAM
5. Cek apakah Serial2 di ESP32-CAM berfungsi

### ❌ "WiFi gagal terhubung"

**Solusi:**

1. Cek SSID & password (case-sensitive!)
2. Pastikan WiFi 2.4GHz (ESP32 tidak support 5GHz)
3. Dekatkan ESP32 ke router
4. Coba WiFi hotspot HP untuk testing

### ❌ "PIR tidak mendeteksi"

**Solusi:**

1. Tunggu 30-60 detik warming up
2. Cek koneksi VCC, GND, OUT
3. Adjust sensitivity di potentiometer PIR
4. Test dengan gerakan besar di depan sensor

### ❌ "Timeout menerima data gambar"

**Solusi:**

1. Gambar terlalu besar untuk Serial (edit resolusi di ESP32-CAM)
2. Koneksi Serial tidak stabil (cek kabel)
3. Tingkatkan MAX_IMAGE_SIZE di controller
4. Cek baudrate sama (115200)

### ❌ "HTTP POST gagal"

**Solusi:**

1. Cek server running
2. Ping IP server dari HP yang sama dengan ESP32
3. Cek firewall komputer server
4. Coba webhook.site untuk testing

---

## 📞 BANTUAN LEBIH LANJUT

Jika masih ada masalah:

1. Screenshot output Serial Monitor
2. Foto wiring hardware
3. Copy error message lengkap
4. Tanyakan dengan detail masalahnya

**Good luck! 🚀**
