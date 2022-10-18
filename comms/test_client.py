import socketio
import base64 as b64

# Change this to the address that appears on start up of the server
# the 127 address is a loopback so if connecting from a separate device choose
# a different one.
IP = '192.168.54.92'
PORT = '5000'

with open("/mnt/c/Users/mrx20/Downloads/animal_shoes.jpeg", "rb") as image_file:
    TEST_IMAGE = b64.b64encode(image_file.read())

sio = socketio.Client()

@sio.on('message')
def print_message(message: str):
    print(f"Received message: {message}")

if __name__ == '__main__':
    sio.connect(f'http://{IP}:{PORT}')
    sio.emit('message', 'Hi, this is my message!')
    sio.emit('image', TEST_IMAGE)