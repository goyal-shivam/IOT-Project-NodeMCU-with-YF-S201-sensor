#include <ThingerESP8266.h>
#include <ESP8266WiFi.h>
#include <espnow.h>


#define USERNAME "mavilog"
#define DEVICE_ID "NodeMCU"
#define DEVICE_CREDENTIAL "zBY4$20U+D8_z8n7"
#define SSID "Starfire"
#define SSID_PASSWORD "12345678"



//#define DEVICE_ID2 "NodeMCU2"
//#define DEVICE_CREDENTIAL2 "iCN%ULwzhw%hi#po"



ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);
float flowRate;
unsigned long totalMilliLitres;



// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    int id;
    float flowRate;
    unsigned long totalMilliLitres;
} struct_message;


// Create a struct_message called myData
struct_message myData;

//// Create a structure to hold the readings from each board
//struct_message board1;
//struct_message board2;
//
//// Create an array with all the structures
//struct_message boardsStruct[2] = {board1, board2};

struct_message boardsStruct[2];

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id-1].flowRate = myData.flowRate;
  boardsStruct[myData.id-1].totalMilliLitres = myData.totalMilliLitres;
  Serial.printf("flowRate value: %d \n", boardsStruct[myData.id-1].flowRate);
  Serial.printf("totalMilliLitres value: %d \n", boardsStruct[myData.id-1].totalMilliLitres);
  Serial.println();

  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(100);                       // wait for a second
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second

  if(myData.id == 1){
    thing["data"] >> [](pson& out){
      out["Flow Rate 1"] = myData.flowRate;
      out["Total 1"]= myData.totalMilliLitres;
    };
    thing.handle();
    thing.stream(thing["data"]);
  }
  else if(myData.id == 2){
    thing["data"] >> [](pson& out){
      out["Flow Rate 2"] = myData.flowRate;
      out["Total 2"]= myData.totalMilliLitres;
    };
    thing.handle();
    thing.stream(thing["data"]);
  }

  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(100);                       // wait for a second
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  thing.add_wifi(SSID, SSID_PASSWORD);

  flowRate = 10.0;
  totalMilliLitres = 50;
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop(){


}
