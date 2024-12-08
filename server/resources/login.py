from flask import Flask
from flask_restful import Resource, reqparse, request
from .datastore import s3

app = Flask(__name__)

class Login(Resource):
    def get(self):
        return "Login", 200

    def post(self):
        data = request.get_json()
        try:
            s3.sign_in(data['username'], data['password'])
        except:
            return "Login Failed", 400
        return "Success", 200
    

class SignUp(Resource):
    def get(self):
        return "Sign Up", 200
    
    def post(self):
        data = request.get_json()
        try:
            s3.sign_up(data['username'], data['password'])
        except:
            return "Sign up Failed", 400
        return "Success", 200