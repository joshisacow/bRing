from flask import Flask
from flask_restful import Resource, reqparse, request
from .datastore import s3

app = Flask(__name__)

class GuestVerification(Resource):
    def post(self):
        # store image and send to ML model + app
        file = request.files['image']
        print(file)
        file.save('verify.png')
        # data = dict(request.form)
        # image = data['image']

        # upload to aws
        # s3.upload_image(image, 'bring-photos', 'verify.png')

        return "Success", 200