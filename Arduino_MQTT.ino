/**************************************************************
 *
 * For this example, you need to install PubSubClient library:
 *   https://github.com/knolleary/pubsubclient
 *   or from http://librarymanager/all#PubSubClient
 *
 * TinyGSM Getting Started guide:
 *   https://tiny.cc/tinygsm-readme
 *
 * For more MQTT examples, see PubSubClient library
 *
 **************************************************************
 * Use Mosquitto client tools to work with MQTT
 *   Ubuntu/Linux: sudo apt-get install mosquitto-clients
 *   Windows:      https://mosquitto.org/download/
 *
 * Subscribe for messages:
 *   mosquitto_sub -h test.mosquitto.org -t GsmClientTest/init -t GsmClientTest/ledStatus -q 1
 * Toggle led:
 *   mosquitto_pub -h test.mosquitto.org -t GsmClientTest/led -q 1 -m "toggle"
 *
 * You can use Node-RED for wiring together MQTT-enabled devices
 *   https://nodered.org/
 * Also, take a look at these additional Node-RED modules:
 *   node-red-contrib-blynk-ws
 *   node-red-dashboard
 *
 **************************************************************/

// Please select the corresponding model

// #define SIM800L_IP5306_VERSION_20190610
// #define SIM800L_AXP192_VERSION_20200327
// #define SIM800C_AXP192_VERSION_20200609
#define SIM800L_IP5306_VERSION_20200811

#include "utilities.h"

// Select your modem:
#define TINY_GSM_MODEM_SIM800

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial1

// See all AT commands, if wanted
#define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// Add a reception delay - may be needed for a fast processor at a slow baud rate
// #define TINY_GSM_YIELD() { delay(2); }

// Define how you're planning to connect to the internet
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""

const byte ledPin = 13;
const byte oneon = 13;
const byte oneoff = 12;
const byte twoone = 25;
const byte twooff = 14;
const byte threeon = 33;
const byte threeoff = 32;
const byte fouron = 19;
const byte fouroff = 18;
// Your GPRS credentials, if any
const char apn[] = "TM";
const char gprsUser[] = "";
const char gprsPass[] = "";

// MQTT details
const char *broker = "mqtthome.ddns.net";

const char *topicLed = "a";
const char *topicInit = "a";

#include <TinyGsmClient.h>
#include <PubSubClient.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif
TinyGsmClient client(modem);
PubSubClient mqtt(client);

int ledStatus = LOW;

uint32_t lastReconnectAttempt = 0;

void mqttCallback(char *topic, byte *payload, unsigned int len)
{
   Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < len; i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  //Serial.println("----------------------------------------");

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "a") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
      digitalWrite(oneoff, LOW);
      digitalWrite(oneon, LOW);
      digitalWrite(twoone, LOW);
      digitalWrite(twooff, LOW);
      digitalWrite(threeon, LOW);
      digitalWrite(threeoff, LOW);
      digitalWrite(fouron, LOW);
      digitalWrite(fouroff, LOW);
    }
    else if(messageTemp == "oneon"){
      Serial.println("----------------------------------------");
      Serial.println("----------------------------------------");
      Serial.println("----------------------------------------");
      Serial.println("oneon");
      digitalWrite(oneon, HIGH);
      digitalWrite(oneoff, LOW);
    }
    else if(messageTemp == "oneoff"){
      
      Serial.println("oneoff");
      digitalWrite(oneon, LOW);
      digitalWrite(oneoff, HIGH);
    }
    else if(messageTemp == "twoone"){
      Serial.println("----------------------------------------");
      Serial.println("----------------------------------------");
      Serial.println("twoon");
      digitalWrite(twoone, HIGH);
      digitalWrite(twooff, LOW);
    }
    else if(messageTemp == "twooff"){
      Serial.println("----------------------------------------");
      Serial.println("----------------------------------------");
      Serial.println("----------------------------------------");
      Serial.println("twooff");
      digitalWrite(twoone, LOW);
      digitalWrite(twooff, HIGH);
    }
    else if(messageTemp == "threeon"){
      Serial.println("----------------------------------------");
      Serial.println("----------------------------------------");
      Serial.println("threeon");
      digitalWrite(threeon, HIGH);
      digitalWrite(threeoff, LOW);
    }
    else if(messageTemp == "threeoff"){
      Serial.println("----------------------------------------");
      Serial.println("----------------------------------------");
      Serial.println("----------------------------------------");
      Serial.println("threeoff");
      digitalWrite(threeon, LOW);
      digitalWrite(threeoff, HIGH);
    }
     else if(messageTemp == "fouron"){
      Serial.println("----------------------------------------");
      Serial.println("----------------------------------------");
      Serial.println("fouron");
      digitalWrite(fouron, HIGH);
      digitalWrite(fouroff, LOW);
    }
    else if(messageTemp == "fouroff"){
      Serial.println("----------------------------------------");
      Serial.println("----------------------------------------");
      Serial.println("----------------------------------------");
      Serial.println("fouroff");
      digitalWrite(fouron, LOW);
      digitalWrite(fouroff, HIGH);
    }
     
  }
  //Serial.println("----------------------------------------");
}

boolean mqttConnect()
{
    SerialMon.print("Connecting to ");
    SerialMon.print(broker);

    // Connect to MQTT Broker
    boolean status = mqtt.connect("GsmClientTest");

    // Or, if you want to authenticate MQTT:
    //boolean status = mqtt.connect("GsmClientName", "mqtt_user", "mqtt_pass");

    if (status == false) {
        SerialMon.println(" fail");
        return false;
    }
    SerialMon.println(" success");
    mqtt.publish(topicInit, "GsmClientTest started");
    mqtt.subscribe(topicLed);
    return mqtt.connected();
}


void setup()
{
    pinMode(oneon, OUTPUT);
    pinMode(oneoff, OUTPUT);
    pinMode(twoone, OUTPUT);
    pinMode(twooff, OUTPUT);
    pinMode(threeon, OUTPUT);
    pinMode(threeoff, OUTPUT);
    pinMode(fouron, OUTPUT);
    pinMode(fouroff, OUTPUT);;
    
    // Set console baud rate
    SerialMon.begin(115200);

    delay(10);

    setupModem();

    SerialMon.println("Wait...");

    // Set GSM module baud rate and UART pins
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

    delay(6000);

    // Restart takes quite some time
    // To skip it, call init() instead of restart()
    SerialMon.println("Initializing modem...");
    modem.restart();
    // modem.init();

    String modemInfo = modem.getModemInfo();
    SerialMon.print("Modem Info: ");
    SerialMon.println(modemInfo);

#if TINY_GSM_USE_GPRS
    // Unlock your SIM card with a PIN if needed
    if ( GSM_PIN && modem.getSimStatus() != 3 ) {
        modem.simUnlock(GSM_PIN);
    }
#endif

    SerialMon.print("Waiting for network...");
    if (!modem.waitForNetwork()) {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    SerialMon.println(" success");

    if (modem.isNetworkConnected()) {
        SerialMon.println("Network connected");
    }

    // GPRS connection parameters are usually set after network registration
    SerialMon.print(F("Connecting to "));
    SerialMon.print(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    SerialMon.println(" success");

    if (modem.isGprsConnected()) {
        SerialMon.println("GPRS connected");
    }

    // MQTT Broker setup
    mqtt.setServer(broker, 1884);
    mqtt.setCallback(mqttCallback);
    
}

void loop()
{

    if (!mqtt.connected()) {
        SerialMon.println("=== MQTT NOT CONNECTED ===");
        // Reconnect every 10 seconds
        uint32_t t = millis();
        if (t - lastReconnectAttempt > 10000L) {
            lastReconnectAttempt = t;
            if (mqttConnect()) {
                lastReconnectAttempt = 0;
            }
        }
        delay(100);
        return;
    }

    mqtt.loop();
}
