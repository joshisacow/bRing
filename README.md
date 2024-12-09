# bRing
ECE 655 Final Project - Josh, Bryant, Emily

This is the repository for the bRing project. It is an end-to-end remote wifi-based doorbell system. The system consists of an ESP32 and various connected hardware components like a doorbell, motor, camera, etc., a Flask HTTP server, and a Swift mobile app that the end users can use to approve unlock requests. The doorbell button is connected to the Raspberry Pi, which is connected to the web server. 

The basic workflow consists of:
1. The doorbell button is pressed
2. The ESP32 takes a picture of the guest and sends a POST request to the server
3. The server sends a push notification to the mobile app
4. The user can approve or deny the unlock request based on the picture taken by the ESP32
5. If approved, the server sends a request to the ESP32 to unlock the door


