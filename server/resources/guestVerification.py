from flask import Flask, send_file
from flask_restful import Resource, reqparse, request
from .datastore import s3
import os
import threading

app = Flask(__name__)
state_lock = threading.Lock()
state = False

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
            return True, 200
        return False, 200
    
    def post(self):
        # store image and send to ML model + app
        # file = request.files['image']

        with state_lock:
            global state
            state = True


        # print(file)
        # file.save('test.jpg')
        # data = dict(request.form)
        # image = data['image']

        # upload to aws
        # s3.upload_image(image, 'bring-photos', 'verify.png')

        return "Success", 200