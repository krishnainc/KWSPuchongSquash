#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Firebase_ESP_Client.h>
#include <WiFiManager.h> 

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"


#define API_KEY "AIzaSyCtF2PDFrAujL9AnGP3TlP1B2Bez0-VUeg"
#define USER_EMAIL "kelabwawasanskuasypuchong@gmail.com"
#define USER_PASSWORD "KWSPuchong123"
#define DATABASE_URL "https://squashredone-default-rtdb.firebaseio.com/" 

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 180000;
bool signupOK = false;

//time
String fullhour;
String fullminute;
String fulltime;

String uid;

//Database main path
String databasePath1;
String databasePath2;

//Safebox
String start = "/start";
String end = "/end";

//buffers
int starttime1;
int endtime1;

int starttime2;
int endtime2;

//Parent Node
String parentPath;

FirebaseJson json;
WiFiClient client;

const int RelayPin1 = 4;
const int RelayPin2 = 0;


// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//Week Days
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Month names
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

void setup() {

  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  
  digitalWrite(RelayPin1, HIGH);
  digitalWrite(RelayPin2, HIGH);

  Serial.begin(115200);
  delay(1500); 

  WiFiManager wifiManager;
  wifiManager.autoConnect("KWSP Circuit Controller");
  

// Initialize a NTPClient to get time
  timeClient.begin();
  timeClient.setTimeOffset(0);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  config.token_status_callback = tokenStatusCallback;
  config.max_token_generation_retry = 5;
  Firebase.begin(&config, &auth);

  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath1 = "/Safebox1";
  databasePath2 = "/Safebox2";

}

void loop() {

  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  // Serial.print("Epoch Time: ");
  // Serial.println(epochTime);


  if (Firebase.ready() && (millis() - sendDataPrevMillis > 2000 || sendDataPrevMillis == 0))
    {
      sendDataPrevMillis = millis();

    //keybox1
      if (Firebase.RTDB.getInt(&fbdo, databasePath1 + start)) 
          {
            if (fbdo.dataType() == "int") 
              {
                starttime1 = fbdo.intData();
              }
          }
  
        else 
          {
            Serial.println(fbdo.errorReason());
          }


      if (Firebase.RTDB.getInt(&fbdo, databasePath1 + end)) 
          {
            if (fbdo.dataType() == "int") 
              {
                endtime1 = fbdo.intData();
              }
          }
  
        else 
          {
            Serial.println(fbdo.errorReason());
          }




      //keybox2
      if (Firebase.RTDB.getInt(&fbdo, databasePath2 + start)) 
          {
            if (fbdo.dataType() == "int") 
              {
                starttime2 = fbdo.intData();
              }
          }
  
        else 
          {
            Serial.println(fbdo.errorReason());
          }


      if (Firebase.RTDB.getInt(&fbdo, databasePath2 + end)) 
          {
            if (fbdo.dataType() == "int") 
              {
                endtime2 = fbdo.intData();
              }
          }
  
        else 
          {
            Serial.println(fbdo.errorReason());
          }


    }

 
   
    // Serial.println(starttime1);
    // Serial.println(starttime2);
    // Serial.println(endtime1);
    // Serial.println(endtime2);
    
    if(epochTime == starttime1)
    {
      Serial.println("relay1on");
      digitalWrite(RelayPin1, LOW);
    }

    else if (epochTime == endtime1)
    {
      Serial.println("relay1off");
      digitalWrite(RelayPin1, HIGH);
    }

    if(epochTime == starttime2)
    {
      Serial.println("relay2on");
      digitalWrite(RelayPin2, LOW);
    }

    else if (epochTime == endtime2)
    {
      Serial.println("relay2off");
      digitalWrite(RelayPin2, HIGH);
    }




}