import socketio

# Change this to the address that appears on start up of the server
# the 127 address is a loopback so if connecting from a separate device choose
# a different one.
IP = '10.14.0.2'
PORT = '5000'

sio = socketio.Client()

@sio.on('message')
def print_message(message: str):
    print(f"Received message: {message}")

if __name__ == '__main__':
    sio.connect(f'http://{IP}:{PORT}')
    sio.emit('message', 'Hi, this is my message!')