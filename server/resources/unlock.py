from flask import Flask
from flask_restful import Resource, reqparse, request
from .datastore import s3
import requests

app = Flask(__name__)

TARGET_URL = "http://172.28.116.119/open-door"

class Unlock(Resource):
    def get(self):
        # s3.download_image('bring-photos', 'verify.png')
 
        return "Unlock", 200

    def post(self):
        # send unlock signal to device
        response = requests.post(TARGET_URL)

        return response.text, response.status_code