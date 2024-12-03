from flask import Flask
from flask_restful import Resource, reqparse, request

app = Flask(__name__)
username = ""
password = ""


class Login(Resource):
    def get(self):
        return "Login", 200

    def post(self):
        data = request.get_json()
        global username
        global password
        username = data['username']
        password = data['password']
        return "Success", 200