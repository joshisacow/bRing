from flask import Flask
from flask_restful import Resource, reqparse, request
from .datastore import s3
import requests

app = Flask(__name__)

class Unlock(Resource):
    def get(self):
        # s3.download_image('bring-photos', 'verify.png')

        return "Unlock", 200

    def post(self):
        # send unlock signal to device
        # resp = requests.post('http://172.28.116.119/open-door')
        resp = requests.post('https://b-ring.vercel.app/api/proxy/open-door')

        return resp.text, resp.status_code