import requests

url = 'https://b-ring.vercel.app/guest-verification'
my_img = {'image': open('test-image1.jpg', 'rb')}
r = requests.post(url, files=my_img)

# convert server response into JSON format.
print(r.json())
