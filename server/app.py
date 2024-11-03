from flask import Flask
from flask_restful import Resource, Api, reqparse
from flask_cors import CORS

app = Flask(__name__)
api = Api(app)
CORS(app)

import json
import base64
import os
from dotenv import load_dotenv, find_dotenv

# # Initialize Firestore DB and decode key
# load_dotenv(find_dotenv())
# encoded_key = os.getenv("SERVICE_ACCOUNT_KEY")
# SERVICE_ACCOUNT_JSON = json.loads(base64.b64decode(encoded_key).decode('utf-8'))

# cred = credentials.Certificate(SERVICE_ACCOUNT_JSON)
# default_app = initialize_app(cred)

# import resources
import resources.guestVerification as Guest 
import resources.unlock as Unlock

class Main(Resource):
    def get(self):
        return "Nav-E API", 200

api.add_resource(Main, '/')
api.add_resource(Guest.GuestVerification, '/guest-verification')
api.add_resource(Unlock.Unlock, '/unlock')


# home route
@app.route("/")
def home():
    return 'bRing API', 200

if __name__ == '__main__':
    app.run(port=8080, debug=True)