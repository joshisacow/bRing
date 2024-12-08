from flask import Flask, send_file
from flask_restful import Resource, reqparse, request
from .datastore import s3
import os
import threading
import numpy as np
import os
from PIL import Image
import sys

app = Flask(__name__)
state_lock = threading.Lock()
state = False

def rgb565_to_rgb888(data):
    """
    Convert RGB565 binary data to RGB888 format for QQVGA (160x120)
    
    Parameters:
    data (bytes): RGB565 binary data
    
    Returns:
    numpy.ndarray: RGB888 image array
    """
    # QQVGA dimensions
    width = 160
    height = 120
    total_pixels = width * height
    bytes_needed = total_pixels * 2  # 2 bytes per pixel for RGB565
    
    # Create a zero-filled buffer of the correct size
    padded_data = bytearray(bytes_needed)
    
    # Copy the received data into the buffer
    padded_data[:len(data)] = data
    
    # Convert binary data to 16-bit unsigned integers
    pixels = np.frombuffer(padded_data, dtype=np.uint16)
    
    # Reshape to QQVGA dimensions
    pixels = pixels.reshape(height, width)
    
    # Extract RGB components
    r = ((pixels & 0xF800) >> 11) * 255 // 31  # 5 bits for red
    g = ((pixels & 0x07E0) >> 5) * 255 // 63   # 6 bits for green
    b = (pixels & 0x001F) * 255 // 31          # 5 bits for blue
    
    # Stack RGB channels
    rgb = np.stack([r, g, b], axis=-1).astype(np.uint8)
    return rgb


class GuestVerification(Resource):
    def get(self):
        # # Check if the image exists
        # if os.path.exists('test.jpg'):
        #     # Return the image file
        #     return send_file('test.jpg', mimetype='image/jpeg')
        # else:
        #     return "No image found", 404
        global state
        if state:
            with state_lock:
                state = False
            if os.path.exists('resources/rgb565_images/test.png'):
                return send_file('resources/rgb565_images/test.png', mimetype='image/png')
            else:
                return "No image", 200
        return False, 200
    
    def post(self):
        # store image and send to ML model + app

        with state_lock:
            global state
            state = True

        try:
            raw_data = request.get_data()

            # Create directory for saving images if it doesn't exist
            save_dir = 'resources/rgb565_images'
            os.makedirs(save_dir, exist_ok=True)
            
            # Save raw RGB565 data
            raw_filename = os.path.join(save_dir, 'test.rgb565')
            with open(raw_filename, 'wb') as f:
                f.write(raw_data)
                
            # Convert and save as readable format
            rgb888_data = rgb565_to_rgb888(raw_data)
            img = Image.fromarray(rgb888_data)
            png_filename = os.path.join(save_dir, 'test.png')
            img.save(png_filename)
        except Exception as e:
            print(e)
            return "Error", 400
        return "Success", 200