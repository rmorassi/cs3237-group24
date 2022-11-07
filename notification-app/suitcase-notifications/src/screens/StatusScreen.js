import Paho from 'paho-mqtt';
import { useState, useEffect, useRef } from 'react';
import { StatusBar } from 'expo-status-bar';
import { Button, StyleSheet, Text, View } from 'react-native';
import { ImageGallery } from '@georstat/react-native-image-gallery';
import { imageString } from '../../test_image/test_image';

const IP = '172.27.210.101'; // Update this
const PORT = 9001; // Check this
const SUB_TOPICS = ['notifications'];
const CLIENT_ID = 'notification-app';
const MAX_IMAGES = 8;

client = new Paho.Client(IP, PORT, CLIENT_ID);

export default function StatusScreen({ navigation }) {
  const [ownerIsFound, setOwnerIsFound] = useState(false);
  const [isConnected, setIsConnected] = useState(false);
  const [galleryIsOpen, setGalleryIsOpen] = useState(false);
  const [images, setImages] = useState([]);

  useEffect(() => {
    connect();
  }, []);

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
      <Button title='Pictures' onPress={() => setGalleryIsOpen(true)} />
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
  },
  text: {
    fontSize: 20,
    fontWeight: 'bold',
  },
});
