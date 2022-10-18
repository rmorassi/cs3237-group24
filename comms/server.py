import base64 as b64
import paho.mqtt.client as mqtt


# This will run on all addresses on your local machine.
# Can change this to a specific one by checking what is available by entering
# the ipconfig command in Windows Command Prompt.
IP = '192.168.54.92'
PORT = 1883

# TODO:
#   - Add rooms/namespaces - rooms probably preferred
#   - Alternatively use the session IDs from clients ??
#   - Find out format needed for image processing model and decode from base64
#   - Security? i.e require user/password? - probably just mention in report
#   - Use image compression for data sent?

def setup():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.message_callback_add("Tembusu/#", echo)
    client.connect(IP, 1883, 60)
    client.publish("UTR", "Hi, it's Tariq!")
    client.subscribe("Tembusu/#")
    return client

def on_connect(client, userdata, flags, rc):
    print("Client connected with result code: " + str(rc))

def echo(client, userdata, msg):
    print("Received: " + msg.payload.decode('utf-8'))

"""Image format should be base64 string."""
def process_image(image : str):
    # if other data is eventually needed then this can be reformatted into
    # JSON
    img_data = b64.b64decode(image)
    with open("test_image.jpg", "wb") as img:
        img.write(img_data)
    
    # Process the image using our model
    # Options can be L, R, F, B, S, N ?
    # Left, Right, Forward, Backward, Stop, Not Found
    direction = 'L' 
    # Emit the labelled image to a phone app? - Might be useful for debugging
    # Image compression here??

def on_message(client, userdata, msg):
    print(msg.topic + " " + msg.payload.decode('ascii'))

if __name__ == '__main__':
    client = setup()
    client.loop_forever()
    

