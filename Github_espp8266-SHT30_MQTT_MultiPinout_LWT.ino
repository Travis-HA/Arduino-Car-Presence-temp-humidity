
/*
*
* ESP8266 (Wemos D1 mini) + SHT30 + LED + Multiple MQTT + Home-Assistant + LWT + Web Updater
* Created by Travis Hayes Sept 2019
* Code stolen from various online sources + Arduino sketch examples
*
*/

#include <EspMQTTClient.h>
#include <ESP8266WiFi.h>
#include <WEMOS_SHT3X.h>                                      // Library for SHT-30 Temp/Humidity Shield

#define wifi_ssid "my_wifi_ssid"
#define wifi_password "my_wifi_pw"

#define http_user "admin"                                     //HTTP username for Web updater (found at IP address)
#define http_pw "admin"                                       //HTTP password for Web updater

#define mqtt_server "192.168.1.100"                           //MQTT server address eg on Raspberry Pi
#define MQTT_Client "Wemos_Client-car"                        //MQTT Client - unique for every device connected to MQTT broker
#define mqtt_user "username"                                  //MQTT username - same for all devices connected to broker
#define mqtt_password "password"                              //MQTT password - same for all devices connected to broker
#define mqtt_port 1883                                        //MQTT Port - default is 1883

#define lwt_topic "home/car/wemos/lwt"                        //LWT Topic
#define lwt_msg "Away"                                        //LWT message
#define startup_msg "Home"                                    //LWT startup message

#define temperature_topic "home/car/sensors/temperature"      //Temperature Topic
#define humidity_topic "home/car/sensors/humidity"            //Humidity Topic
#define subscribe_topic1 "home/cari/wemos/switch1"            //Subscribed Topic
#define subscribe_topic2 "home/car/wemos/switch2"             //Subscribed Topic
#define subscribe_topic3 "home/car/wemos/switch3"             //Subscribed Topic

#define interval_time 300                   //Time interval between sensor readings







char message_buff[100];                     //Buffer for decoding received MQTT messages
long lastMsg = 0;                           //Timestamp of the last message posted on MQTT
long lastReceived = 0;
bool debug = true;                          //Shows incoming MQTT msgs on the serial monitor if true


//Enable SHT Temperature / Humidity shield
SHT3X sht30(0x45);     

//MQTT Client (espClient) setup
EspMQTTClient client(
  wifi_ssid,              // WiFI SSID
  wifi_password,          // WiFi Password
  mqtt_server,            // MQTT Broker server ip
  mqtt_user,              // Can be omitted if not needed
  mqtt_password,          // Can be omitted if not needed
  MQTT_Client,            // Beginning of Client ID that uniquely identify your device
  mqtt_port               // The MQTT port, default to 1883. this line can be omitted
);

void setup() {
  Serial.begin(115200);                     //Serial port to debug

  // Optional functionalities of EspMQTTClient : 
  client.enableDebuggingMessages(true); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater( http_user , http_pw ); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  client.enableLastWillMessage(lwt_topic, lwt_msg, true);  // You can activate the retain flag by setting the third parameter to true
  
  
  pinMode(D7,OUTPUT);                       //Pin 7 on Wemos D1 mini
  pinMode(D6,OUTPUT);                       //Pin 6 on Wemos D1 mini
  pinMode(D5,OUTPUT);                       //Pin 5 on Wemos D1 mini
  
}

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  client.subscribe(subscribe_topic1, [](const String & payload) {});
  client.subscribe(subscribe_topic2, [](const String & payload) {});
  client.subscribe(subscribe_topic3, [](const String & payload) {});
  
  Serial.print("MQTT: LWT message will be set as '");
  Serial.print(lwt_msg);
  Serial.print("' to topic: ");
  Serial.println(lwt_topic);
  Serial.println("");
  
  client.publish(lwt_topic, startup_msg, true); // You can activate the retain flag by setting the third parameter to true
  
  client.publish(temperature_topic, String(sht30.cTemp).c_str(), false);      //Initial Publish on startup before the loop temperature_topic
  client.publish(humidity_topic, String(sht30.humidity).c_str(), false);      //Initial Publish on startup before the loop  humidity_topic
}

void loop()
{
   
  client.loop();

  long now = millis();
  
  if (now - lastMsg > 1000 * interval_time) {   //Number of seconds between serial messages (in millisecs * interval time set above)
    lastMsg = now;

  if(sht30.get()==0){
    Serial.println("");
  //  Serial.print("Temperature in Celsius : ");
  //  Serial.println(sht30.cTemp);
  //  Serial.print("Temperature in Fahrenheit : ");
  //  Serial.println(sht30.fTemp);
  //  Serial.print("Relative Humidity : ");
  //  Serial.println(sht30.humidity);
  //  Serial.println();
  }
  else
  {
    Serial.println("SHT Sensor Error!");
  }
   
    client.publish(temperature_topic, String(sht30.cTemp).c_str(), false);      //Publish the temperature to the topic temperature_topic
    client.publish(humidity_topic, String(sht30.humidity).c_str(), false);      //...and humidity_topic

  }

}

// Triggers actions upon receipt of a message
// As per http://m2mio.tumblr.com/post/30048662088/a-simple-example-arduino-mqtt-m2mio
void callback(char* topic, byte* payload, unsigned int length) {

  int i = 0;
  if ( debug ) {
    Serial.println("Message received =>  topic: " + String(topic));
    Serial.print(" | Payload length: " + String(length,DEC));
    Serial.println();
  }
  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  
  String msgString = String(message_buff);
  if ( debug ) {
    Serial.println(" | Payload text: " + msgString);
    Serial.println();
  }

 if ( String(topic) == subscribe_topic1 ) {
  
  if ( msgString == "ON" ) {
    digitalWrite(D7,HIGH);  
  } else {
    digitalWrite(D7,LOW);  
    }
 }
  else if ( String(topic) == subscribe_topic2 ) {
 
  if ( msgString == "ON" ) {
    digitalWrite(D6,HIGH);  
  } else {
    digitalWrite(D6,LOW);  
    }
  } 
  else if ( String(topic) == subscribe_topic3 ) {
 
  if ( msgString == "ON" ) {
    digitalWrite(D5,HIGH);  
  } else {
    digitalWrite(D5,LOW);  
    }
  }
  else
    {

  Serial.println("Error =>  no GPIO pin assigned to MQTT topic: " + String(topic));
      
  }
}
