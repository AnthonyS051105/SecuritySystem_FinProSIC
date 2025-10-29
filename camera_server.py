#!/usr/bin/env python3
"""
Security System - Camera Server dengan Webcam Laptop
Menggantikan ESP32-CAM dengan webcam laptop

Server ini menerima trigger dari ESP32 via HTTP,
lalu mengambil foto dari webcam laptop dan menyimpannya.
"""

from flask import Flask, request, jsonify, render_template_string, send_from_directory
import cv2
import os
from datetime import datetime
import json

app = Flask(__name__)

# Konfigurasi
UPLOAD_FOLDER = 'uploads'
CAPTURE_FOLDER = 'captures'
os.makedirs(UPLOAD_FOLDER, exist_ok=True)
os.makedirs(CAPTURE_FOLDER, exist_ok=True)

# Inisialisasi webcam (gunakan index 0 untuk webcam default)
camera = None

def init_camera():
    """Inisialisasi webcam laptop dengan optimasi"""
    global camera
    if camera is None or not camera.isOpened():
        camera = cv2.VideoCapture(0)
        if not camera.isOpened():
            print("[ERROR] Tidak dapat membuka webcam!")
            return False
        
        # Set resolusi untuk performa lebih baik
        camera.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
        camera.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
        
        # OPTIMASI: Set buffer size rendah untuk mengurangi lag
        camera.set(cv2.CAP_PROP_BUFFERSIZE, 1)
        
        # OPTIMASI: Warm-up camera (ambil beberapa frame dummy)
        print("[INIT] Warming up camera...")
        for _ in range(5):
            camera.read()  # Discard dummy frames
        
        print("[INIT] Webcam berhasil diinisialisasi dan ready!")
        return True
    return True

def capture_image():
    """Capture gambar dari webcam dengan optimasi"""
    if not init_camera():
        return None
    
    # OPTIMASI: Flush old frames dari buffer
    for _ in range(2):
        camera.grab()  # Discard buffered frames
    
    # Ambil frame fresh dari webcam
    ret, frame = camera.read()
    
    if not ret:
        print("[ERROR] Gagal mengambil frame dari webcam!")
        return None
    
    # Generate filename dengan timestamp
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    filename = f"motion_{timestamp}.jpg"
    filepath = os.path.join(CAPTURE_FOLDER, filename)
    
    # OPTIMASI: Simpan dengan kompresi JPEG quality 85 (balance speed vs quality)
    cv2.imwrite(filepath, frame, [cv2.IMWRITE_JPEG_QUALITY, 85])
    print(f"[CAMERA] Foto disimpan: {filename}")
    
    return filepath

@app.route('/')
def index():
    """Halaman gallery untuk melihat foto-foto yang tertangkap"""
    images = sorted([f for f in os.listdir(CAPTURE_FOLDER) if f.endswith('.jpg')], reverse=True)
    
    html = '''
    <!DOCTYPE html>
    <html>
    <head>
        <title>Security System - Gallery</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                margin: 20px;
                background: #f0f0f0;
            }
            h1 {
                color: #333;
                text-align: center;
            }
            .stats {
                background: white;
                padding: 20px;
                margin: 20px auto;
                max-width: 800px;
                border-radius: 10px;
                box-shadow: 0 2px 5px rgba(0,0,0,0.1);
            }
            .gallery {
                display: grid;
                grid-template-columns: repeat(auto-fill, minmax(300px, 1fr));
                gap: 20px;
                margin-top: 20px;
            }
            .image-card {
                background: white;
                border-radius: 10px;
                overflow: hidden;
                box-shadow: 0 2px 5px rgba(0,0,0,0.1);
                transition: transform 0.2s;
            }
            .image-card:hover {
                transform: translateY(-5px);
                box-shadow: 0 5px 15px rgba(0,0,0,0.2);
            }
            .image-card img {
                width: 100%;
                height: 200px;
                object-fit: cover;
            }
            .image-info {
                padding: 15px;
            }
            .image-name {
                font-weight: bold;
                color: #333;
            }
            .image-time {
                color: #666;
                font-size: 0.9em;
                margin-top: 5px;
            }
            .no-images {
                text-align: center;
                color: #999;
                padding: 50px;
                background: white;
                border-radius: 10px;
            }
            .btn-test {
                background: #4CAF50;
                color: white;
                border: none;
                padding: 10px 20px;
                border-radius: 5px;
                cursor: pointer;
                font-size: 16px;
                margin: 10px;
            }
            .btn-test:hover {
                background: #45a049;
            }
            .status {
                display: inline-block;
                padding: 5px 10px;
                border-radius: 5px;
                margin: 5px;
            }
            .status-ok {
                background: #4CAF50;
                color: white;
            }
            .status-error {
                background: #f44336;
                color: white;
            }
        </style>
    </head>
    <body>
        <h1>🔒 Security System - Motion Capture Gallery</h1>
        
        <div class="stats">
            <h2>📊 Status Sistem</h2>
            <p><strong>Total Gambar Tertangkap:</strong> {{ total }}</p>
            <p><strong>Status Webcam:</strong> 
                <span class="status status-ok">✓ Aktif</span>
            </p>
            <button class="btn-test" onclick="testCapture()">📸 Test Capture</button>
            <button class="btn-test" onclick="location.reload()">🔄 Refresh</button>
        </div>
        
        {% if images %}
        <div class="gallery">
            {% for img in images %}
            <div class="image-card">
                <img src="/captures/{{ img }}" alt="{{ img }}">
                <div class="image-info">
                    <div class="image-name">{{ img }}</div>
                    <div class="image-time">
                        📅 {{ img.split('_')[1].split('.')[0][:8] }} 
                        🕐 {{ img.split('_')[1].split('.')[0][8:10] }}:{{ img.split('_')[1].split('.')[0][10:12] }}:{{ img.split('_')[1].split('.')[0][12:14] }}
                    </div>
                </div>
            </div>
            {% endfor %}
        </div>
        {% else %}
        <div class="no-images">
            <h2>📷 Belum Ada Gambar</h2>
            <p>Gerakkan tangan di depan sensor PIR untuk memicu capture!</p>
            <p>Atau klik tombol "Test Capture" untuk test webcam.</p>
        </div>
        {% endif %}
        
        <script>
            function testCapture() {
                fetch('/capture', {
                    method: 'POST',
                    headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify({test: true})
                })
                .then(response => response.json())
                .then(data => {
                    alert(data.message);
                    location.reload();
                })
                .catch(error => {
                    alert('Error: ' + error);
                });
            }
        </script>
    </body>
    </html>
    '''
    
    from flask import render_template_string
    return render_template_string(html, images=images, total=len(images))

@app.route('/captures/<filename>')
def serve_image(filename):
    """Serve gambar yang sudah di-capture"""
    return send_from_directory(CAPTURE_FOLDER, filename)

@app.route('/capture', methods=['POST'])
def capture():
    """
    Endpoint untuk trigger capture dari ESP32
    ESP32 akan POST ke endpoint ini saat PIR mendeteksi gerakan
    """
    try:
        # Capture gambar dari webcam
        filepath = capture_image()
        
        if filepath is None:
            return jsonify({
                'success': False,
                'message': 'Gagal mengambil gambar dari webcam'
            }), 500
        
        filename = os.path.basename(filepath)
        
        print(f"[SUCCESS] Motion detected! Foto disimpan: {filename}")
        
        return jsonify({
            'success': True,
            'message': 'Gambar berhasil di-capture',
            'filename': filename,
            'timestamp': datetime.now().isoformat()
        }), 200
        
    except Exception as e:
        print(f"[ERROR] Exception: {str(e)}")
        return jsonify({
            'success': False,
            'message': f'Error: {str(e)}'
        }), 500

@app.route('/status')
def status():
    """Endpoint untuk cek status webcam"""
    webcam_ok = init_camera()
    
    return jsonify({
        'webcam': 'OK' if webcam_ok else 'ERROR',
        'total_images': len([f for f in os.listdir(CAPTURE_FOLDER) if f.endswith('.jpg')]),
        'server': 'running'
    })

@app.route('/api/images')
def api_images():
    """API endpoint untuk list semua gambar"""
    images = sorted([f for f in os.listdir(CAPTURE_FOLDER) if f.endswith('.jpg')], reverse=True)
    
    return jsonify({
        'total': len(images),
        'images': images
    })

def cleanup():
    """Cleanup saat server ditutup"""
    global camera
    if camera is not None:
        camera.release()
        print("[CLEANUP] Webcam released")

if __name__ == '__main__':
    try:
        print("=" * 50)
        print("  Security System - Camera Server")
        print("=" * 50)
        print()
        print("[INIT] Menginisialisasi webcam...")
        
        if init_camera():
            print("[INIT] Webcam siap!")
            print()
            print("[SERVER] Starting Flask server...")
            print("[SERVER] Gallery: http://0.0.0.0:5000/")
            print("[SERVER] Capture endpoint: POST http://0.0.0.0:5000/capture")
            print()
            print("Tekan Ctrl+C untuk stop server")
            print()
            
            app.run(host='0.0.0.0', port=5000, debug=False)
        else:
            print("[ERROR] Gagal menginisialisasi webcam!")
            print("[ERROR] Pastikan webcam terhubung dan tidak digunakan aplikasi lain")
            
    except KeyboardInterrupt:
        print("\n[INFO] Server dihentikan oleh user")
    finally:
        cleanup()
