#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

ESP8266WebServer server(80);

//Status indicator
const int statusLed = 13;

//State indicator
const int stateLed = 15;

//State indicator
const int buttonPin = 12;

bool lightState = LOW;
bool lastButtonState = LOW; 
bool lastLightState = LOW; 

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "ryanparsley"
#define AIO_KEY         "95f1b2a3e0e64f16a9ac99023220fc08"

WiFiClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feeds called 'light' and 'button' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>

String feed = String(AIO_USERNAME) + String("/feeds/dingus-") + String(ESP.getChipId());
Adafruit_MQTT_Publish light = Adafruit_MQTT_Publish(&mqtt, feed.c_str());
Adafruit_MQTT_Publish button = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/button");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");

void MQTT_connect();

void setup(void){
  
  pinMode(statusLed, OUTPUT);
  pinMode(stateLed, OUTPUT);    // Use Built-In LED for Indication
  
  /* INPUT_PULLUP enables the Arduino Internal Pull-Up Resistor */
  pinMode(buttonPin, INPUT_PULLUP); 
  
  Serial.begin(115200);
  WiFiManager wifiManager;

  Serial.println("chipID: " + String(ESP.getChipId()));
  
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  
  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&onoffbutton);
  // Turn off the light and publish that when the dingus is plugged in.
  MQTT_connect();
  light.publish(LOW);
  if (! light.publish(lightState)) {
    Serial.println(F(" Failed to initialize to low"));
  } else {
    Serial.println(F(" OK! we should have reset the light status to low on startup"));
  }
}

void loop(void){
  MQTT_connect();
  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);
    }
  }
   
  bool buttonState = digitalRead(buttonPin);  // store current state of pin 12
  
  if (buttonState != lastButtonState) {
    Serial.print(F("\nBottonState has changed. "));
    
    if (buttonState == LOW) {
      Serial.print("Button was pressed, toggle light.");
      lightState = !lightState;
    }
    
    // Now we can publish stuff!
    Serial.print(F("\nSending button state val "));
    Serial.print(buttonState);
    Serial.print("...");
    Serial.print(F("\nSending light state val "));
    Serial.print(lightState);
    
    if (! button.publish(buttonState)) {
      Serial.println(F(" Failed"));
    } else {
      Serial.println(F(" OK!"));
    }

    delay(50);

  }
  writeLED(lightState);
    
  lastButtonState = buttonState;
  lastLightState = lightState;
  
  server.handleClient();
}
