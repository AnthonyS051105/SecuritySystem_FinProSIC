#!/bin/bash

# Script untuk menjalankan Camera Server dengan Webcam Laptop

echo "================================================"
echo "  Security System - Camera Server dengan Webcam"
echo "================================================"
echo ""

# Install dependencies jika belum ada
echo "[SETUP] Memeriksa dependencies..."
if ! python3 -c "import cv2" 2>/dev/null; then
    echo "[INSTALL] Installing opencv-python..."
    pip3 install opencv-python
fi

if ! python3 -c "import flask" 2>/dev/null; then
    echo "[INSTALL] Installing flask..."
    pip3 install flask
fi

echo ""
echo "[INFO] Dependencies OK!"
echo ""

# Dapatkan IP address
IP=$(hostname -I | awk '{print $1}')
echo "======================================"
echo "  Server akan berjalan di:"
echo "  http://$IP:5000"
echo "======================================"
echo ""
echo "📌 PENTING: Ganti IP di ESP32 Controller!"
echo "   Edit src/main.cpp baris 35:"
echo "   const char* captureURL = \"http://$IP:5000/capture\";"
echo ""
echo "======================================"
echo ""
echo "✅ Fitur:"
echo "   • Gallery: http://$IP:5000/"
echo "   • Capture endpoint: POST http://$IP:5000/capture"
echo "   • Status: GET http://$IP:5000/status"
echo ""
echo "Tekan Ctrl+C untuk stop server"
echo ""

# Jalankan server
python3 camera_server.py
