import boto3
import os
from dotenv import load_dotenv, find_dotenv 
from hashlib import sha256

load_dotenv(find_dotenv())
aws_id = os.getenv("AWS_ID")
aws_key = os.getenv("AWS_KEY")

s3 = boto3.client('s3', aws_access_key_id=aws_id, aws_secret_access_key=aws_key)

def upload_image(image, bucket, key):
    s3.upload_fileobj(image, bucket, key)

def download_image(bucket, key):
    s3.download_fileobj(bucket, key, key)

def sign_up(username, password):
    # create a new user
    password_hash = sha256(password.encode()).hexdigest()
    s3.put_object(Bucket='users', Key=username, Body=password_hash)

def sign_in(username, password):
    # check if the user exists
    try:
        password_hash = s3.get_object(Bucket='users', Key=username)['Body'].read().decode()
        if password_hash == sha256(password.encode()).hexdigest():
            return True
        else:
            return False
    except:
        return False