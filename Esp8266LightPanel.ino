#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "./index.h"
#include "/home/jsingh/private/wireless.h"

#define IP_ADDRESS_SUFFIX 202

const char* ssid = PRIVATE_SSID;
const char* password = PRIVATE_PSK;

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

const int relayPin = 0; // D3
const int powerButtonPin = 14; // D5
bool relayState = false;

const String setRelayState(bool newRelayState) {
	bool oldRelayState = relayState;
  relayState = newRelayState;
	const String output = relayState ? "Switch is on" : "Switch is off";
	if (oldRelayState != relayState) {
		digitalWrite(relayPin, relayState ? 1 : 0);
  	Serial.println(output);
	}
	return output;
}

int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void processPowerButton() {
	  // read the state of the switch into a local variable:
  int reading = digitalRead(powerButtonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        setRelayState(!relayState);
			  Serial.println(F("Toggled LED."));
      }
    }
  }
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}

void toggleRelay() {
	const String output = setRelayState(!relayState);
  server.send(200, "text/plain", output);
}
void turnRelayOn() {
	const String output = setRelayState(true);
  server.send(200, "text/plain", output);
}
void turnRelayOff() {
	const String output = setRelayState(false);
  server.send(200, "text/plain", output);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  pinMode(relayPin, OUTPUT);
  pinMode(powerButtonPin, INPUT);
  setRelayState(false);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  #ifdef IP_ADDRESS_SUFFIX
	  IPAddress ip(10, 2, 1, IP_ADDRESS_SUFFIX);
		IPAddress gateway(10, 2, 1, 1);
		IPAddress subnet(255, 255, 255, 0);
		IPAddress dns(10, 2, 1, 1);
		WiFi.config(ip, dns, gateway, subnet);
	#endif
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/toggle-relay", toggleRelay);
  server.on("/relay/on", turnRelayOn);
  server.on("/relay/off", turnRelayOff);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  setRelayState(true);
  delay(2000);
  setRelayState(false);
}

void loop(void) {
  server.handleClient();
  processPowerButton();
}
