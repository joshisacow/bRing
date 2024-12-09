# 🛠️ bRing: IoT Smart Door-Opener

Duke ECE / CS 655 Course Project - Josh, Bryant, Emily

### Overview

bRing is an end-to-end remote wifi-based doorbell system. The system consists of an ESP32 and various connected hardware components like a doorbell, motor, camera, etc., a Flask HTTP server, and a Swift mobile app that the end users can use to approve unlock requests.

The basic workflow consists of:
1. The doorbell button is pressed
2. The ESP32 takes a picture of the guest and sends a POST request to the server
3. The server sends a push notification to the mobile app
4. The user can approve or deny the unlock request based on the picture taken by the ESP32
5. If approved, the server sends a request to the ESP32 to unlock the door

### System Design 

<img width="1090" alt="Screenshot 2024-12-08 at 10 45 18 PM" src="https://github.com/user-attachments/assets/ffb5055b-8537-4f0e-997b-65981d656684">

### Materials
- 15 KG motor (https://www.amazon.com/Servo-Torque-Steering-Waterproof-Profile/dp/B0D1G1KJ42/ref=sr_1_8?crid=38B8Y2UQDBGD&dib=eyJ2IjoiMSJ9.hXXGkJMSa2-I-RlO9tWZytHHAnPSqesOS62v-Hcx8ZV98xKLLn9iyuCQ1CVv4HkybAqsyUIehMK8JMqwN_dagDajiJh6tIy2rYCuU7xMiBWtMj0h1JhAcUR_YYwugsBgmV6xXyGwEDrIk9uwX37zkJWRH4zdLas8kI2K13G1o94aBmdz41p-we2931WG3SLJTvbZmuyIFN6uUvk5OJMyHvqufKSaPIQxwncHA7s70VoIQcg9Y6pSSfRsB6z8prnY3nPluCEz69kGUOup59FXeKChfQ-dSrmkX_ZTZKSSAXM.pVR1demiaySz7Wywnqo11MHBgnCXKU2ZaXbObj6TKL0&dib_tag=se&keywords=high+torque+motor+servo&qid=1729827956&s=industrial&sprefix=high+torque+motor+servo%2Cindustrial%2C99&sr=1-8)
- 9V battery (https://www.amazon.com/Energizer-Alkaline-General-Purpose-Battery/dp/B00003IE4E/ref=sr_1_9?crid=2MR21PDMVPL0S&dib=eyJ2IjoiMSJ9.UTIEEJxrXzAacW7rXzP5bfvEuJRS08CBgFJrkRhI2Xt-kWtDhsALiAGALXcQOUToD8lBxRbDL0OrmzhZSpgr6U-gRYK7-WjgiDMrFcuJCkcava1CetJTCzFpRB9j-pxE0GC4f0BVS9z0NJhPMSq_oYIoa03cLfEssVZElS_DIa2zsCaoCq9Yz13IJC6vg5ZoIAAUdJPtKHqtL_FI7wsAWHBLlqBTdK6fFPz35IJcMtweHzH6Y1d1z2xQmFhG4-XqLTWbWtA45LILJT4f1kL4lHONq5DbNDmhgTdTQEZ-UZA.AUK8vZhIqAuWbu2_it2xhwhbgU-cWInoXYVp_mabzMw&dib_tag=se&keywords=9v+battery&qid=1730600372&sprefix=9v+battery%2Caps%2C115&sr=8-9)
- 9V battery connector (https://www.amazon.com/LAMPVPATH-Pack-Battery-Holder-Switch/dp/B07T83B4SW?crid=1RD9L5WN9JTFZ&dib=eyJ2IjoiMSJ9.zRXEgi4yWfomn6nK7Ij-DEZehDPwSY8J7F5eegZQBsc8-fqi8-BOID_q_eCoycYolRTc6yY2SwoFxNVGqfwAybwiKeH3Xc9Y-teuMKjyzo_PXP_PIFNNPACk3GPRp_sGpKWJolGs2kLtPL4xJq6J0Nj_PSZIvLOn_upTGBJdX3p-RSyIfAQppNhFUSjAgJ7IW0dXnScYC0J9AArJrNR2vJXD3uY0zO6k3ImA31J-_Hs.uZmjvh_fpJ0pBRurz4wqBXdjsDZ5vGbbY2BuympkF30&dib_tag=se&keywords=9v+battery+box&qid=1730600299&sprefix=9v+battery+box%2Caps%2C101&sr=8-3)
- ESP32 S3

### Technologies

#### Frontend:
![Swift](https://img.shields.io/badge/swift-F54A2A?style=for-the-badge&logo=swift&logoColor=white)

#### Backend:
Firmware:
 ![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white) ![Espressif](https://img.shields.io/badge/espressif-E7352C.svg?style=for-the-badge&logo=espressif&logoColor=white) 

Server / App:
 ![Python](https://img.shields.io/badge/python-3670A0?style=for-the-badge&logo=python&logoColor=ffdd54) ![Flask](https://img.shields.io/badge/flask-%23000.svg?style=for-the-badge&logo=flask&logoColor=white) ![Vercel](https://img.shields.io/badge/vercel-%23000000.svg?style=for-the-badge&logo=vercel&logoColor=white) ![Postman](https://img.shields.io/badge/Postman-FF6C37?style=for-the-badge&logo=postman&logoColor=white)

### Future Plans / Development in Progress: 
- higher resolution camera
- ML model for visitor analytics and automatic facial recognition


