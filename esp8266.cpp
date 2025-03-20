/********************************************************************************** 
 *  TITLE: Arduino IoT Cloud control 4 Relays using NodeMCU
 *  Click on the following links to learn more.
 *  YouTube Video: https://youtu.be/K09vSRz3Ugk
 *  Related Blog : https://iotcircuithub.com/esp8266-projects/
 *  by Tech StudyCell
 *  Preferences--> Aditional boards Manager URLs :  
 *  https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *  
 *  Download Board ESP8266 NodeMCU : https://github.com/esp8266/Arduino
 *  Download the libraries
 *  ArduinoIoTCloud Library with all the dependencies
 **********************************************************************************/

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include <ESP8266Wifi.h>

const char DEVICE_LOGIN_NAME[]  = "809fa5dc-7cf4-4daa-9e25-48a1982095bc"; //Enter DEVICE ID

const char SSID[]               = "POCOM3";    //Enter WiFi SSID (name)
const char PASS[]               = "9495105481";    //Enter WiFi password
const char DEVICE_KEY[]         = "WfZ6aXX8#Uhv!4cfthq88aIra";    //Enter Secret device password (Secret Key)


// define the GPIO connected with Relays and switches
#define RelayPin1 5  //D1
#define RelayPin2 4  //D2
#define RelayPin3 14 //D5
#define RelayPin4 12 //D6

#define SwitchPin1 10  //SD3
#define SwitchPin2 0   //D3 
#define SwitchPin3 13  //D7
#define SwitchPin4 3   //RX

#define wifiLed   16   //D0


int toggleState_1 = 1; //Define integer to remember the toggle state for relay 1
int toggleState_2 = 1; //Define integer to remember the toggle state for relay 2
int toggleState_3 = 1; //Define integer to remember the toggle state for relay 3
int toggleState_4 = 1; //Define integer to remember the toggle state for relay 4

void onSwitch1Change();
void onSwitch2Change();
void onSwitch3Change();
void onSwitch4Change();

CloudSwitch switch1;
CloudSwitch switch2;
CloudSwitch switch3;
CloudSwitch switch4;

void initProperties(){

  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);

  ArduinoCloud.addProperty(switch1, READWRITE, ON_CHANGE, onSwitch1Change);
  ArduinoCloud.addProperty(switch2, READWRITE, ON_CHANGE, onSwitch2Change);
  ArduinoCloud.addProperty(switch3, READWRITE, ON_CHANGE, onSwitch3Change);
  ArduinoCloud.addProperty(switch4, READWRITE, ON_CHANGE, onSwitch4Change);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

const int debounceDelay = 200; // Debounce delay in milliseconds
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long lastDebounceTime3 = 0;
unsigned long lastDebounceTime4 = 0;

// Track the previous state of each switch to handle edge detection
bool previousSwitchState1 = HIGH;
bool previousSwitchState2 = HIGH;
bool previousSwitchState3 = HIGH;
bool previousSwitchState4 = HIGH;

void manual_control() {
  unsigned long currentMillis = millis();

  // Manual Switch Control
  // Check each switch pin and toggle the corresponding relay if the switch is pressed (LOW state)
  
  // Check if Switch 1 is pressed
  if (digitalRead(SwitchPin1) == LOW && previousSwitchState1 == HIGH) {
    if (currentMillis - lastDebounceTime1 >= debounceDelay) {
      toggleState_1 = !toggleState_1;
      digitalWrite(RelayPin1, !toggleState_1);
      switch1 = toggleState_1; // Toggle Relay 1
      lastDebounceTime1 = currentMillis; // Update the last debounce time
    }
  }
  previousSwitchState1 = digitalRead(SwitchPin1);

  // Check if Switch 2 is pressed
  if (digitalRead(SwitchPin2) == LOW && previousSwitchState2 == HIGH) {
    if (currentMillis - lastDebounceTime2 >= debounceDelay) {
      toggleState_2 = !toggleState_2;
      digitalWrite(RelayPin2, !toggleState_2);
      switch2 = toggleState_2; // Toggle Relay 2
      lastDebounceTime2 = currentMillis; // Update the last debounce time
    }
  }
  previousSwitchState2 = digitalRead(SwitchPin2);

  // Check if Switch 3 is pressed
  if (digitalRead(SwitchPin3) == LOW && previousSwitchState3 == HIGH) {
    if (currentMillis - lastDebounceTime3 >= debounceDelay) {
      toggleState_3 = !toggleState_3;
      digitalWrite(RelayPin3, !toggleState_3);
      switch3 = toggleState_3; // Toggle Relay 3
      lastDebounceTime3 = currentMillis; // Update the last debounce time
    }
  }
  previousSwitchState3 = digitalRead(SwitchPin3);

  // Check if Switch 4 is pressed
  if (digitalRead(SwitchPin4) == LOW && previousSwitchState4 == HIGH) {
    if (currentMillis - lastDebounceTime4 >= debounceDelay) {
      toggleState_4 = !toggleState_4;
      digitalWrite(RelayPin4, !toggleState_4);
      switch4 = toggleState_4; // Toggle Relay 4
      lastDebounceTime4 = currentMillis; // Update the last debounce time
    }
  }
  previousSwitchState4 = digitalRead(SwitchPin4);
}


void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);

  pinMode(wifiLed, OUTPUT);

  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  pinMode(SwitchPin3, INPUT_PULLUP);
  pinMode(SwitchPin4, INPUT_PULLUP);

  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, HIGH);
  digitalWrite(RelayPin2, HIGH);
  digitalWrite(RelayPin3, HIGH);
  digitalWrite(RelayPin4, HIGH);

  digitalWrite(wifiLed, HIGH);  //Turn OFF WiFi LED
}

void loop() {
  ArduinoCloud.update();
  
  manual_control(); //Control relays manually

  if (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(wifiLed, HIGH); //Turn OFF WiFi LED
    //Serial.println("NO WIFI");
  }
  else{
    digitalWrite(wifiLed, LOW); //Turn ON WiFi LED
    //Serial.println("YES WIFI");
  }
}

void onSwitch1Change() {
  if (switch1 == 1)
  {
    digitalWrite(RelayPin1, LOW);
    Serial.println("Device1 ON");
    toggleState_1 = 1;
  }
  else
  {
    digitalWrite(RelayPin1, HIGH);
    Serial.println("Device1 OFF");
    toggleState_1 = 0;
  }
}

void onSwitch2Change() {
  if (switch2 == 1)
  {
    digitalWrite(RelayPin2, LOW);
    Serial.println("Device2 ON");
    toggleState_2 = 1;
  }
  else
  {
    digitalWrite(RelayPin2, HIGH);
    Serial.println("Device2 OFF");
    toggleState_2 = 0;
  }
}

void onSwitch3Change() {
  if (switch3 == 1)
  {
    digitalWrite(RelayPin3, LOW);
    Serial.println("Device2 ON");
    toggleState_3 = 1;
  }
  else
  {
    digitalWrite(RelayPin3, HIGH);
    Serial.println("Device3 OFF");
    toggleState_3 = 0;
  }
}

void onSwitch4Change() {
  if (switch4 == 1)
  {
    digitalWrite(RelayPin4, LOW);
    Serial.println("Device4 ON");
    toggleState_4 = 1;
  }
  else
  {
    digitalWrite(RelayPin4, HIGH);
    Serial.println("Device4 OFF");
    toggleState_4 = 0;
  }
}






/**********************************************************************************
 *  TITLE: Arduino IoT Cloud + IR + Manual (Switch) control 4 Relays with DHT11 & Real-Time feedback using ESP32
 *  Click on the following links to learn more. 
 *  YouTube Video: https://youtu.be/UlT72MMbPu8
 *  Related Blog : 
 *  by Tech StudyCell
 *  Preferences--> Aditional boards Manager URLs : 
 *  https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *  
 *  Download Board ESP32 : https://github.com/espressif/arduino-esp32
 *  Download the libraries
 *  ArduinoIoTCloud Library (Version 2.0.1) with all the dependencies: https://github.com/arduino-libraries/ArduinoIoTCloud
 *  IRremote Library (Version 3.3.0): https://github.com/Arduino-IRremote/Arduino-IRremote
 *  DHT Library (Version 1.4.6): https://github.com/adafruit/DHT-sensor-library
 **********************************************************************************/

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include <DHT.h>
#include <IRremote.h>

const char DEVICE_LOGIN_NAME[]  = "809fa5dc-7cf4-4daa-9e25-48a1982095bc"; //Enter DEVICE ID

const char SSID[]               = "POCOM3";    //Enter WiFi SSID (name)
const char PASS[]               = "9495105481";    //Enter WiFi password
const char DEVICE_KEY[]         = "WfZ6aXX8#Uhv!4cfthq88aIra";    //Enter Secret device password (Secret Key)

#define DHTPIN              4 //D4  pin connected with DHT
#define IR_RECV_PIN         35 //D35 pin connected with IR Receiver IC

// define the GPIO connected with Relays and switches
#define RelayPin1 23  //D23
#define RelayPin2 19  //D19
#define RelayPin3 18  //D18
#define RelayPin4 5  //D5

#define SwitchPin1 13  //D13
#define SwitchPin2 12  //D12
#define SwitchPin3 14  //D14
#define SwitchPin4 27  //D27

#define wifiLed    2   //D2

// Uncomment whatever type you're using!
#define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301


DHT dht(DHTPIN, DHTTYPE);
IRrecv irrecv(IR_RECV_PIN);
decode_results results;

int toggleState_1 = 0; //Define integer to remember the toggle state for relay 1
int toggleState_2 = 0; //Define integer to remember the toggle state for relay 2
int toggleState_3 = 0; //Define integer to remember the toggle state for relay 3
int toggleState_4 = 0; //Define integer to remember the toggle state for relay 4

// Switch State
bool SwitchState_1 = LOW;
bool SwitchState_2 = LOW;
bool SwitchState_3 = LOW;
bool SwitchState_4 = LOW;

float temperature1 = 0;
float humidity1   = 0;
int   reconnectFlag = 0;

void onLight1Change();
void onLight2Change();
void onLight3Change();
void onLight4Change();

CloudSwitch light1;
CloudSwitch light2;
CloudSwitch light3;
CloudSwitch light4;
CloudTemperatureSensor temperature;

void initProperties(){

  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  ArduinoCloud.addProperty(light1, READWRITE, ON_CHANGE, onLight1Change);
  ArduinoCloud.addProperty(light2, READWRITE, ON_CHANGE, onLight2Change);
  ArduinoCloud.addProperty(light3, READWRITE, ON_CHANGE, onLight3Change);
  ArduinoCloud.addProperty(light4, READWRITE, ON_CHANGE, onLight4Change);
  ArduinoCloud.addProperty(temperature, READ, 8 * SECONDS, NULL); //Update temperature value after every 8 seconds
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);


void readSensor(){
     
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
  
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  else {
    humidity1 = h;
    temperature = t;
   // Serial.println(tempareture);
  }  
}

void sendSensor()
{
  readSensor();
}

void ir_remote_control(){
  if (irrecv.decode(&results)) {
    
      switch(results.value){
          case 0x80BF49B6:  relayOnOff(1); light1 = toggleState_1; break; //update the HEX-code
          case 0x80BFC936:  relayOnOff(2); light2 = toggleState_2; break; //update the HEX-code
          case 0x80BF33CC:  relayOnOff(3); light3 = toggleState_3; break; //update the HEX-code
          case 0x80BF718E:  relayOnOff(4); light4 = toggleState_4; break; //update the HEX-code
          default : break;         
        }   
        //Serial.println(results.value, HEX);    
        irrecv.resume();   
  } 
}

void relayOnOff(int relay) {

  switch (relay) {
    case 1:
      if (toggleState_1 == 0) {
        digitalWrite(RelayPin1, LOW); // turn on relay 1
        toggleState_1 = 1;
        Serial.println("Device1 ON");
      }
      else {
        digitalWrite(RelayPin1, HIGH); // turn off relay 1
        toggleState_1 = 0;
        Serial.println("Device1 OFF");
      }
      delay(100);
      break;
    case 2:
      if (toggleState_2 == 0) {
        digitalWrite(RelayPin2, LOW); // turn on relay 2
        toggleState_2 = 1;
        Serial.println("Device2 ON");
      }
      else {
        digitalWrite(RelayPin2, HIGH); // turn off relay 2
        toggleState_2 = 0;
        Serial.println("Device2 OFF");
      }
      delay(100);
      break;
    case 3:
      if (toggleState_3 == 0) {
        digitalWrite(RelayPin3, LOW); // turn on relay 3
        toggleState_3 = 1;
        Serial.println("Device3 ON");
      } else {
        digitalWrite(RelayPin3, HIGH); // turn off relay 3
        toggleState_3 = 0;
        Serial.println("Device3 OFF");
      }
      delay(100);
      break;
    case 4:
      if (toggleState_4 == 0) {
        digitalWrite(RelayPin4, LOW); // turn on relay 4
        toggleState_4 = 1;
        Serial.println("Device4 ON");
      }
      else {
        digitalWrite(RelayPin4, HIGH); // turn off relay 4
        toggleState_4 = 0;
        Serial.println("Device4 OFF");
      }
      delay(100);
      break;
    default : break;
  }
}

void manual_control()
{
  if (digitalRead(SwitchPin1) == LOW && SwitchState_1 == LOW) {
    digitalWrite(RelayPin1, LOW);
    toggleState_1 = 1;
    SwitchState_1 = HIGH;
    light1 = toggleState_1;
    Serial.println("Switch-1 on");
  }
  if (digitalRead(SwitchPin1) == HIGH && SwitchState_1 == HIGH) {
    digitalWrite(RelayPin1, HIGH);
    toggleState_1 = 0;
    SwitchState_1 = LOW;
    light1 = toggleState_1;
    Serial.println("Switch-1 off");
  }
  if (digitalRead(SwitchPin2) == LOW && SwitchState_2 == LOW) {
    digitalWrite(RelayPin2, LOW);
    toggleState_2 = 1;
    SwitchState_2 = HIGH;
    light2 = toggleState_2;
    Serial.println("Switch-2 on");
  }
  if (digitalRead(SwitchPin2) == HIGH && SwitchState_2 == HIGH) {
    digitalWrite(RelayPin2, HIGH);
    toggleState_2 = 0;
    SwitchState_2 = LOW;
    light2 = toggleState_2;
    Serial.println("Switch-2 off");
  }
  if (digitalRead(SwitchPin3) == LOW && SwitchState_3 == LOW) {
    digitalWrite(RelayPin3, LOW);
    toggleState_3 = 1;
    SwitchState_3 = HIGH;
    light3 = toggleState_3;
    Serial.println("Switch-3 on");
  }
  if (digitalRead(SwitchPin3) == HIGH && SwitchState_3 == HIGH) {
    digitalWrite(RelayPin3, HIGH);
    toggleState_3 = 0;
    SwitchState_3 = LOW;
    light3 = toggleState_3;
    Serial.println("Switch-3 off");
  }
  if (digitalRead(SwitchPin4) == LOW && SwitchState_4 == LOW) {
    digitalWrite(RelayPin4, LOW);
    toggleState_4 = 1;
    SwitchState_4 = HIGH;
    light4 = toggleState_4;
    Serial.println("Switch-4 on");
  }
  if (digitalRead(SwitchPin4) == HIGH && SwitchState_4 == HIGH) {
    digitalWrite(RelayPin4, HIGH);
    toggleState_4 = 0;
    SwitchState_4 = LOW;
    light4 = toggleState_4;
    Serial.println("Switch-4 off");
  }
}  

void doThisOnConnect(){
  /* add your custom code here */
  Serial.println("Board successfully connected to Arduino IoT Cloud");
  digitalWrite(wifiLed, HIGH); //Turn off WiFi LED
}
void doThisOnSync(){
  /* add your custom code here */
  Serial.println("Thing Properties synchronised");
}

void doThisOnDisconnect(){
  /* add your custom code here */
  Serial.println("Board disconnected from Arduino IoT Cloud");
  digitalWrite(wifiLed, LOW); //Turn off WiFi LED
}

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  // Defined in thingProperties.h
  initProperties();
  dht.begin();
  irrecv.enableIRIn(); // Start the receiver
  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  ArduinoCloud.addCallback(ArduinoIoTCloudEvent::CONNECT, doThisOnConnect);
  ArduinoCloud.addCallback(ArduinoIoTCloudEvent::SYNC, doThisOnSync);
  ArduinoCloud.addCallback(ArduinoIoTCloudEvent::DISCONNECT, doThisOnDisconnect);

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);

  pinMode(wifiLed, OUTPUT);

  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  pinMode(SwitchPin3, INPUT_PULLUP);
  pinMode(SwitchPin4, INPUT_PULLUP);

  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, HIGH);
  digitalWrite(RelayPin2, HIGH);
  digitalWrite(RelayPin3, HIGH);
  digitalWrite(RelayPin4, HIGH);
}

void loop() {

  ArduinoCloud.update();
  
  manual_control();     //Manual Control
  ir_remote_control();  //IR Remote Control
  sendSensor();         //Get Sensor Data
}

void onLight1Change() {
  //Control the device
  if (light1 == 1)
  {
    digitalWrite(RelayPin1, LOW);
    Serial.println("Device1 ON");
    toggleState_1 = 1;
  }
  else
  {
    digitalWrite(RelayPin1, HIGH);
    Serial.println("Device1 OFF");
    toggleState_1 = 0;
  }
}

void onLight2Change() {
  if (light2 == 1)
  {
    digitalWrite(RelayPin2, LOW);
    Serial.println("Device2 ON");
    toggleState_2 = 1;
  }
  else
  {
    digitalWrite(RelayPin2, HIGH);
    Serial.println("Device2 OFF");
    toggleState_2 = 0;
  }
}

void onLight3Change() {
  if (light3 == 1)
  {
    digitalWrite(RelayPin3, LOW);
    Serial.println("Device3 ON");
    toggleState_3 = 1;
  }
  else
  {
    digitalWrite(RelayPin3, HIGH);
    Serial.println("Device3 OFF");
    toggleState_3 = 0;
  }
}

void onLight4Change() {
  if (light4 == 1)
  {
    digitalWrite(RelayPin4, LOW);
    Serial.println("Device4 ON");
    toggleState_4 = 1;
  }
  else
  {
    digitalWrite(RelayPin4, HIGH);
    Serial.println("Device4 OFF");
    toggleState_4 = 0;
  }
}

