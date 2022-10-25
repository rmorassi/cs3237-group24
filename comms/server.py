from flask import Flask
from flask_socketio import SocketIO, send

# This will run on all addresses on your local machine.
# Can change this to a specific one by checking what is available by entering
# the ipconfig command in Windows Command Prompt.
HOST = '0.0.0.0'

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
    socketio.run(app, host=HOST)