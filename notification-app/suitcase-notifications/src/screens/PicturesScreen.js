import { StyleSheet, Text, StatusBar, SafeAreaView } from 'react-native';
import React from 'react';

export default function PicturesScreen() {
  return (
    <SafeAreaView style={styles.mainContainer}>
      <StatusBar style='auto' />
      <Text>PicturesScreen</Text>
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  mainContainer: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
    paddingTop: Platform.OS === 'android' ? StatusBar.currentHeight : 0,
  },
});
