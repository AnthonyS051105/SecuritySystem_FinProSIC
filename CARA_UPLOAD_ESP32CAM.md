# 📘 CARA UPLOAD KE ESP32-CAM (Penjelasan Detail)

## 🔌 Apa itu FTDI/USB-to-Serial?

ESP32-CAM **TIDAK PUNYA** port USB micro seperti ESP32 DevKit biasa!

Untuk upload program, butuh **FTDI adapter** (USB to Serial):

- Komputer → USB → FTDI → TX/RX → ESP32-CAM

## 🛠️ Yang Dibutuhkan:

1. **FTDI Adapter** (USB to TTL Serial) - biasanya warna merah/hijau
2. **Kabel Jumper** Female-to-Female
3. **ESP32-CAM**

## 📋 Wiring FTDI ↔ ESP32-CAM:

```
FTDI Adapter    →    ESP32-CAM
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
VCC (5V)        →    5V
GND             →    GND
TXD             →    U0R (atau RX)
RXD             →    U0T (atau TX)

KHUSUS SAAT UPLOAD:
GPIO0           →    GND (pakai jumper)
```

**⚠️ PENTING:** Pin U0R dan U0T biasanya ada tulisan di board ESP32-CAM!

## 🚀 Langkah-langkah Upload:

### 1️⃣ Persiapan Kode

```bash
cd /home/anthony05/Coding/FinalProject_SIC

# Backup kode controller
cp src/main.cpp src/main_controller.cpp.backup

# Copy kode ESP32-CAM
cp src/esp32cam_slave.cpp.example src/main.cpp

# Edit platformio.ini
cat > platformio.ini << 'EOF'
[env:esp32cam]
platform = espressif32
board = esp32cam
framework = arduino
monitor_speed = 115200
upload_speed = 115200
EOF
```

### 2️⃣ Wiring Hardware

1. **Hubungkan FTDI ke ESP32-CAM** sesuai tabel di atas
2. **PENTING:** Hubungkan **GPIO0 ke GND** dengan jumper
   - Ini membuat ESP32-CAM masuk "Flash Mode"
3. Colokkan FTDI ke USB komputer

### 3️⃣ Upload Kode

```bash
# Cek port yang terdeteksi
ls /dev/ttyUSB*
# atau
ls /dev/ttyACM*

# Upload (ganti port sesuai hasil di atas)
pio run -t upload --upload-port /dev/ttyUSB0

# Jika port auto-detect:
pio run -t upload
```

**Monitor saat upload:**

```
Connecting.....
Writing at 0x00001000... (10%)
Writing at 0x00005000... (20%)
...
Hash of data verified.
Leaving...
Hard resetting via RTS pin...
```

### 4️⃣ Test ESP32-CAM

1. **LEPAS jumper GPIO0 dari GND** (PENTING!)
2. **Tekan tombol RESET** di ESP32-CAM
3. Monitor serial:

```bash
pio device monitor -b 115200
```

**Output yang BENAR:**

```
========================================
  ESP32-CAM Slave Module
========================================

[INIT] Menginisialisasi kamera...
[INIT] Kamera berhasil diinisialisasi
[INIT] Serial2 initialized (GPIO14/15)

========================================
  ESP32-CAM Ready!
========================================
```

## ❌ Troubleshooting

### Output Aneh (seperti: K��K�S�`S�'####)

**Penyebab:** Baudrate tidak cocok

**Solusi:**

```bash
# Coba baudrate lain di monitor:
pio device monitor -b 9600
pio device monitor -b 74880
pio device monitor -b 115200
```

### "Connecting......" Stuck

**Penyebab:** GPIO0 tidak terhubung ke GND

**Solusi:**

1. Pastikan GPIO0 terhubung ke GND saat upload
2. Tekan dan tahan tombol IO0 (jika ada) saat upload
3. Coba tekan RESET sambil upload dimulai

### "Failed to connect"

**Penyebab:** Port salah atau FTDI tidak terdeteksi

**Solusi:**

```bash
# Cek device yang terdeteksi
ls -l /dev/ttyUSB*
ls -l /dev/ttyACM*

# Berikan permission (jika perlu)
sudo chmod 666 /dev/ttyUSB0

# Atau tambahkan user ke dialout group
sudo usermod -a -G dialout $USER
# Logout & login lagi
```

### Upload Berhasil Tapi Output Tetap Aneh

**Penyebab:** Lupa lepas GPIO0 dari GND

**Solusi:**

1. Lepas jumper GPIO0 dari GND
2. Tekan RESET
3. Monitor lagi

## 🎯 Checklist Upload ESP32-CAM

- [ ] FTDI terhubung dengan benar (VCC, GND, TX, RX)
- [ ] GPIO0 terhubung ke GND saat upload
- [ ] Port USB terdeteksi di komputer
- [ ] platformio.ini board = esp32cam
- [ ] File main.cpp = kode ESP32-CAM (bukan controller)
- [ ] Upload berhasil (100%)
- [ ] GPIO0 dilepas dari GND setelah upload
- [ ] Tombol RESET ditekan
- [ ] Serial monitor menampilkan output yang benar

## 📸 Foto Referensi Wiring

```
      ESP32-CAM Board
   ╔═══════════════════╗
   ║  [Camera Module]  ║
   ║                   ║
   ║  5V  ●            ║ ← Connect to FTDI 5V
   ║  GND ●            ║ ← Connect to FTDI GND
   ║  U0T ●            ║ ← Connect to FTDI RXD
   ║  U0R ●            ║ ← Connect to FTDI TXD
   ║  GPIO0 ●          ║ ← Connect to GND (saat upload)
   ║                   ║
   ║  [SD Card Slot]   ║
   ╚═══════════════════╝
```

---

**Setelah berhasil upload ESP32-CAM, baru lanjut upload ESP32 Controller!**
