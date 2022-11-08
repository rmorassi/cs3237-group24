import Paho from 'paho-mqtt';
import { useState, useEffect } from 'react';
import { StatusBar } from 'expo-status-bar';
import { Button, StyleSheet, Text, View, TextInput } from 'react-native';
import { ImageGallery } from '@georstat/react-native-image-gallery';

const IP = '172.27.210.101'; // Update this
const PORT = 9001; // Check this
const SUB_TOPICS = ['notifications'];
const CLIENT_ID = 'notification-app';
const MAX_IMAGES = 8;

export default function StatusScreen() {
  const [ownerIsFound, setOwnerIsFound] = useState(false);
  const [isConnected, setIsConnected] = useState(false);
  const [galleryIsOpen, setGalleryIsOpen] = useState(false);
  const [images, setImages] = useState([]);
  const [ip, setIp] = useState('192.168.123.456');
  const [port, setPort] = useState('9001');

  client = new Paho.Client(ip, parseInt(port), CLIENT_ID);

  const onMessage = (message) => {
    if (message.destinationName === 'notifications') {
      console.log(`Image received`);
      try {
        setImages((prevImages) => {
          let newImages = prevImages;
          if (prevImages.length >= MAX_IMAGES) {
            newImages.shift();
          }
          newImages.push({
            url: 'data:image/jpeg;base64,' + message.payloadString,
          });
          console.log(`I have ${newImages.length} images.`);
          return newImages;
        });
      } catch (e) {
        console.log(e);
      }
      // client.publish('ack', 'message received!');
    }
  };

  const onConnect = () => {
    console.log(`Connected to ${ip}:${port} as ${CLIENT_ID}`);
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
  };

  const connect = () => {
    client.connect({
      onSuccess: onConnect,
      onFailure: () => {
        console.log(`Failed to connect to ${ip}:${port} as ${CLIENT_ID}`);
        setIsConnected(false);
      },
    });
  };

  return (
    <View style={styles.container}>
      <View style={[styles.rowContainer, { justifyContent: 'space-between' }]}>
        <Text style={styles.text}>IP </Text>
        <TextInput
          placeholder='192.168.123.456'
          value={ip}
          onChangeText={setIp}
          style={styles.textInput}
          keyboardType='numeric'
        />
      </View>
      <View style={[styles.rowContainer, { justifyContent: 'space-between' }]}>
        <Text style={styles.text}>Port </Text>
        <TextInput
          placeholder='9001'
          value={port}
          onChangeText={setPort}
          keyboardType='numeric'
          style={styles.textInput}
        />
      </View>
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
      <View style={styles.rowContainer}>
        <View
          style={[
            styles.rowContainer,
            { flex: 1, justifyContent: 'space-around' },
          ]}
        >
          <Button title='Pictures' onPress={() => setGalleryIsOpen(true)} />
          <Button title='Connect!' onPress={() => connect()} />
        </View>
      </View>
      <ImageGallery
        isOpen={galleryIsOpen}
        close={() => setGalleryIsOpen(false)}
        images={images}
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
    paddingVertical: 5,
    width: '80%',
    justifyContent: 'center',
  },
  text: {
    fontSize: 20,
    fontWeight: 'bold',
  },
  textInput: {
    fontSize: 20,
    borderWidth: 1,
    width: '85%',
  },
});
