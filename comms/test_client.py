import base64 as b64
import paho.mqtt.client as mqtt
import time

# This file is outdated and needs to be updated to use MQTT

# Change this to the address that appears on start up of the server
# the 127 address is a loopback so if connecting from a separate device choose
# a different one.
IP = '192.168.86.92'
PORT = 1883
CLIENT_ID = 'test'
DELAY = 2

def setup():
    client = mqtt.Client(CLIENT_ID)
    client.on_connect = on_connect
    client.on_message = print_message
    client.connect(IP, 1883, 60)
    return client

def send_directions(client):
    # Send some directions here
    # client.publish('car/control', 'start')
    # client.publish('car/speed', '1.0')
    # time.sleep(1)
    client.publish('car/direction', '-1.0')   # turn left
    print("dir 1")
    time.sleep(DELAY)
    client.publish('car/direction', '1.0')    # turn right
    print("dir 2")
    time.sleep(DELAY)
    client.publish('car/control', 'stop')
    print("dir 3")

def on_connect(client, userdata, flags, rc):
    print("Client connected with result code: " + str(rc))


def print_message(message: str):
    print(f"Received message: {message}")

def main():
    client = setup()
    # for i in range(5):
    #     client.publish('arduino/simple', 'hello Roberto!')
    #     time.sleep(2)
    while True:
        speed = 60
        print(speed)
        client.publish('car/control', 'start')
        client.publish('car/speed', str(speed / 100.0))
        time.sleep(1)
        send_directions(client)
        # send_directions(client)
        # time.sleep(DELAY)
        # client.publish('car/direction', '-1.0')
        # print("dir 4")
        # time.sleep(DELAY)
        print('Finished!')



if __name__ == '__main__':
    main()
