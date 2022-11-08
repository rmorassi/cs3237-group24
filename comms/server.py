import base64 as b64
import paho.mqtt.client as mqtt
import subprocess
import time
import ai

IP = '192.168.200.92'
PORT = 1883
last_time = 0
NOTIFICATION_INTERVAL = 500
NOTIFICATION_TOPIC = 'notifications/'

sess, args, feed_dict, model = ai.setup()

# TODO:
#   - Add rooms/namespaces - rooms probably preferred
#   - Alternatively use the session IDs from clients ??
#   - Security? i.e require user/password? - probably just mention in report
#   - Use image compression for data sent?


def setup():
    # command = 'mosquitto -c /etc/mosquitto/mosquitto.conf'
    # process = subprocess.Popen(command.split(), stdout=subprocess.PIPE)
    # output, error = process.communicate()
    client = mqtt.Client()
    client.on_connect = on_connect
    client.connect(IP, PORT, 60)
    return client

def on_connect(client, userdata, flags, rc):
    print("Client connected with result code: " + str(rc))
    client.subscribe('image/#')
    client.message_callback_add('image/#', process_image)
    client.on_message = on_message

def echo(client, userdata, msg):
    print("Received: " + msg.payload.decode('utf-8'))

def process_image(client, userdata, message):
    """Image format should be base64 string."""
    print('received image')
    # if other data is eventually needed then this can be reformatted into
    # JSON
    curr_time = round(time.time() * 1000)
    global last_time
    if (curr_time - last_time > NOTIFICATION_INTERVAL):
        print('sending image to phone')
        last_time = curr_time
        client.publish(NOTIFICATION_TOPIC, message.payload.decode('utf-8'))
    
    img = ai.b64ToOCV2Image(message.payload.decode('utf-8'))
    dir = ai.find_loc(img, sess, args, feed_dict, model)
    # Emit the labelled image to a phone app? - Might be useful for debugging
    # Image compression here??
    if dir is None:
        client.publish('car/direction', 0)
    else:
        client.publish('car/direction', round(dir, 2))
        time.sleep(0.2)
        client.publish('car/direction', 0)

def on_message(client, userdata, msg):
    print(msg.topic + " " + msg.payload.decode('ascii'))

def main():
    client = setup()
    client.loop_forever()

if __name__ == '__main__':
    main()
