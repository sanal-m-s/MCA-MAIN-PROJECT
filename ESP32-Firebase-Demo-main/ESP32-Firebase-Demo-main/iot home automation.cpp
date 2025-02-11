#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

// WiFi Credentials
#define WIFI_SSID "iphone"  // Replace with your WiFi SSID
#define WIFI_PASSWORD "**********"  // Replace with your WiFi Password

// Firebase Credentials
#define FIREBASE_HOST "home-automation-system-253a5-default-rtdb.firebaseio.com"  // Firebase Realtime Database URL (without http:// and /)
#define FIREBASE_AUTH "UVOaNN1Fs6jVfthVX9Ubc8wvVEjMimiAHamtXm5w"         // Firebase Database Secret Key

// Firebase Objects
FirebaseData firebaseData;
FirebaseAuth firebaseAuth;
FirebaseConfig firebaseConfig;

//  Relay and Switch GPIO Definitions
#define Relay1 16  // GPIO 16 (D0)
#define Relay2 5   // GPIO 5  (D1)
#define Relay3 4   // GPIO 4  (D2)
#define Relay4 0   // GPIO 0  (D3)

#define Switch1 10 // GPIO SD3
#define Switch2 9  // GPIO SD2
#define Switch3 12 // GPIO D5
#define Switch4 14 // GPIO D6

#define wifiLed 2  // GPIO D4 (Indicator LED)

// Variables for Relay States
int load1 = 1, load2 = 1, load3 = 1, load4 = 1, Power = 0;

void setup() {
    Serial.begin(115200);

    // Initialize WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");

    // Initialize Firebase
    firebaseConfig.host = FIREBASE_HOST;
    firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&firebaseConfig, &firebaseAuth);
    Firebase.reconnectWiFi(true);

    // Set Pin Modes
    pinMode(Relay1, OUTPUT); digitalWrite(Relay1, HIGH);
    pinMode(Relay2, OUTPUT); digitalWrite(Relay2, HIGH);
    pinMode(Relay3, OUTPUT); digitalWrite(Relay3, HIGH);
    pinMode(Relay4, OUTPUT); digitalWrite(Relay4, HIGH);

    pinMode(wifiLed, OUTPUT);
    
    pinMode(Switch1, INPUT_PULLUP);
    pinMode(Switch2, INPUT_PULLUP);
    pinMode(Switch3, INPUT_PULLUP);
    pinMode(Switch4, INPUT_PULLUP);
}

void loop() {
    // WiFi Status Check
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi Disconnected!");
        digitalWrite(wifiLed, HIGH); // Turn off WiFi LED
    } else {
        Serial.println("WiFi Connected!");
        digitalWrite(wifiLed, LOW);  // Turn on WiFi LED
    }

    // Read Firebase Data
    readFirebaseData();

    // Check Switches and Update Firebase
    checkSwitch(Switch1, "/L1", load1);
    checkSwitch(Switch2, "/L2", load2);
    checkSwitch(Switch3, "/L3", load3);
    checkSwitch(Switch4, "/L4", load4);

    // Update Relay States
    updateRelays();

    delay(300);
}

// Function to Read Firebase Data
void readFirebaseData() {
    if (Firebase.getInt(firebaseData, "/P1")) {
        Power = firebaseData.intData();
    }
    if (Firebase.getInt(firebaseData, "/L1")) {
        load1 = firebaseData.intData();
    }
    if (Firebase.getInt(firebaseData, "/L2")) {
        load2 = firebaseData.intData();
    }
    if (Firebase.getInt(firebaseData, "/L3")) {
        load3 = firebaseData.intData();
    }
    if (Firebase.getInt(firebaseData, "/L4")) {
        load4 = firebaseData.intData();
    }
}

// Function to Check Switch Inputs and Update Firebase
void checkSwitch(int switchPin, const char* firebasePath, int &load) {
    if (digitalRead(switchPin) == LOW) {
        Power = 0;
        updatePowerStatus();

        load = !load; // Toggle Load
        updateRelays();

        if (Firebase.setInt(firebaseData, firebasePath, load)) {
            Serial.println("Firebase Update SUCCESS: " + String(firebasePath));
        } else {
            Serial.println("Firebase Update FAILED: " + firebaseData.errorReason());
        }
        delay(300); // Debounce
    }
}

// Function to Update Power Status in Firebase
void updatePowerStatus() {
    if (Firebase.setInt(firebaseData, "/P1", Power)) {
        Serial.println("Power Status Updated Successfully!");
    } else {
        Serial.println("Failed to Update Power Status: " + firebaseData.errorReason());
    }
}

// Function to Update Relay States
void updateRelays() {
    if (Power == 0) {
        digitalWrite(Relay1, load1);
        digitalWrite(Relay2, load2);
        digitalWrite(Relay3, load3);
        digitalWrite(Relay4, load4);
    } else {
        digitalWrite(Relay1, HIGH);
        digitalWrite(Relay2, HIGH);
        digitalWrite(Relay3, HIGH);
        digitalWrite(Relay4, HIGH);
    }
}