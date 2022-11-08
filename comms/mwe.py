import paho.mqtt.client as mqtt

IP = '192.168.200.92'
PORT = 1883

def setup():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(IP, PORT, 60)
    return client

def on_connect(client, userdata, flags, rc):
    print("Client connected with result code: " + str(rc))
    client.subscribe('image/#')

def on_message(client, userdata, msg):
    print('received image')

def main():
    client = setup()
    client.loop_forever()

if __name__ == '__main__':
    main()