void handleRoot() {
  digitalWrite(statusLed, 1);
  
  if (server.hasArg("LED")) {
    handleSubmit();
  } else {
    server.send(200, "text/html", INDEX_HTML);
  }
  
  digitalWrite(statusLed, 0);
}

void handleSubmit() {
  String LEDvalue;

  if (!server.hasArg("LED")) return returnFail("BAD ARGS");
  
  LEDvalue = server.arg("LED");
  
  if (LEDvalue != "1" && LEDvalue != "0") {
    returnFail("Bad LED value");
  }
  
  if (LEDvalue == "1" && lastLightState != HIGH) {
    writeLED(HIGH);
    server.send(200, "text/html", INDEX_HTML);
  } else if (LEDvalue == "0" && lastLightState != LOW) {
    writeLED(LOW);
    server.send(200, "text/html", INDEX_HTML);
  }
  
  // refresh the page, but don't change the light 
  server.send(200, "text/html", INDEX_HTML);
}

void returnFail(String msg) {
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(500, "text/plain", msg + "\r\n");
}

void returnOK() {
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "OK\r\n");
}

void writeLED(bool LEDon) {
  // Note inverted logic for Adafruit HUZZAH board
  if (LEDon) {
    lightState = HIGH;
  } else {
    lightState = LOW;
  }
      
  if (lightState != lastLightState) {
    if (! light.publish(lightState)) {
      Serial.println(F(" Failed"));
    } else {
      Serial.println(F(" OK!"));
    }
  }

  
  digitalWrite(stateLed, lightState);
}

void handleNotFound(){
  digitalWrite(statusLed, 1);
  
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  
  server.send(404, "text/plain", message);
  digitalWrite(statusLed, 0);
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
