import Paho from 'paho-mqtt';
import { useState, useEffect } from 'react';
import { StatusBar } from 'expo-status-bar';
import { StyleSheet, Text, View } from 'react-native';

const IP = '172.31.215.122'; // Update this
const PORT = 9001; // Check this

client = new Paho.Client(IP, PORT, 'notification-app');

export default function App() {
  const [notification, setNotification] = useState('Owner not found!');

  useEffect(() => {
    client.connect({
      onSuccess: () => {
        console.log('Connected!');
        client.subscribe('notifications', {
          onSuccess: () => console.log('subscribed'),
        });
        client.onMessageArrived = onMessage;
        // client.onMessageDelivered = () => console.log('Hi');
        client.onConnectionLost = () => console.log('lost connection');
      },
      onFailure: () => {
        console.log('Failed to connect!');
      },
    });
  }, []);

  const onMessage = (message) => {
    // if (message.destinationName === 'notifications') {
    console.log(`Message received: ${message.payloadString}`);
    client.publish('ack', 'message received!');
    // }
  };

  return (
    <View style={styles.container}>
      <Text>{notification}</Text>
      <StatusBar style='auto' />
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#fff',
    alignItems: 'center',
    justifyContent: 'center',
  },
});
