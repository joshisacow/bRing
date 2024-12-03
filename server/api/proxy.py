from flask import Flask, Response
import requests
from urllib.parse import urljoin

app = Flask(__name__)

@app.route('/api/proxy/<path:url_path>', methods=['POST'])
def proxy_hls(url_path):
    # Base URL of your HLS server
    ESP_URL = "http://172.28.116.119"
    
    # Construct the full URL
    url = urljoin(ESP_URL, url_path)
    print(url)
    
    try:
        # Forward the request to the HLS server
        response = requests.post(url)
        
        return response.content, response.status_code
    except requests.RequestException as e:
        return str(e), 500

if __name__ == '__main__':
    app.run()