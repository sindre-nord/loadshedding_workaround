//Map D1 to pin 5, stricly not necessary but it makes it easier to read.
//Remove if error occurs
#define D1 5

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <time.h>

//Wifi Credentials and constants
#define ssid "SomeWifiSSID"
#define password "SomeWifiPassword"

char systemState = 'init';

int reconnectWifiCounter = 0; //If we cant reconnect to the wifi after x amount of tries, reset the device.
int reconnectWifiLimit = 10; //The amount of times we wait for the wifi to reconnect before resetting the device.

String icsFileGithub = "https://github.com/beyarkay/eskom-calendar/releases/download/latest/city-of-cape-town-area-8.ics";


void setup() {
  pinMode(D1, OUTPUT); //D1 is the control pin for the relay making it go from NO to closed.

  //Setup the wifi connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  //Waits for the connection and gives some feedback to the user.
  Serial.begin(115200);
  Serial.println("Connecting to WiFi..");
  while(WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }
  //Setup NTP time server:
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
}

void(* resetFunc) (void) = 0;  // declare reset fuction at address 0


void loop() {
  switch (systemState)
  {
  case 'reconnectWifi':
    //Check if we have a wifi connection, if not try to connect.
    if(WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, password);
    }
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
      reconnectWifiCounter++;
      if(reconnectWifiCounter >= reconnectWifiLimit) {
        resetFunc();
      }
    }
    reconnectWifiCounter = 0;
    systemState = 'retriveIcsFile';
    break;

  case 'retriveIcsFile':
    //Check if we have a wifi connection, if not try to connect.
    if(WiFi.status() != WL_CONNECTED) {
      systemState = 'init';
    }
    //Retrive the ics file from the source by get request.
    doGetRequest();
    systemState = 'compareTime';
    break;
  case 'compareTime':
    //Check if we have a wifi connection, if not try to connect.
    if(WiFi.status() != WL_CONNECTED) {
      systemState = 'init';
    }
    //Compare the time to the ics file and see if we need to turn on the relay.
    //TBD
    Serial.println("Compare time");
    //Sleep for 1 second
    delay(1000);
    break;
  default:
    break;
  }


  //Check if we still have a wifi connection, try to reconnect if not.
  if(WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
  }

  //Update the time
  
  //Check the ics file and parse it.

  //Check if the time is within the range of x amount of time before the loadshedding event,
  //then turn on the relay and wait for y amout of time to pass.

}

//Function to toggle the relay
void setRelayTo(bool state) {
  digitalWrite(D1, state);
}

void doGetRequest(){
  HTTPClient http;
    http.begin(icsFileGithub);
    int httpCode = http.GET();
    if(httpCode > 0) {
      if(httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    }
    http.end();
}