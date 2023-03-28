# RU Intro Embedded Systems Libraries

## UART to MQTT Bridge

MSP430FR2355 UART will be done over EUSCI A1 Peripheral.

### ESP8266 Command Messages

#### & - System Status
"& W": Wifi Status
Returns a "@ 1" if connected to WiFi and "@ 0" if not

"& B": Broker Status
Returns a "@ 1" if connected to Broker and "@ 0" if not

#### Subscribe to Topic
"$ topicName" - Subscribe to the topic as given in topicName
Response:
- "@ 1" no errors
- "@ 0" Error Occurred

#### Unsubscribe from Topic
"! topicName" - Unsubscribe from the topic
Response:
- "@ 1" no errors
- "@ 0" Error Occurred

#### Publish Message
"# topicName Data" - ESP8266 will publish the data to the specified topic.
Response:
- "@ 1" no errors
- "@ 0" Error Occurred

#### Received Message
"~ topicName payload" - ESP8266 will send to the host MCU data when received on a subscribed topic.
