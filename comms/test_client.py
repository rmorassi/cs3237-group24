import socketio

IP = '127.0.0.1'
PORT = '5000'

sio = socketio.Client()

@sio.on('message')
def print_message(message: str):
    print(f"Received message: {message}")

if __name__ == '__main__':
    sio.connect(f'http://{IP}:{PORT}')
    sio.emit('message', 'Hi, this is my message!')