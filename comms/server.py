from flask import Flask
from flask_socketio import SocketIO, emit, send

app = Flask(__name__)
app.config['SECRET_KEY'] = 'group24'    # change this to something else later
socketio = SocketIO(app)

@socketio.on('connect')
def hello():
    print("Client connected!")
    return 'Hello, World!'

@socketio.on('message')
def echo(message: str):
    print(f"client sent message: {message}")
    send(message)

@socketio.on('*')
def default_handler(event, data):
    print(f"Received: {data} from {event}")

if __name__ == '__main__':
    socketio.run(app)