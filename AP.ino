

/************************* Client MAC *********************************/

//unsigned char* MAC;
String DEVICE_MAC;

/************************* Router Settings *********************************/

//unsigned char* ROUTERSSID;
//unsigned char* ROUTERPASSWD;
String ROUTERSSID;
String ROUTERPASSWD;

/************************* WiFi Access Point (AP) *********************************/

const char* ssid = "APOLLO16";
const char* password = "12345678";


/************************* TCP Server Settings *********************************/

// The port to listen for incoming TCP connections
#define LISTEN_PORT 80

// Create an instance of the server
WiFiServer server(LISTEN_PORT);


/************************* Variables *********************************/

// Variables to be exposed to the API
//String UID;
//String KEY;


/************************* Function Declarations *********************************/

void exposeVariablesRestService() {

}

/************************* Setting Up the Wifi Server *********************************/

void SetupServer() {

  // Setup WiFi network
  WiFi.softAP(ssid, password);
  Serial.println("");
  Serial.println("WiFi created");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  Serial.println("#####################");
}

void LoopRestServer() {
  
  // Handle REST calls
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  while(!client.available()){
    delay(100);
  }

//  rest.handle(client);
  
}

/************************* Functions *********************************/

int macAdrs(String mac) {
  Serial.println("MAC POST: " + mac);
  DEVICE_MAC = mac;
  return 1;
}

int routerSsid(String ssid) {
  Serial.println("SSID POST: " + ssid);
//  ROUTERSSID = ssid;
  WLAN_SSID_str = ssid;
  return 1;
}

int routerPass(String passwd) {
  Serial.println("PASSWD POST: "+ passwd);
//  ROUTERPASSWD = passwd;
  WLAN_PASS_str = passwd;  
  return 1;
}

int keyExchange(String key) {
  Serial.println(key);
  return 1;
}


