#include <ESP8266WiFi.h>
#include <espnow.h>

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x84, 0xCC, 0xA8, 0x88, 0x74, 0x09};

// Set your Board ID (ESP32 Sender #1 = BOARD_ID 1, ESP32 Sender #2 = BOARD_ID 2, etc)
#define BOARD_ID 2


long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
float flowRate;
unsigned long totalMilliLitres;



// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    int id;
    float flowRate;
    unsigned long totalMilliLitres;
} struct_message;

// Create a struct_message called test to store variables to be sent
struct_message myData;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
//    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//    delay(100);                       // wait for a second
//    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  }
  else{
    Serial.println("Delivery fail");
//    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//    delay(100);                       // wait for a second
//    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//    delay(100);                       // wait for a second
//    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//    delay(100);                       // wait for a second
//    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//    delay(1000);
  }
}

void setup() {
//  pinMode(LED_BUILTIN, OUTPUT);
  
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } 
  // Set ESP-NOW role
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  // Once ESPNow is successfully init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  // Setup NodeMCU for sensor readings
  flowRate = 10.0;
  totalMilliLitres = 50;
  previousMillis = 0;
}
 
void loop() {
  currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    
    if(flowRate < 35.0){
      flowRate += 10.0;
    }
    else{
      flowRate = 10.0;
    }
    
    previousMillis = millis();
    
    if(totalMilliLitres < 75){
      totalMilliLitres += 10;
    }
    else{
      totalMilliLitres = 50;
    }
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\t");       // Print tab space
    // Print the cumulative total of litres flowed since starting
    Serial.print("Output Liquid Quantity: ");
    Serial.print(totalMilliLitres);
    Serial.print("mL / ");
    Serial.print(totalMilliLitres / 1000);
    Serial.println("L");
    

    // Set values to send
    myData.id = BOARD_ID;
    myData.flowRate = flowRate;
    myData.totalMilliLitres = totalMilliLitres;

    // Send message via ESP-NOW
    esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
  }
}
