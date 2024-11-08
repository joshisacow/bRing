from flask import Flask
from flask_restful import Resource, reqparse, request
from .datastore import s3
3

app = Flask(__name__)

class Unlock(Resource):
    def get(self):
        s3.download_image('bring-photos', 'verify.png')

        return "Unlock", 200

    def post(self):
        # send unlock signal to device
        return "Success", 200