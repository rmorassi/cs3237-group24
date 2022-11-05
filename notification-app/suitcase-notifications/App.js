import 'react-native-gesture-handler';
import { NavigationContainer } from '@react-navigation/native';
import { createStackNavigator } from '@react-navigation/stack';
import StatusScreen from './src/screens/StatusScreen';
import PicturesScreen from './src/screens/PicturesScreen';

const Stack = createStackNavigator();

export default function App() {
  return (
    <NavigationContainer>
      <MainStack />
    </NavigationContainer>
  );
}

function MainStack() {
  return (
    <Stack.Navigator screenOptions={{ headerShown: false }}>
      <Stack.Screen name='StatusScreen' component={StatusScreen} />
      <Stack.Screen name='PicturesScreen' component={PicturesScreen} />
    </Stack.Navigator>
  );
}
