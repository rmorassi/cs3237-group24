import Paho from 'paho-mqtt';
import { useState, useEffect } from 'react';
import { StatusBar } from 'expo-status-bar';
import { Button, StyleSheet, Text, View } from 'react-native';

const IP = '172.27.210.101'; // Update this
const PORT = 9001; // Check this
const SUB_TOPICS = ['notifications'];
const CLIENT_ID = 'notification-app';

client = new Paho.Client(IP, PORT, CLIENT_ID);

export default function StatusScreen({ navigation }) {
  const [ownerIsFound, setOwnerIsFound] = useState(false);
  const [isConnected, setIsConnected] = useState(false);

  useEffect(() => {
    connect();
  }, []);

  const onMessage = (message) => {
    // if (message.destinationName === 'notifications') {
    console.log(`Message received: ${message.payloadString}`);
    client.publish('ack', 'message received!');
    // }
  };

  const onConnect = () => {
    console.log(`Connected to ${IP}:${PORT} as ${CLIENT_ID}`);
    setIsConnected(true);
    SUB_TOPICS.forEach((topic) => {
      client.subscribe(topic, {
        onSuccess: () => console.log(`Subscribed to ${topic}`),
        onFailure: () => console.log(`Failed to subscribe to ${topic}`),
      });
    });
    client.onMessageArrived = onMessage;
    client.onConnectionLost = onConnectionLost;
  };

  const onConnectionLost = () => {
    setIsConnected(false);
    console.log('Lost connection...');
    console.log(`Attempting to reconnect to ${IP}:${PORT} as ${CLIENT_ID}`);
    connect();
  };

  const connect = () => {
    client.connect({
      onSuccess: onConnect,
      onFailure: () => {
        console.log(`Failed to connect to ${IP}:${PORT} as ${CLIENT_ID}`);
        console.log('Retrying...');
        connect();
      },
    });
  };

  return (
    <View style={styles.container}>
      <View style={styles.rowContainer}>
        <Text style={styles.text}>Connection Status: </Text>
        <Text style={[styles.text, { color: isConnected ? 'green' : 'red' }]}>
          {isConnected ? 'Connected!' : 'Not Connected!'}
        </Text>
      </View>
      <View style={styles.rowContainer}>
        <Text style={styles.text}>Owner </Text>
        <Text style={[styles.text, { color: ownerIsFound ? 'green' : 'red' }]}>
          {ownerIsFound ? 'Found!' : 'Not Found!'}
        </Text>
      </View>
      <StatusBar style='auto' />
      <Button
        title='Pictures'
        onPress={() => navigation.navigate('PicturesScreen')}
      />
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
  rowContainer: {
    flexDirection: 'row',
  },
  text: {
    fontSize: 20,
    fontWeight: 'bold',
  },
});
