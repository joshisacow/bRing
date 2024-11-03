from flask import Flask
from flask_restful import Resource, reqparse, request

app = Flask(__name__)

class Unlock(Resource):
    def post(self):
        # send unlock signal to device
        return "Success", 200