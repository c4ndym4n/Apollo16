// Libraries
#include <Arduino.h>
#include <Hash.h>

#include "ESP8266WiFi.h"
#include <ESP8266WiFiMulti.h>
#include <ESP8266httpUpdate.h>

#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
//#include <aREST.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


/************************* PIN Setup *********************************/

// Pin Setup
//const int buttonPin = 2;  

#define PIN_BUTTON    15

#define PIN_RED       14
#define PIN_GREEN     12
#define PIN_BLUE      13
#define PIN_ROTOR     4


/************************* Firmware *********************************/

#define FIRMWARE_VERSION  8
#define USE_SERIAL Serial
ESP8266WiFiMulti WiFiMulti;

/************************* WiFi Access Point (Client) *********************************/

#define WLAN_SSID       "********"
#define WLAN_PASS       "********"

String WLAN_SSID_str;
String WLAN_PASS_str;



///************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  8883                   // TLS1.2
#define AIO_USERNAME    "*******"
#define AIO_KEY         "**************************"


/**************************************** Global State MQTT **********************************************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
//WiFiClient client;
// or... use WiFiFlientSecure for TLS1.2
WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);


/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/photocell");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");
Adafruit_MQTT_Subscribe firmware_update = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/firmware");


/************************* WiFi Access Point (AP) *********************************/

int AccessPointActivation = 10;         // In seconds; How many sec. hold button to activate AP_MODE
int AccessPointLifetime =   180;        // In seconds

int firmwareActivation =    6;          // Seconds to hold for firmware updates
int deactivation =          15;         // Hold .. seconds to deactivate the device

/************************* Rest API *********************************/

// Create aREST instance
aREST rest = aREST();

// Variables to be exposed to the API
String UID;
String KEY;


/************************* Mode States *********************************/

int modeState = 0;      // Start with Mode OFF


/************************* Led/Button Setup *********************************/

int ledState = 3;        // current state of the Led
int rotorState;

int buttonState = 0;     // current state of the button
int lastButtonState = 0; // previous state of the button
int startPressed = 0;    // the time button was pressed
int endPressed = 0;      // the time button was released
int timeHold = 0;        // the time button is hold
int timeReleased = 0;    // the time button is released

boolean pressed = false;
int seconds_true = 0;


/************************* Function Declarations *********************************/


void MQTT_connect();

void buttonAction();
String HashText();
void StateOnOff();
void StateFirmwareUpdate();
void StateAccessPoint();
void exposeVariablesRestService();
void SetupServer();
void LoopRestServer();
void FirmUpdater();

int macAdrs(String mac);
int routerSsid(String ssid);
int routerPass(String passwd);
int keyExchange(String key);


/************************* Setup *********************************/

void setup() {

  // initialize the button pin as a input:
  pinMode(PIN_BUTTON, INPUT);

  pinMode(PIN_ROTOR, OUTPUT);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  
  // Connect Serial
  Serial.begin(115200);

  /************************* Api Related *********************************/
  
  UID = "UID123434523423";
  KEY = "KEY2384yhfuw8sdfsd8fdsufsdijfwiu9fwusdfh";
    
  rest.variable("UID", &UID);
  rest.variable("key", &KEY);    

  /************************* Route Funcs *********************************/

  // Function to be exposed
  rest.function("mac", macAdrs);
  rest.function("ssid", routerSsid);
  rest.function("passwd", routerPass);

  /************************* ESP8266 ID Rest API *********************************/
  
  // Give name & ID to the device (ID should be 6 characters long)
  rest.set_id("123654");
  rest.set_name("ESP8266-BREAKOUT");

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&onoffbutton);
  mqtt.subscribe(&firmware_update);
  
}

/************************* Loop *********************************/

void loop() {  

  // If button is pressed, do some action

  buttonState = digitalRead(PIN_BUTTON);
  //  buttonAction();
  
  lightingAction();
  MQTT_connect();

  
 Adafruit_MQTT_Subscribe *subscription;
  
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton) {
//      Serial.print(F("Got: "));
//      Serial.println((char *)onoffbutton.lastread);
      char * action = (char *)onoffbutton.lastread;

      String astr = String(action);

      
      if (astr == "ON") {
        StateOn();
      }
      if (astr == "OFF") {
        StateIdle();
      }
      
    }

    else if (subscription == &firmware_update) {
      Serial.print(F("Got URL: "));

      Serial.print("https://tinyurl.com/");
      Serial.println((char *)firmware_update.lastread);

      
    }
  }
}

void lightingAction() {

  switch (ledState) {
    case 0:    
      // Serial.println("Red");
      digitalWrite(PIN_RED, LOW);
      digitalWrite(PIN_GREEN, HIGH);  // (red)
      digitalWrite(PIN_BLUE, LOW);
      break;
    case 1:
      // Serial.println("Green");
      /*
       * Somehow Blue and Green must be in reversed order to get Green Led Light
       */
      digitalWrite(PIN_RED, LOW);
      digitalWrite(PIN_GREEN, LOW);
      digitalWrite(PIN_BLUE, HIGH); // (green)
      break;
    case 2:    // your hand is a few inches from the sensor
      // Serial.println("Blue");
      digitalWrite(PIN_RED, HIGH);  // (blue)
      digitalWrite(PIN_GREEN, LOW);
      digitalWrite(PIN_BLUE, LOW);     
      break;
    case 3:
      // Serial.println("Yellow");
      digitalWrite(PIN_RED, HIGH);
      digitalWrite(PIN_GREEN, LOW);
      digitalWrite(PIN_BLUE, HIGH);
      break;
    case 4:
      digitalWrite(PIN_RED, LOW);
      digitalWrite(PIN_GREEN, LOW);
      digitalWrite(PIN_BLUE, LOW);    
      break;      
  }
  delay(1);

}

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
