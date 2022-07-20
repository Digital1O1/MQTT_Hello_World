#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define LED_BUILTIN 2
#define TX 1
#define RX 3
#define D0 16
#define D1 5
#define D2 4
#define D3 14 // Don't use for PWM signal
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15 // Don't use when uploading code
#define ADC A0

// --------------------------------------------------- WiFi connection objects --------------------------------------------------- //
WiFiClient espClient;
PubSubClient client(espClient);

// --------------------------------------------------- MQTT connection objects --------------------------------------------------- //

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

// --------------------------------------------------- WiFi variables --------------------------------------------------- //
const char *ssid = "zTURN_spot";                                                               // The SSID (name) of the Wi-Fi network you want to connect to
const char *password = "zTURN_spot!@#";                                                        // Enter WiFi password
const char *mqttServer = "192.168.1.226";                                                      // SDA Server IP address
const char *topic = "hello_world_led";                                                         // Topic the server 'listens' to
const int mqttPort = 1883;                                                                     // Port number
const char *mqttUser = "homeassistant";                                                        // MQTT Server "username"
const char *mqttPassword = "Me6Thuu2coon9bieCai8tiexee3doov6chikucee9tohwie2EeNangaiTeepeiNa"; // MQTT Server password

void MQTTcallback(char *topic, byte *payload, unsigned int length)
{
  String incoming_message;
  Serial.println();
  Serial.println("-------------------------------------------------------");

  Serial.print("\nMessage arrived in topic [ ");
  Serial.print(topic);
  Serial.println(" ] : ");
  Serial.print("Message recieved from UI : ");

  // Read incoming values from LoveLace UI
  for (unsigned int i = 0; i < length; i++)
  {
    incoming_message = incoming_message + (char)payload[i]; // Conver *byte to String || Splits speed and value into two seperate strings
  }
  Serial.println(incoming_message);
  Serial.println();
  Serial.println("\n=======================================================");
  // --------------------------------------------------- DO STUFF WITH INCOMING INFORMATION HERE --------------------------------------------------- //
  /*
  if (incoming_message == "D1_ON")
  {
    Serial.println("D1 ON");
    digitalWrite(D1, HIGH);
  }
  else if (incoming_message == "D1_OFF")

  {
    Serial.println("D1 OFF");
    digitalWrite(D1, LOW);
  }
  else if (incoming_message == "D2_ON")
  {
    Serial.println("D2 ON");
    digitalWrite(D2, HIGH);
  }
  else if (incoming_message == "D2_OFF")

  {
    Serial.println("D1 OFF");
    digitalWrite(D2, LOW);
  }
  */
  // onBoth
}

void reconnect()
{
  while (!client.connected())
  {
    digitalWrite(LED_BUILTIN, HIGH);
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());

    // Serial.printf("The client %s connects to mosquitto mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqttUser, mqttPassword))
    {
      Serial.println("ESP BOARD CONNECTED TO MQTT BROKER");
      client.subscribe(topic);
      digitalWrite(LED_BUILTIN, LOW);
    }
    else
    {
      /*
          Reference link for client.state() : https://github.com/knolleary/pubsubclient/blob/master/src/PubSubClient.h#L45

          // Possible values for client.state()
          #define MQTT_CONNECTION_TIMEOUT     -4
          #define MQTT_CONNECTION_LOST        -3
          #define MQTT_CONNECT_FAILED         -2
          #define MQTT_DISCONNECTED           -1
          #define MQTT_CONNECTED               0
          #define MQTT_CONNECT_BAD_PROTOCOL    1
          #define MQTT_CONNECT_BAD_CLIENT_ID   2
          #define MQTT_CONNECT_UNAVAILABLE     3
          #define MQTT_CONNECT_BAD_CREDENTIALS 4
          #define MQTT_CONNECT_UNAUTHORIZED    5
      */
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(100);
    }
  }
  client.subscribe(topic);
  Serial.print("Subscribed to topic : [ ");
  Serial.print(topic);
  Serial.println(" ]");
}

// --------------------------------------------------- CHECK WIFI CONNECTION --------------------------------------------------- //

void onWifiConnect(const WiFiEventStationModeGotIP &event)
{
  Serial.println("Connected to Wi-Fi sucessfully.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(D1, LOW);
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
  // digitalWrite(LED_BUILTIN,HIGH);
  Serial.println("Disconnected from Wi-Fi, trying to connect...");
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  // client.subscribe(topic);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(D1, HIGH);
}

void setup()
{
  Serial.begin(115200);

  // --------------------------------------------------- WIFI SETUP --------------------------------------------------- //
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D5, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("\n--------------------------------------------");
  Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    Serial.print(".");
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
  }

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print("\nConnected to WiFi : ");
  Serial.println(WiFi.SSID());

  client.setServer(mqttServer, mqttPort);
  client.setCallback(MQTTcallback);

  while (!client.connected()) // If client/ESP loses connection to broker
  {
    Serial.print("Attempting MQTT connection...");
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());

    Serial.printf("The client [ %s ] connects to mosquitto mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqttUser, mqttPassword))
    {
      // client.publish(topic, "hi");
      Serial.println("ESP BOARD CONNECTED TO MQTT BROKER");
      client.subscribe(topic);
      Serial.print("Subscribed to topic : ");
      Serial.println(topic);
    }
    else
    {
      /*
          Reference link : https://github.com/knolleary/pubsubclient/blob/master/src/PubSubClient.h#L45

          --- Possible values for client.state() ---

          #define MQTT_CONNECTION_TIMEOUT     -4
          #define MQTT_CONNECTION_LOST        -3
          #define MQTT_CONNECT_FAILED         -2
          #define MQTT_DISCONNECTED           -1
          #define MQTT_CONNECTED               0
          #define MQTT_CONNECT_BAD_PROTOCOL    1
          #define MQTT_CONNECT_BAD_CLIENT_ID   2
          #define MQTT_CONNECT_UNAVAILABLE     3
          #define MQTT_CONNECT_BAD_CREDENTIALS 4
          #define MQTT_CONNECT_UNAUTHORIZED    5

      */
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(100);
    }
  }

  //  client.subscribe(topic);
  //  Serial.print("Subscribed to topic : ");
  //  Serial.println(topic);

  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("\n------------------------------ PROGRAM STARTED ------------------------------ ");
}

void loop()
{
  client.loop(); // Proccesses message queues

  // client.publish("digital potentiometer", "hi"); // Arguments(const char*, const char * payload)
  if (!client.connected())
  {
    digitalWrite(LED_BUILTIN, LOW);
    reconnect();
    digitalWrite(LED_BUILTIN, HIGH);
  }
  if (!client.loop())
  {
    client.connect("esp8266-client-");
  }
}