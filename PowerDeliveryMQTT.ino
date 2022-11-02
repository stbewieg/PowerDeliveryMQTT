/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board:
  ----> https://www.adafruit.com/product/2471

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <UIPEthernet.h>

#define MQTT_DEBUG
#define MAXSUBSCRIPTIONS 1
#define SUBSCRIPTIONDATALEN 15
#define TOPICLEN 14

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


/************************* Adafruit.io Setup *********************************/


#define halt(s) \
  { \
    Serial.println(F(s)); \
    resetFunc(); \
  }

// ---------> Constants and Static Vars <-------------
// > Constants Makros

#define SERVER "192.168.178.1"
#define SERVERPORT 1883
#define USERNAME "public"
#define KEY "public"
// > Constants Stored in Flash memory

const byte mac[] PROGMEM = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };



// > Constants Stored in SRAM
const char PDCTopicWild[] = "cube00/PDC/#";
const char publishInit[] = "ON";
const char willMsg[] = "OFF";




// > Statics
char PDCTopic[] = "cube00/PDC";
EthernetClient client;
Adafruit_MQTT_Client mqtt(&client, SERVER, SERVERPORT);//, USERNAME, USERNAME, KEY);



Adafruit_MQTT_Subscribe PDCSubTopic =
  Adafruit_MQTT_Subscribe(&mqtt, PDCTopicWild);


/*************************** Sketch Code ************************************/

void (*_resetFunc)(void) = 0;
void resetFunc(void) {
  Serial.println(F("\nRestart in 15 sec\n"));
  delay(15000);
  _resetFunc();
}


void setup() {
  Serial.begin(115200);
  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  // Initialise the Client
  Serial.println(F("\nInit the Client..."));
  Ethernet.begin(mac);
  delay(1000);  //give the ethernet a second to initialize

  Serial.print(F("IP: "));
  Serial.println(Ethernet.localIP());

  // configure MQTT
  mqtt.will(PDCTopic, willMsg, 0, 1);
  mqtt.subscribe(&PDCSubTopic);
  //mqtt.setKeepAliveInterval((uint16_t)10);// second


  //connect
  MQTT_connect();
}


void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets and callback em' busy subloop
  // try to spend your time here:
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(1000))) {
    if (subscription == &PDCSubTopic) {
      Serial.print(F("Got: "));
      Serial.println((char *)PDCSubTopic.lastread);
      Serial.print(F("In: "));
      Serial.println((char *)PDCSubTopic.lastreadTopic);
    }
  }
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds

  if (!mqtt.ping()) {
    mqtt.disconnect();
  }
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print(F("Connecting to MQTT... "));

  while ((ret = mqtt.connect()) != 0) {  // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.print(F("Code:\t"));
    Serial.println(ret);
    Serial.println(F("Retrying MQTT connection in 5 seconds..."));
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
  }
  mqtt.publish(PDCTopic, publishInit, 0);

  Serial.println(F("MQTT Connected!"));
}