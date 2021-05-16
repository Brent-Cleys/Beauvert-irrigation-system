#include <ESP8266WiFi.h>
#include <espnow.h>


uint8_t broadcastAddress[] = {0x24, 0x62, 0xAB, 0x0A, 0xF5, 0x01};

uint8_t wire1_moter1 = 5; //D1
uint8_t wire2_moter1 = 4; //D2
uint8_t wire3_moter1 = 0; //D3
uint8_t wire4_moter1 = 2; //D4

uint8_t wire1_moter2 = 14; //D5
uint8_t wire2_moter2 = 12; //D6
uint8_t wire3_moter2 = 13; //D7
uint8_t wire4_moter2 = 15; //D8

const uint16_t _delay = 5; /* delay in between two steps. minimum delay more the rotational speed */

const long bak_delay = 50000;
const long pomp_delay = 5000;

const int pompPin =  16; //D0
int VochtNood_weinig = 35;
int VochtNood_veel = 55;
int ventiel_1 = 0;
int ventiel_2 = 0;
int pomp_value = 0;


int bak1 = 1;
int bak2 = 1;
int bak3 = 1;
int vochtigheid_waarde;

unsigned long pompMillis;
unsigned long bak1_millis;
unsigned long bak2_millis;
unsigned long bak3_millis;

// Updates DHT readings every 10 seconds
const long interval = 10000; 
unsigned long previousMillis = 0;    // will store last time DHT was updated 
// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_recievedata {
    int bak1;
    int bak2;
    int bak3;
    int vochtigheid_waarde;
} struct_struct_recievedata;

// Create a struct_message called DHTReadings to hold sensor readings

/*
typedef struct struct_senddata {
    int tank_waarde;
    int voctigheid_waarde;
    int batterij_waarde;
} struct_senddata;

struct_senddata send_data;
*/
struct_recievedata incomingReadings;

/*
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}
*/

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  bak1 = incomingReadings.bak1;
  bak2 = incomingReadings.bak2;
  bak3 = incomingReadings.bak3;
  vochtigheid_waarde = incomingReadings.vochtigheid_waarde;
  Serial.println("INCOMING READINGS");
  Serial.print("bak1: ");
  Serial.println(bak1);
  Serial.print("bak2: ");
  Serial.println(bak2);
  Serial.print("bak3: ");
  Serial.println(bak3);
  Serial.print("voctigheid_waarde: ");
  Serial.println(vochtigheid_waarde);
  Serial.println();
}

void sequence_moter1(bool a, bool b, bool c, bool d){  /* four step sequence_moter1 to stepper motor */
  digitalWrite(wire1_moter1, a);
  digitalWrite(wire2_moter1, b);
  digitalWrite(wire3_moter1, c);
  digitalWrite(wire4_moter1, d);
  delay(_delay);
}
void sequence_moter2(bool a, bool b, bool c, bool d){  /* four step sequence_moter1 to stepper motor */
  digitalWrite(wire1_moter2, a);
  digitalWrite(wire2_moter2, b);
  digitalWrite(wire3_moter2, c);
  digitalWrite(wire4_moter2, d);
  delay(_delay);
}


void setup() {
    pinMode(pompPin, OUTPUT);
    pinMode(wire1_moter1, OUTPUT); /* set four wires as output */
    pinMode(wire2_moter1, OUTPUT);
    pinMode(wire3_moter1, OUTPUT);
    pinMode(wire4_moter1, OUTPUT);
    
    pinMode(wire1_moter2, OUTPUT); /* set four wires as output */
    pinMode(wire2_moter2, OUTPUT);
    pinMode(wire3_moter2, OUTPUT);
    pinMode(wire4_moter2, OUTPUT);
    Serial.begin(115200);
    
    
    WiFi.mode(WIFI_STA);
    // Init ESP-NOW
    if (esp_now_init() != 0) {
      Serial.println("Error initializing ESP-NOW");
      return;
    }
    // Set ESP-NOW Role
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  
    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(OnDataRecv);
    
}

void moter1_240(){
    for(int i = 0; i<427; i++)
  {
    sequence_moter1(HIGH, LOW, LOW, LOW);
    sequence_moter1(HIGH, HIGH, LOW, LOW);
    sequence_moter1(LOW, HIGH, LOW, LOW);
    sequence_moter1(LOW, HIGH, HIGH, LOW);
    sequence_moter1(LOW, LOW, HIGH, LOW);
    sequence_moter1(LOW, LOW, HIGH, HIGH);
    sequence_moter1(LOW, LOW, LOW, HIGH);
    sequence_moter1(HIGH, LOW, LOW, HIGH);
    //Serial.print("moter1  ");
    //Serial.println(i);
  }
  sequence_moter1(HIGH, LOW, LOW, LOW);
}

void moter1_480(){
    for(int i = 0; i<853; i++)
  {
    sequence_moter1(HIGH, LOW, LOW, LOW);
    sequence_moter1(HIGH, HIGH, LOW, LOW);
    sequence_moter1(LOW, HIGH, LOW, LOW);
    sequence_moter1(LOW, HIGH, HIGH, LOW);
    sequence_moter1(LOW, LOW, HIGH, LOW);
    sequence_moter1(LOW, LOW, HIGH, HIGH);
    sequence_moter1(LOW, LOW, LOW, HIGH);
    sequence_moter1(HIGH, LOW, LOW, HIGH);
    //Serial.print("moter1  ");
    //Serial.println(i);
  }
  sequence_moter1(HIGH, LOW, LOW, LOW);
}

void moter2_240(){
    for(int i = 0; i<427; i++)
  {
    sequence_moter2(HIGH, LOW, LOW, LOW);
    sequence_moter2(HIGH, HIGH, LOW, LOW);
    sequence_moter2(LOW, HIGH, LOW, LOW);
    sequence_moter2(LOW, HIGH, HIGH, LOW);
    sequence_moter2(LOW, LOW, HIGH, LOW);
    sequence_moter2(LOW, LOW, HIGH, HIGH);
    sequence_moter2(LOW, LOW, LOW, HIGH);
    sequence_moter2(HIGH, LOW, LOW, HIGH);
    //Serial.print("moter2  ");
    //Serial.println(i);
  }
  sequence_moter2(HIGH, LOW, LOW, LOW);
}


void moter2_480(){
    for(int i = 0; i<853; i++)
  {
    sequence_moter2(HIGH, LOW, LOW, LOW);
    sequence_moter2(HIGH, HIGH, LOW, LOW);
    sequence_moter2(LOW, HIGH, LOW, LOW);
    sequence_moter2(LOW, HIGH, HIGH, LOW);
    sequence_moter2(LOW, LOW, HIGH, LOW);
    sequence_moter2(LOW, LOW, HIGH, HIGH);
    sequence_moter2(LOW, LOW, LOW, HIGH);
    sequence_moter2(HIGH, LOW, LOW, HIGH);
    //Serial.print("moter2  ");
    //Serial.println(i);
  }
  sequence_moter2(HIGH, LOW, LOW, LOW);
}


void loop() {
      if (bak1_millis <= millis() and (bak1 == 1 and vochtigheid_waarde <= VochtNood_weinig or bak1 == 2 and vochtigheid_waarde <= VochtNood_veel)){
        Serial.println("bak1 siclus gestart");
        if (ventiel_1 == 1){
          moter1_480();
          ventiel_1 = 0;
        }
        if (ventiel_2 == 1){
          moter2_480();
          ventiel_2 = 0;
        }
        bak1_millis = millis()+bak_delay;
        for (int k = 100; k<255; k++){
          analogWrite(pompPin,k);
          delay(10);
        }
        for (int k = 255; k>100; k--){
          analogWrite(pompPin,k);
          delay(10);
        }
        analogWrite(pompPin,0);
      }
      
      else if (bak2_millis <= millis() and (bak2 == 1 and vochtigheid_waarde <= VochtNood_weinig or bak2 == 2 and vochtigheid_waarde <= VochtNood_veel)){
        Serial.println("bak2 siclus gestart");
        if (ventiel_1 == 1){
          moter1_480();
          ventiel_1 = 0;
        }
        if (ventiel_2 == 0){
          moter2_240();
          ventiel_2 = 1;
        }
        bak2_millis = millis()+bak_delay;
        for (int k = 100; k<255; k++){
          analogWrite(pompPin,k);
          delay(10);
        }
        for (int k = 255; k>100; k--){
          analogWrite(pompPin,k);
          delay(10);
        }
        analogWrite(pompPin,0);
      }
      
      else if (bak3_millis <= millis() and (bak3 == 1 and vochtigheid_waarde <= VochtNood_weinig or bak3 == 2 and vochtigheid_waarde <= VochtNood_veel)){
        Serial.println("bak3 siclus gestart");
        if (ventiel_1 == 0){
          moter1_240();
          ventiel_1 = 1;
        }
        bak3_millis = millis()+bak_delay;
        for (int k = 100; k<255; k++){
          analogWrite(pompPin,k);
          delay(10);
        }
        for (int k = 255; k>100; k--){
          analogWrite(pompPin,k);
          delay(10);
        }
        analogWrite(pompPin,0);

      }


    /*   
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      // save the last time you updated the DHT values
      previousMillis = currentMillis;

      //Set values to send
      send_data.tank_waarde = Niveau;
      send_data.voctigheid_waarde = Vochtigheid*100;
      send_data.batterij_waarde = 13;

      // Send message via ESP-NOW
      esp_now_send(broadcastAddress, (uint8_t *) &send_data, sizeof(send_data));
  }
  */
}
