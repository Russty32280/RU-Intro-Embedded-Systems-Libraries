/*
 * UART to MQTT Message Bridge
 * Author: Russell Trafford
 * Version 1.0
 * Updated: 05DEC22
 * 
 * ALL MESSAGE MUST END WITH A "\n" (newline) Character! If it does not, your message will not be read
 * 
 * When system is powered on, you can query the status of the system using the & Command
 * & - System Status
 * "& W" - Wifi Status returns a "@ 1" if connected to wifi and "@ 0" if not
 * "& B" - Broker Status returns a "@ 1" if the system is connected to the MQTT broker and "@ 0" if not
 * 
 * To subscribe to a topic, utilize the $ command
 * "$ topicName" - Subscribe to the topic as given in topicName
 * Response:
 *  "@ 1" - If no errors
 *  "@ 0" - Error Occurred
 * 
 * To unsubscribe from a topic
 * "! topicName" - Unsubscribe from topicName.
 * Repsonse:
 *  "@ 1" - If no errors
 *  "@ 0" - Error Occurred
 * 
 * Published messages will be sent to the host device using the # command with the following structure:
 * "# topicName Data" - Data recieved on topicName. Note the use of the "\" which will be needed to parse in the host device.
 * Response:
 *  "@ 1" - If no errors
 *  "@ 0" - Error Occured
 * 
 * 
 * Recieved Messages will arrive over the UART port as a "~" command
 * "~ topicName payload" - The command, topic, and payload are all space deliminated
 * 
 * 
 * All messages will respond using the "@" character as an alert from the wifi module. The exception
 * is the received message which has its own command character.
 * 
 * 
 * Note: There will be a perfomance issue with running the client.loop while waiting for the serial
 * to complete. It is recommended that further work attaches the code to a timer to check the loop
 * in the background, using something like this: https://circuits4you.com/2018/01/02/esp8266-timer-ticker-example/
 * 
 *
 */




#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "RowanWiFi"; // Enter your WiFi name
const char *password = "";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "public.cloud.shiftr.io";
const char *arrivalTopic = "ECE09342/NewDevices";
const char *mqtt_username = "public";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

const char *groupName = "Group1";

WiFiClient espClient;
PubSubClient client(espClient);


// Global System Flags
int wifiStatus = 0; // 0 - Not Connected, 1 - Connected
int mqttStatus = 0; // 0 - Not Connected, 1 - Connected



void setup() {
  // Set software serial baud to 115200;
  Serial.begin(9600);
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print("0");
      wifiStatus = 0; 
  }
  wifiStatus = 1; //  Set global flag for status commands
  Serial.println("1");
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          mqttStatus = 1;
      } else {
          mqttStatus = 0;
      }
  } 
  client.publish(arrivalTopic, groupName);
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("~ ");
  Serial.print(topic);
  Serial.print(" ");
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
  
}


void loop() {
  client.loop();
  String userInString = Serial.readStringUntil('\n');
  int str_len = userInString.length() + 1;
  char userIn_array[str_len];
  userInString.toCharArray(userIn_array, str_len);
  char *deliminator = " ";
  if (userIn_array[0] != NULL){
    char *command;
    command = strtok( userIn_array, deliminator); // Grab the first character to determine function 
    
    // System Command
    if (*command == '&'){
      char *systemMsg;
      systemMsg = strtok( NULL, " ");
      if (*systemMsg == 'W'){
        Serial.print("@ ");
        Serial.println(wifiStatus);
      }
      else if (*systemMsg == 'B'){
        Serial.print("@ ");
        Serial.println(mqttStatus);
      }
    }

    // Subscribe to topic
    else if (*command == '$'){
      char *subscribeTopic = strtok( NULL, " ");
      client.subscribe(subscribeTopic);
      Serial.println("@ 1");
    }

    // Unsubscribe from a topic
    else if (*command == '!'){
      char *subscribeTopic = strtok( NULL, " ");
      client.unsubscribe(subscribeTopic);
      Serial.println("@ 1");
    }

    
    // Publish to Topic
    else if (*command == '#'){
      char *publishTopic = strtok( NULL, " ");
      char *msgBody = strtok( NULL, " ");
      client.publish(publishTopic, msgBody);
      Serial.println("@ 1");
    }
    
  }
    
}

  
