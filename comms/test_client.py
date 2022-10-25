import base64 as b64
import paho.mqtt.client as mqtt

# This file is outdated and needs to be updated to use MQTT

# Change this to the address that appears on start up of the server
# the 127 address is a loopback so if connecting from a separate device choose
# a different one.
IP = '192.168.54.92'
PORT = '5000'

def setup():
    return mqtt.Client()

def main():
    client = setup()
    # loop load image

    client.publish("image", img_str)
