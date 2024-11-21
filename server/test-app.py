import requests

url = 'http://10.197.43.108:8080/guest-verification'
my_img = {'image': open('test-image.jpg', 'rb')}
r = requests.post(url, files=my_img)

# convert server response into JSON format.
print(r.json())
