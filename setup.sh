#!/bin/bash

# Script helper untuk switch mode programming
# Antara ESP32 Controller dan ESP32-CAM

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
SRC_DIR="$SCRIPT_DIR/src"
PLATFORMIO_INI="$SCRIPT_DIR/platformio.ini"

# Warna untuk output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_header() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}  $1${NC}"
    echo -e "${BLUE}========================================${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

print_info() {
    echo -e "${BLUE}ℹ $1${NC}"
}

# Fungsi untuk switch ke mode ESP32-CAM
setup_esp32cam() {
    print_header "Setup untuk ESP32-CAM"
    
    # Backup file controller jika ada
    if [ -f "$SRC_DIR/main.cpp" ]; then
        print_info "Backup main.cpp (ESP32 Controller)..."
        cp "$SRC_DIR/main.cpp" "$SRC_DIR/main_esp32_controller.cpp.backup"
        print_success "Backup selesai: main_esp32_controller.cpp.backup"
    fi
    
    # Copy file ESP32-CAM slave
    if [ -f "$SRC_DIR/esp32cam_slave.cpp.example" ]; then
        print_info "Copy file ESP32-CAM slave ke main.cpp..."
        cp "$SRC_DIR/esp32cam_slave.cpp.example" "$SRC_DIR/main.cpp"
        print_success "File ESP32-CAM slave siap"
    else
        print_error "File esp32cam_slave.cpp.example tidak ditemukan!"
        exit 1
    fi
    
    # Update platformio.ini
    print_info "Update platformio.ini untuk ESP32-CAM..."
    cat > "$PLATFORMIO_INI" << EOF
; PlatformIO Project Configuration File
; Mode: ESP32-CAM Programming

[env:esp32cam]
platform = espressif32
board = esp32cam
framework = arduino
monitor_speed = 115200
EOF
    print_success "platformio.ini updated untuk ESP32-CAM"
    
    echo ""
    print_header "Instruksi ESP32-CAM"
    print_warning "PENTING: Untuk upload ke ESP32-CAM:"
    echo "  1. Hubungkan GPIO0 ke GND (masuk mode programming)"
    echo "  2. Hubungkan FTDI USB-to-Serial:"
    echo "     - FTDI TX → ESP32-CAM RX (GPIO3)"
    echo "     - FTDI RX → ESP32-CAM TX (GPIO1)"
    echo "     - FTDI GND → ESP32-CAM GND"
    echo "     - FTDI 5V → ESP32-CAM 5V"
    echo "  3. Jalankan: pio run --target upload"
    echo "  4. Setelah upload, lepas GPIO0 dari GND"
    echo "  5. Tekan tombol RESET di ESP32-CAM"
    echo ""
    print_success "Siap untuk programming ESP32-CAM!"
}

# Fungsi untuk switch ke mode ESP32 Controller
setup_esp32dev() {
    print_header "Setup untuk ESP32 Controller"
    
    # Backup file ESP32-CAM jika ada
    if [ -f "$SRC_DIR/main.cpp" ]; then
        # Cek apakah ini file ESP32-CAM
        if grep -q "ESP32-CAM Slave Module" "$SRC_DIR/main.cpp"; then
            print_info "Backup main.cpp (ESP32-CAM)..."
            cp "$SRC_DIR/main.cpp" "$SRC_DIR/main_esp32cam.cpp.backup"
            print_success "Backup selesai: main_esp32cam.cpp.backup"
        fi
    fi
    
    # Restore file controller
    if [ -f "$SRC_DIR/main_esp32_controller.cpp.backup" ]; then
        print_info "Restore file ESP32 Controller ke main.cpp..."
        cp "$SRC_DIR/main_esp32_controller.cpp.backup" "$SRC_DIR/main.cpp"
        print_success "File ESP32 Controller restored"
    else
        print_error "File main_esp32_controller.cpp.backup tidak ditemukan!"
        print_error "Pastikan Anda sudah menjalankan mode esp32cam terlebih dahulu"
        exit 1
    fi
    
    # Update platformio.ini
    print_info "Update platformio.ini untuk ESP32 Dev Board..."
    cat > "$PLATFORMIO_INI" << EOF
; PlatformIO Project Configuration File
; Mode: ESP32 Controller Programming

[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps = 
    ; Library untuk komunikasi dengan ESP32-CAM
EOF
    print_success "platformio.ini updated untuk ESP32 Dev Board"
    
    echo ""
    print_header "Instruksi ESP32 Controller"
    print_warning "JANGAN LUPA:"
    echo "  1. Edit WiFi credentials di main.cpp:"
    echo "     - const char* ssid = \"YOUR_WIFI_SSID\";"
    echo "     - const char* password = \"YOUR_WIFI_PASSWORD\";"
    echo "  2. Edit server URL:"
    echo "     - const char* serverURL = \"http://...:PORT/upload\";"
    echo "  3. Hubungkan ESP32 Dev Board via USB"
    echo "  4. Jalankan: pio run --target upload"
    echo ""
    print_success "Siap untuk programming ESP32 Controller!"
}

# Fungsi untuk menampilkan status
show_status() {
    print_header "Status Project"
    
    echo -e "${BLUE}Current Configuration:${NC}"
    if [ -f "$PLATFORMIO_INI" ]; then
        if grep -q "esp32cam" "$PLATFORMIO_INI"; then
            echo "  Board: ESP32-CAM"
        elif grep -q "esp32dev" "$PLATFORMIO_INI"; then
            echo "  Board: ESP32 Dev Board (Controller)"
        else
            echo "  Board: Unknown"
        fi
    else
        print_error "platformio.ini not found!"
    fi
    
    echo ""
    echo -e "${BLUE}Available Files:${NC}"
    
    if [ -f "$SRC_DIR/main.cpp" ]; then
        if grep -q "ESP32-CAM Slave Module" "$SRC_DIR/main.cpp"; then
            echo "  ✓ main.cpp (ESP32-CAM Slave)"
        elif grep -q "ESP32 Controller" "$SRC_DIR/main.cpp"; then
            echo "  ✓ main.cpp (ESP32 Controller)"
        else
            echo "  ? main.cpp (Unknown)"
        fi
    fi
    
    [ -f "$SRC_DIR/esp32cam_slave.cpp.example" ] && echo "  ✓ esp32cam_slave.cpp.example"
    [ -f "$SRC_DIR/main_esp32_controller.cpp.backup" ] && echo "  ✓ main_esp32_controller.cpp.backup"
    [ -f "$SRC_DIR/main_esp32cam.cpp.backup" ] && echo "  ✓ main_esp32cam.cpp.backup"
}

# Menu utama
show_menu() {
    echo ""
    print_header "Security System - Programming Helper"
    echo ""
    echo "Pilih mode:"
    echo "  1) Setup untuk ESP32-CAM (upload ke ESP32-CAM)"
    echo "  2) Setup untuk ESP32 Controller (upload ke ESP32 Dev Board)"
    echo "  3) Lihat status project"
    echo "  4) Exit"
    echo ""
    echo -n "Pilihan [1-4]: "
}

# Main script
case "$1" in
    esp32cam)
        setup_esp32cam
        ;;
    esp32dev)
        setup_esp32dev
        ;;
    status)
        show_status
        ;;
    *)
        while true; do
            show_menu
            read choice
            case $choice in
                1)
                    setup_esp32cam
                    break
                    ;;
                2)
                    setup_esp32dev
                    break
                    ;;
                3)
                    show_status
                    ;;
                4)
                    echo "Goodbye!"
                    exit 0
                    ;;
                *)
                    print_error "Pilihan tidak valid!"
                    ;;
            esac
        done
        ;;
esac

echo ""
print_info "Untuk build & upload, jalankan: pio run --target upload"
print_info "Untuk monitoring serial: pio device monitor"
echo ""
