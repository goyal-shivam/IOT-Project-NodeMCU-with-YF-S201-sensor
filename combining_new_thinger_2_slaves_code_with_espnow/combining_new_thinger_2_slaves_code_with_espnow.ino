/*
  This code is obtained on combining thinger.io code that
  supports uploading 4 readings with espnow protocol rcvr code
*/
#include <ThingerESP8266.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#define USERNAME "mavilog"
#define DEVICE_ID "NodeMCU"
#define DEVICE_CREDENTIAL "zBY4$20U+D8_z8n7"
#define SSID "Starfire"
#define SSID_PASSWORD "12345678"
#define SENSOR  D3

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
//boolean ledState = LOW;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;

unsigned int flowMilliLitres;

float flowRate1;
unsigned long totalMilliLitres1;
float flowRate2;
unsigned long totalMilliLitres2;



typedef struct struct_message {
    int id;
    float flowRate;
    unsigned long totalMilliLitres;
} struct_message;


struct_message myData;
struct_message boardsStruct[2];

void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

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
  Serial.printf("flowRate value: %f \n", boardsStruct[myData.id-1].flowRate);
  Serial.printf("totalMilliLitres value: %lu \n", boardsStruct[myData.id-1].totalMilliLitres);
  Serial.println();

  if(myData.id == 1){
    flowRate1 = myData.flowRate;
    totalMilliLitres1 = myData.totalMilliLitres;
  }
  else if(myData.id == 2){
    flowRate2 = myData.flowRate;
    totalMilliLitres2 = myData.totalMilliLitres;
  }

/*
    thing["data"] >> [](pson& out){
      out["Flow Rate 1"] = flowRate1;
      out["Total 1"]= totalMilliLitres1;
      out["Flow Rate 2"] = flowRate2;
      out["Total 2"]= totalMilliLitres2;
    };
    thing.handle();
    thing.stream(thing["data"]);
*/
}


void setup()
{
  Serial.begin(115200);
  thing.add_wifi(SSID, SSID_PASSWORD);


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
}
void loop()
{
//  currentMillis = millis();
//  if (currentMillis - previousMillis > interval) {
//    pulse1Sec = pulseCount;
//    pulseCount = 0;
//    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
//    previousMillis = millis();
//    flowMilliLitres = (flowRate / 60) * 1000;
//    totalMilliLitres += flowMilliLitres;
//    // Print the flow rate for this second in litres / minute
//    Serial.print("Flow rate: ");
//    Serial.print(int(flowRate));  // Print the integer part of the variable
//    Serial.print("L/min");
//    Serial.print("\t");       // Print tab space
//    // Print the cumulative total of litres flowed since starting
//    Serial.print("Output Liquid Quantity: ");
//    Serial.print(totalMilliLitres);
//    Serial.print("mL / ");
//    Serial.print(totalMilliLitres / 1000);
//    Serial.println("L");
//}

    thing["data"] >> [](pson& out){
      out["Flow Rate 1"] = flowRate1;
      out["Total 1"]= totalMilliLitres1;
      out["Flow Rate 2"] = flowRate2;
      out["Total 2"]= totalMilliLitres2;
    };
    thing.handle();
    thing.stream(thing["data"]);

    delay(100);

/*
    if(flowRate1 < 35){
      flowRate1 += 10;
    }
    else{
      flowRate1 = 10;
    }

    if(totalMilliLitres1 < 75){
      totalMilliLitres1 += 10;
    }
    else{
      totalMilliLitres1 = 50;
    }

    flowRate2 = flowRate1 + 100;
    totalMilliLitres2 = totalMilliLitres1 + 100;

    delay(2000);
    */
}
