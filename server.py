#!/usr/bin/env python3
"""
Simple Image Upload Server
Untuk menerima foto dari ESP32 Security System
"""

from flask import Flask, request, jsonify
import os
from datetime import datetime

app = Flask(__name__)

# Folder untuk menyimpan foto
UPLOAD_FOLDER = 'uploads'
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

@app.route('/')
def index():
    """Halaman utama"""
    files = sorted([f for f in os.listdir(UPLOAD_FOLDER) if f.endswith('.jpg')])
    files.reverse()  # Terbaru di atas
    
    html = """
    <!DOCTYPE html>
    <html>
    <head>
        <title>ESP32 Security System - Image Gallery</title>
        <style>
            body { font-family: Arial; margin: 20px; background: #f0f0f0; }
            h1 { color: #333; }
            .stats { background: white; padding: 15px; border-radius: 5px; margin-bottom: 20px; }
            .gallery { display: grid; grid-template-columns: repeat(auto-fill, minmax(300px, 1fr)); gap: 20px; }
            .card { background: white; padding: 10px; border-radius: 5px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }
            .card img { width: 100%; border-radius: 5px; }
            .card p { margin: 10px 0 0 0; color: #666; font-size: 14px; }
            .status { display: inline-block; padding: 5px 10px; background: #4CAF50; color: white; border-radius: 3px; }
        </style>
    </head>
    <body>
        <h1>🔐 ESP32 Security System - Image Gallery</h1>
        <div class="stats">
            <span class="status">● Server Running</span>
            <p><strong>Total Images:</strong> """ + str(len(files)) + """</p>
            <p><strong>Upload Endpoint:</strong> POST /upload</p>
        </div>
        <div class="gallery">
    """
    
    for filename in files:
        filepath = os.path.join(UPLOAD_FOLDER, filename)
        filesize = os.path.getsize(filepath)
        html += f"""
            <div class="card">
                <img src="/image/{filename}" alt="{filename}">
                <p><strong>{filename}</strong></p>
                <p>Size: {filesize:,} bytes</p>
            </div>
        """
    
    html += """
        </div>
    </body>
    </html>
    """
    
    return html

@app.route('/image/<filename>')
def get_image(filename):
    """Serve image file"""
    from flask import send_file
    filepath = os.path.join(UPLOAD_FOLDER, filename)
    if os.path.exists(filepath):
        return send_file(filepath, mimetype='image/jpeg')
    return "Image not found", 404

@app.route('/upload', methods=['POST'])
def upload_image():
    """Endpoint untuk menerima foto dari ESP32"""
    try:
        # Terima data foto
        image_data = request.get_data()
        
        if len(image_data) == 0:
            return jsonify({"status": "error", "message": "No data received"}), 400
        
        # Buat nama file dengan timestamp
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"motion_{timestamp}.jpg"
        filepath = os.path.join(UPLOAD_FOLDER, filename)
        
        # Simpan foto
        with open(filepath, 'wb') as f:
            f.write(image_data)
        
        print(f"✅ Foto diterima: {filename} ({len(image_data):,} bytes)")
        
        return jsonify({
            "status": "success",
            "filename": filename,
            "size": len(image_data)
        }), 200
        
    except Exception as e:
        print(f"❌ Error: {str(e)}")
        return jsonify({
            "status": "error",
            "message": str(e)
        }), 500

@app.route('/stats')
def stats():
    """API untuk statistik"""
    files = [f for f in os.listdir(UPLOAD_FOLDER) if f.endswith('.jpg')]
    total_size = sum(os.path.getsize(os.path.join(UPLOAD_FOLDER, f)) for f in files)
    
    return jsonify({
        "total_images": len(files),
        "total_size_bytes": total_size,
        "latest_image": max(files) if files else None
    })

if __name__ == '__main__':
    import socket
    
    # Dapatkan IP address
    hostname = socket.gethostname()
    ip_address = socket.gethostbyname(hostname)
    
    print("=" * 60)
    print("🚀 ESP32 Security System - Image Upload Server")
    print("=" * 60)
    print(f"Server running at:")
    print(f"  • Local:   http://localhost:5000")
    print(f"  • Network: http://{ip_address}:5000")
    print(f"")
    print(f"📁 Folder penyimpanan: {os.path.abspath(UPLOAD_FOLDER)}")
    print(f"📸 Gallery: http://{ip_address}:5000")
    print(f"📤 Upload endpoint: http://{ip_address}:5000/upload")
    print(f"")
    print(f"⚙️  Edit ESP32 code:")
    print(f"   const char* serverURL = \"http://{ip_address}:5000/upload\";")
    print("=" * 60)
    print("")
    print("Press Ctrl+C to stop server")
    print("")
    
    app.run(host='0.0.0.0', port=5000, debug=True)
