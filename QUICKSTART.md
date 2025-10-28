# 🚀 QUICK START - ESP32 Security System

## ✅ KODE SUDAH BENAR!

Kode sudah disesuaikan dengan wiring Anda:

- ESP32-CAM GPIO14 (TX2) → ESP32 GPIO16 (RX2) ✅
- ESP32-CAM GPIO15 (RX2) → ESP32 GPIO17 (TX2) ✅

---

## 📋 LANGKAH SINGKAT

### 1️⃣ Setup Server (Jalankan di Terminal 1)

```bash
cd /home/anthony05/Coding/FinalProject_SIC

# Install Flask & jalankan server
./start_server.sh

# Server akan menampilkan:
# Server running at:
#   • Network: http://192.168.1.XXX:5000  ← CATAT IP INI!
```

**Copy IP server yang muncul!**

---

### 2️⃣ Upload ke ESP32-CAM (Sekali Saja)

**Terminal 2:**

```bash
cd /home/anthony05/Coding/FinalProject_SIC

# Backup controller code
cp src/main.cpp src/main_controller.cpp.backup

# Gunakan ESP32-CAM code
cp src/esp32cam_slave.cpp.example src/main.cpp

# Set board ke ESP32-CAM
cat > platformio.ini << 'EOF'
[env:esp32cam]
platform = espressif32
board = esp32cam
framework = arduino
monitor_speed = 115200
EOF

# Hubungkan ESP32-CAM ke USB (via FTDI)
# PENTING: Hubungkan GPIO0 ke GND!

# Upload
pio run -t upload

# Lepas GPIO0, tekan RESET di ESP32-CAM
# Monitor untuk verifikasi
pio device monitor
# Tunggu sampai muncul "ESP32-CAM Ready!"
# Tekan Ctrl+C untuk keluar
```

---

### 3️⃣ Upload ke ESP32 Controller

```bash
# Restore controller code
cp src/main_controller.cpp.backup src/main.cpp

# Set board ke ESP32 Dev
cat > platformio.ini << 'EOF'
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
EOF

# Edit server URL di src/main.cpp baris 22
# Ganti: const char* serverURL = "http://192.168.1.XXX:5000/upload";
# Dengan IP dari langkah 1!
nano src/main.cpp
# atau
code src/main.cpp

# Hubungkan ESP32 DevKit ke USB
# Upload
pio run -t upload

# Monitor
pio device monitor
```

---

### 4️⃣ Wiring Hardware

```
ESP32-CAM ↔ ESP32 DevKit:
  5V       → 5V
  GND      → GND
  GPIO14   → GPIO16
  GPIO15   → GPIO17

PIR → ESP32 DevKit:
  VCC → 5V
  GND → GND
  OUT → GPIO13

LED → ESP32 DevKit:
  + → GPIO33 [via 220Ω] → GND

Switch → ESP32 DevKit:
  → GPIO12 ↔ GND
```

---

### 5️⃣ Test!

**Di Serial Monitor, harus muncul:**

```
[INIT] ESP32-CAM berhasil terhubung!  ← PENTING!
[INIT] WiFi berhasil terhubung!
[STATUS] System Armed
```

**Gerakkan tangan di depan PIR:**

- LED menyala ✅
- Serial monitor: "Motion Detected!" ✅
- Foto dikirim ke server ✅

**Buka browser:** `http://IP_SERVER:5000`

- Lihat gallery foto yang tertangkap! 📸

---

## 🆘 TROUBLESHOOTING CEPAT

### ESP32-CAM tidak terhubung?

```bash
# Cek wiring GPIO14/15/GND
# Pastikan ESP32-CAM powered (LED nyala)
# Coba swap GPIO14 ↔ GPIO15
```

### WiFi gagal?

```bash
# Cek SSID & password di src/main.cpp
# Pastikan WiFi 2.4GHz
# Dekatkan ESP32 ke router
```

### PIR tidak deteksi?

```bash
# Tunggu 30-60 detik (warming up)
# Gerakan besar di depan sensor
# Adjust sensitivity di PIR
```

---

## 📄 Dokumentasi Lengkap

- **PANDUAN_UPLOAD.md** - Tutorial detail step-by-step
- **SETUP_GUIDE.md** - Setup hardware & troubleshooting
- **README.md** - Overview project

---

## ✅ CHECKLIST

Pastikan sebelum test:

- [ ] Server running (./start_server.sh)
- [ ] IP server sudah di-copy ke src/main.cpp
- [ ] ESP32-CAM sudah di-upload & menampilkan "Ready"
- [ ] ESP32 Controller sudah di-upload
- [ ] Wiring sesuai diagram (GPIO14→16, GPIO15→17, GND)
- [ ] WiFi SSID & password benar
- [ ] PIR, LED, Switch sudah terhubung

**Selamat mencoba! 🎉**
