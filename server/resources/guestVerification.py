from flask import Flask
from flask_restful import Resource, reqparse, request

app = Flask(__name__)

class GuestVerification(Resource):
    def post(self):
        # store image and send to ML model + app
        return "Success", 200