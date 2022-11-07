import base64 as b64
import paho.mqtt.client as mqtt
import numpy as np
import cv2
import ai

# This will run on all addresses on your local machine.
# Can change this to a specific one by checking what is available by entering
# the ipconfig command in Windows Command Prompt.
IP = '192.168.86.92'
PORT = 1883

sess, args, feed_dict = ai.setup()

# TODO:
#   - Add rooms/namespaces - rooms probably preferred
#   - Alternatively use the session IDs from clients ??
#   - Security? i.e require user/password? - probably just mention in report
#   - Use image compression for data sent?


def setup():
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
    # if other data is eventually needed then this can be reformatted into
    # JSON
    # print(f'{message.payload.decode("utf-8")}')

    img_bytes = b64.b64decode(message.payload.decode('utf-8')) + b'=='  # honestly wtf is this
    im_arr = np.frombuffer(img_bytes, dtype=np.uint8)  # im_arr is one-dim Numpy array
    img = cv2.imdecode(im_arr, flags=cv2.IMREAD_COLOR)
    # with open("test_image.jpg", "wb") as img:
    #     img.write(img_data)

    dir = ai.classify(img, sess, args, feed_dict)

    # Emit the labelled image to a phone app? - Might be useful for debugging
    # Image compression here??
    if dir is None:
        client.publish('car/direction', -1)
    else:
        client.publish('car/direction', round(dir, 2))


def on_message(client, userdata, msg):
    print(msg.topic + " " + msg.payload.decode('ascii'))

def main():
    client = setup()
    client.loop_forever()

if __name__ == '__main__':
    main()
