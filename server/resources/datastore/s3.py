import boto3
import os
from dotenv import load_dotenv, find_dotenv 

load_dotenv(find_dotenv())
aws_id = os.getenv("AWS_ID")
aws_key = os.getenv("AWS_KEY")

s3 = boto3.client('s3', aws_access_key_id=aws_id, aws_secret_access_key=aws_key)

def upload_image(image, bucket, key):
    s3.upload_fileobj(image, bucket, key)

def download_image(bucket, key):
    s3.download_fileobj(bucket, key, key)
