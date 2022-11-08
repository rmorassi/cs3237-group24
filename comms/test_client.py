import base64 as b64
import paho.mqtt.client as mqtt
import time

# This file is outdated and needs to be updated to use MQTT

# Change this to the address that appears on start up of the server
# the 127 address is a loopback so if connecting from a separate device choose
# a different one.
IP = '172.27.210.101'
PORT = 1883
PUB_TOPIC = 'notifications'

def setup():
    return mqtt.Client()

def main():
    client = setup()
    for i in range(12):
        send_image(client)
        time.sleep(3)
    
    print("Done!")

def setup():
    client = mqtt.Client()
    client.connect(IP, 1883, 60)
    time.sleep(2)
    return client

def send_image(client):
    client.publish(PUB_TOPIC, TEST_IMAGE.decode('utf-8'))
    print(TEST_IMAGE.decode('utf-8'))
    print('\n\n\n\n')

def print_message(message: str):
    print(f"Received message: {message}")

if __name__ == '__main__':
    main()
