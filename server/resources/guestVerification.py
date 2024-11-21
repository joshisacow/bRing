from flask import Flask, send_file
from flask_restful import Resource, reqparse, request
from .datastore import s3
import os

app = Flask(__name__)

class GuestVerification(Resource):
    def get(self):
        # Check if the image exists
        if os.path.exists('test.jpg'):
            # Return the image file
            return send_file('test.jpg', mimetype='image/jpeg')
        else:
            return "No image found", 404
    def post(self):
        # store image and send to ML model + app
        file = request.files['image']
        print(file)
        file.save('test.jpg')
        # data = dict(request.form)
        # image = data['image']

        # upload to aws
        # s3.upload_image(image, 'bring-photos', 'verify.png')

        return "Success", 200