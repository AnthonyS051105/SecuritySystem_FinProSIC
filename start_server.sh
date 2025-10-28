#!/bin/bash

# Quick Start Server Script
# Untuk menjalankan server penerima foto

echo "========================================"
echo "  ESP32 Security System - Server Setup"
echo "========================================"
echo ""

# Cek apakah Flask sudah terinstall
if ! python3 -c "import flask" 2>/dev/null; then
    echo "📦 Installing Flask..."
    pip3 install -r requirements.txt
    echo "✅ Flask installed!"
    echo ""
fi

# Jalankan server
echo "🚀 Starting server..."
echo ""
python3 server.py
