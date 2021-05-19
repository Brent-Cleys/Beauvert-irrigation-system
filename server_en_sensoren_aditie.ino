#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <EMailSender.h>
#include <WebSocketsServer.h>
#include <espnow.h>
#include <ArduinoJson.h>
#include <Ticker.h> //https://github.com/sstaub/Ticker
#include "Adafruit_seesaw.h"


void send_sensor();
void Sonar();
void Soil();
void batterij_meting();

Ticker timer1;
Ticker timer2;
Ticker timer3;
Ticker timer4;
Adafruit_seesaw ss;

const char* ssid = "Wifi_ssid";
const char* password = "password";

EMailSender emailSend("beauvert.notemcu@gmail.com","qxpglafnojdnkdyu");

uint8_t broadcastAddress[] = {0xEC,0xFA,0xBC,0x6E,0xEB,0x1E};

int tank_email_gestuurd = 0;
int batterij_email_gestuurd = 0;
String datum_tijd = "16/03/2021  0:19";
const long interval = 1000; 
unsigned long previousMillis = 0;
int groenten_bak1;
int groenten_bak2; 
int groenten_bak3;
String email_adress;
// soil sensor, zwart:gnd,rood:3.3V,wit:D2,groen:D1,
int Vochtigheid;
#define echoPin 14 //D5
#define trigPin 12 //D6
long duration; // variable for the duration of sound wave travel
float distance; // variable for the distance measurement
int sensorPin = A0; 
int sensorValue;  
float Niveau;
float tempC;
uint16_t Moist;
int batterij_percentage;

float batterij_metingwaarde[] = {0.025, 0.103, 0.248, 0.356, 0.365, 0.374, 0.488, 0.602, 0.720, 0.838, 0.846, 0.854, 0.862, 0.870, 1.970, 3.693, 4.742, 5.397, 6.112, 7.655, 9.146, 10.362, 10.894, 11.427, 12.433, 13.220, 13.729, 14.269, 15.354, 16.583, 17.368, 18.022, 19.159, 20.474, 20.989, 21.504, 22.165, 22.698, 23.493, 24.535, 25.879, 27.644, 29.407, 31.177, 32.227, 33.386, 34.440, 35.494, 38.871, 42.407, 45.042, 47.398, 50.375, 53.129, 54.744, 55.730, 56.969, 58.311, 59.274, 60.237, 61.713, 62.931, 64.453, 65.524, 65.936, 66.372, 66.521, 66.712, 68.054, 69.276, 69.597, 69.918, 70.479, 71.635, 72.894, 74.151, 75.402, 76.254, 77.004, 77.939, 78.510, 79.090, 79.518, 79.945, 80.761, 81.497, 82.260, 83.072, 83.839, 84.388, 84.684, 85.223, 86.200, 87.170, 87.637, 88.103, 88.809, 89.336, 89.221, 89.782, 90.055, 90.990, 92.021, 92.787, 93.469, 94.355, 94.840, 95.324, 96.131, 97.127, 98.018, 98.947};


typedef struct struct_senddata {
    int bak1;
    int bak2;
    int bak3;
    int voctigheid_waarde;
} struct_senddata;



struct_senddata send_data;

char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <style>
        body{
    background-color: rgb(152, 209, 60);
    text-align: center;
}
h1{
    color: rgb(255, 255, 255);
    font-size: 300%;
    text-align: center;
}
.note{
    color: rgb(80, 170, 255);
}

h2{
    color: rgb(255, 255, 255);
    text-align: center;
}
div{
    float: left;
    width: 49.5%;
    height: 200%;
}
p{
    color: rgb(255, 255, 255);
    text-align: center;
}
label{
    color: rgb(255, 255, 255);
}
#myProgress {
    width: 49.5%;
    background-color: rgb(255, 255, 255);
}
  
#Bar_vochtigheid {
    width: 1%;
    height: 30px;
    background-color: rgb(80, 170, 255);
}
#Bar_tankInhoud {
    width: 1%;
    height: 30px;
    background-color: rgb(80, 170, 255);
}
#witruimte {
    width: 1%;
    height: 30px;
}
#Bar_batterij {
    width: 1%;
    height: 30px;
    background-color: rgb(80, 170, 255);
}

#volgende_regel {
    width: 49.5%;
    height: 30px;
}
#email_tekst{
    color:rgb(255, 255, 255);
    font-size: 175%;
}
#email_adress_input{
    width: 300px;
    height: 25pX;
    font-size: 120%;
    text-align: center;
    color: rgb(0, 118, 228);
    border-color: rgb(0, 118, 228);
}
#submit_knop{
    width: 100px;
    height: 24px;
    background-color: rgb(152, 209, 60);
    color: rgb(255, 255, 255);
    border-color: rgb(0, 118, 228);
    border-radius: 7px;
    border-width: 4px;
}
#div_bak{
    float: left;
    width: 33.3%;
    height: 200%;
}
</style>
<script>
console.log("java is geladen");
var connection = new WebSocket('ws://'+location.hostname+':81/');
var vochtigheid_waarde = 100;
var tank_waarde = 100;
var batterij_percentage = 100;
var Temperatuur = 0.0;
var delayInMilliseconds = 1000;
var email_adress
var bak1
var bak2
var bak3


var Email = { send: function (a) { return new Promise(function (n, e) { a.nocache = Math.floor(1e6 * Math.random() + 1), a.Action = "Send"; var t = JSON.stringify(a); Email.ajaxPost("https://smtpjs.com/v3/smtpjs.aspx?", t, function (e) { n(e) }) }) }, ajaxPost: function (e, n, t) { var a = Email.createCORSRequest("POST", e); a.setRequestHeader("Content-type", "application/x-www-form-urlencoded"), a.onload = function () { var e = a.responseText; null != t && t(e) }, a.send(n) }, ajax: function (e, n) { var t = Email.createCORSRequest("GET", e); t.onload = function () { var e = t.responseText; null != n && n(e) }, t.send() }, createCORSRequest: function (e, n) { var t = new XMLHttpRequest; return "withCredentials" in t ? t.open(e, n, !0) : "undefined" != typeof XDomainRequest ? (t = new XDomainRequest).open(e, n) : t = null, t } };


connection.onmessage = function(event){
  var full_data = event.data;
  console.log(full_data);
  var data = JSON.parse(full_data);
  vochtigheid_waarde = data.vocht;
  tank_waarde = data.tank;
  batterij_percentage = data.batt;
  Temperatuur = data.temp;

  document.getElementById("vochtigheid_procent").innerHTML = vochtigheid_waarde + "%";
  document.getElementById("Bar_vochtigheid").style.width = vochtigheid_waarde + "%";

  document.getElementById("tank_procent").innerHTML = tank_waarde + "%";
  document.getElementById("Bar_tankInhoud").style.width = tank_waarde + "%";
  if(tank_waarde < 20){
    document.getElementById("Bar_tankInhoud").style.backgroundColor = "red";
  }else{
    document.getElementById("Bar_tankInhoud").style.backgroundColor = "#50AAFF";
  }


  document.getElementById("batterij_procent").innerHTML = batterij_percentage + "%";
  document.getElementById("Bar_batterij").style.width = batterij_percentage + "%";
  if(batterij_percentage < 20){
    document.getElementById("Bar_batterij").style.backgroundColor = "red";

  }else{
    document.getElementById("Bar_batterij").style.backgroundColor = "#50AAFF";
  }

 

  document.getElementById("Temperatuur").innerHTML = Temperatuur+"\u00B0"+"C";

}

function send_data()
{
  var send_data = '{"Email_adress" : "'+email_adress+'","bak1":"'+bak1+'","bak2":"'+bak2+'","bak3":"'+bak3+'"}';
  connection.send(send_data);
}



function submit_functie(){
        email_adress = document.getElementById("email_adress_input").value;
        console.log(email_adress);
        sendEmail("Bevesteging email.",email_adress,"Beste mevrouw/meneer.\nDit email adress is toegevoed aan de lijst van emailadressen. \nVriendelijke groeten het BeauVert project.");
        send_data()
}
function submit_bak1(){
        if (document.getElementById("veel_water_bak_1").checked == true){
            bak1 = 2
        } else if (document.getElementById("weinig_water_bak_1").checked == true){
            bak1 = 1
        } else if (document.getElementById("UIT_bak_1").checked == true){
            bak1 = 0
        }
        console.log(bak1);
        send_data()
}
function submit_bak2(){
        if (document.getElementById("veel_water_bak_2").checked == true){
            bak2 = 2
        } else if (document.getElementById("weinig_water_bak_2").checked == true){
            bak2 = 1
        } else if (document.getElementById("UIT_bak_2").checked == true){
            bak2 = 0
        }
        console.log(bak2);
        send_data()
}
function submit_bak3(){

        if (document.getElementById("veel_water_bak_3").checked == true){
            bak3 = 2
        } else if (document.getElementById("weinig_water_bak_3").checked == true){
            bak3 = 1
        } else if (document.getElementById("UIT_bak_3").checked == true){
            bak3 = 0
        }
        console.log(bak3);
        send_data()
}



function sendEmail(subj,email,message){
    Email.send({
        Host: "smtp.gmail.com",
        Username: "beauvert.notemcu@gmail.com",
        Password:"qxpglafnojdnkdyu",
        To: `${email}`,
        From: "beauvert.notemcu@gmail.com",
        Subject: `${subj}`,
        Body: `${message}`
    })
}



    </script>
    <title>BeauVert-InfoWebsite</title> 
</head>

<body>
    <header>
        <h1>B<span class="note">EAU</span>VERT</h1>
    </header>
    <div id="vochtigheid">
        <h2>
            Vochtigheid:
        </h2>
        <p id="vochtigheid_procent"></p>
    </div>
    <div id="witruimte"></div>
    <div>
        <h2>
            Tank inhoud:
        </h2>
        <p id="tank_procent"></p>
    </div>

    <div id="myProgress">
        <div id="Bar_vochtigheid"></div>
    </div>
    <div id="witruimte"></div>
    <div id="myProgress">
        <div id="Bar_tankInhoud"></div>
    </div>

    <div>
        <h2>
            Batterij:
        </h2>
        <p id="batterij_procent"></p>
    </div>
    <div id="witruimte"></div>
    <div>
        <h2>
            Temperatuur:
        </h2>
        <p id="Temperatuur"></p>
    </div>

    <div id="myProgress">
        <div id="Bar_batterij"></div>
    </div>
    <div id="witruimte"></div>
    <div id="volgende_regel"></div>

    <div id="volgende_regel"></div>
    <div id="volgende_regel"></div>
    <div id="div_bak">
        <h2>Moesbak 1</h2>    
        <input type="radio" id="veel_water_bak_1" name="stand" value= 2>
        <label for="veel_water_bak_1">veel water geven</label><br>
        <input type="radio" id="weinig_water_bak_1" name="stand" value=1>
        <label for="weinig_water_bak_1">weinig water geven</label><br>
        <input type="radio" id="UIT_bak_1" name="stand" value=0>
        <label for="UIT_bak_1">Geen water geven</label><br><br>
        <button  id="submit_knop" onclick="submit_bak1()">Submit</button>
    </div>
    <div id="div_bak">
        <h2>Moesbak 2</h2>    
        <input type="radio" id="veel_water_bak_2" name="stand" value= 2>
        <label for="veel_water_bak_2">veel water geven</label><br>
        <input type="radio" id="weinig_water_bak_2" name="stand" value=1>
        <label for="weinig_water_bak_2">weinig water geven</label><br>
        <input type="radio" id="UIT_bak_2" name="stand" value=0>
        <label for="UIT_bak_2">Geen water geven</label><br><br>
        <button  id="submit_knop" onclick="submit_bak2()">Submit</button>
    </div>
    <div id="div_bak">
        <h2>Moesbak 3</h2>    
        <input type="radio" id="veel_water_bak_3" name="stand" value= 2>
        <label for="veel_water_bak_3">veel water geven</label><br>
        <input type="radio" id="weinig_water_bak_3" name="stand" value=1>
        <label for="weinig_water_bak_3">weinig water geven</label><br>
        <input type="radio" id="UIT_bak_3" name="stand" value=0>
        <label for="UIT_bak_3">Geen water geven</label><br><br>
        <button  id="submit_knop" onclick="submit_bak3()">Submit</button>
    </div>

    <div id="volgende_regel"></div>
    <div id="volgende_regel"></div>
    <label for="email_adress" id="email_tekst">Nood Email adress:</label><br>
    <input type="text" id="email_adress_input">
    <br>
    <br>
    <button  id="submit_knop" onclick="submit_functie()">Submit</button>
</body>
</html>
)=====";

#include <ESPAsyncWebServer.h>

AsyncWebServer server(80); // server port 80
WebSocketsServer websockets(81);


void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Page Not found");
}

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = websockets.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      String message = String((char*)( payload));
      Serial.println(message);

      
     DynamicJsonDocument doc(200);
    // deserialize the data
    DeserializationError error = deserializeJson(doc, message);
    // parse the parameters we expect to receive (TO-DO: error handling)
      // Test if parsing succeeds.
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  String email_adress_voorlopig = doc["Email_adress"];
  Serial.print("email adress = ");
  Serial.println(email_adress);
  String bak1 = doc["bak1"];
  Serial.print("bak1 = ");
  Serial.println(bak1);
  String bak2 = doc["bak2"];
  Serial.print("bak2 = ");
  Serial.println(bak2);
  String bak3 = doc["bak3"];
  Serial.print("bak3 = ");
  Serial.println(bak3);
  if(bak1 == "2"){
    groenten_bak1 = 2;
  }
  if(bak2 == "2"){
    groenten_bak2 = 2;
  }
  if(bak3 == "2"){
    groenten_bak3 = 2;
  }
  if(bak1 == "1"){
    groenten_bak1 = 1;
  }
  if(bak2 == "1"){
    groenten_bak2 = 1;
  }
  if(bak3 == "1"){
    groenten_bak3 = 1;
  }
  if(bak1 == "0"){
    groenten_bak1 = 0;
  }
  if(bak2 == "0"){
    groenten_bak2 = 0;
  }
  if(bak3 == "0"){
    groenten_bak3 = 0;
  }
  if(email_adress_voorlopig != "undefined"){
    email_adress = email_adress_voorlopig;
  }
  }
}

void setup(void)
{
  
  Serial.begin(115200);
  
    // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
      // put your setup code here, to run once:
  pinMode(echoPin,INPUT);
  pinMode(trigPin,OUTPUT);
  Serial.begin(115200);
    
  
  if (!ss.begin(0x36)) {
    Serial.println("ERROR! seesaw not found");
    while(1);
  } else {
    Serial.print("seesaw started! version: ");
    Serial.println(ss.getVersion(), HEX);
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  if (!MDNS.begin("esp8266")) {             // Start the mDNS responder for esp8266.local
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");
  
  


  server.on("/", [](AsyncWebServerRequest * request)
  { 
   
  request->send_P(200, "text/html", webpage);
  });

  server.onNotFound(notFound);

  server.begin();  // it will start webserver
  websockets.begin();
  websockets.onEvent(webSocketEvent);
  timer1.attach(2,send_sensor);
  timer2.attach(2,Sonar);
  timer3.attach(2,Soil);
  timer4.attach(2,batterij_meting);

  MDNS.addService("http","tcp",80);

}


void loop(void){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;

    //Set values to send
    send_data.bak1 = groenten_bak1;
    send_data.bak2 = groenten_bak2;
    send_data.bak3 = groenten_bak3;
    send_data.voctigheid_waarde = Vochtigheid;

    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &send_data, sizeof(send_data));
  }

  if(Niveau <= 15 and tank_email_gestuurd == 0){
    Serial.println(Niveau);
    tank_email_gestuurd = 1;
    EMailSender::EMailMessage message;
    message.subject = "Tankwaarde gevaarlijk laag.";
    message.message = "De tank heeft op het moment nog "+String(Niveau)+"%.<br>Bijvullen van de tank is aangeraden.<br>Vriendelijke groeten BeauVert instalatie.";

    EMailSender::Response resp = emailSend.send(email_adress, message);
    Serial.println("Sending status: ");
    Serial.println(resp.status);
    Serial.println(resp.code);
    Serial.println(resp.desc);
  }
  else if(Niveau > 50 and tank_email_gestuurd == 1){
    tank_email_gestuurd = 0;
    Serial.println("tank email gereset");
  }
  MDNS.update();
  websockets.loop();
  
}

void send_sensor()
{
   int sens = digitalRead(D5);
  // JSON_Data = {"temp":t,"hum":h}"
  String JSON_Data = "{\"vocht\":";
         JSON_Data += Vochtigheid;
         JSON_Data += ",\"tank\":";
         JSON_Data += Niveau;
         JSON_Data += ",\"batt\":";
         JSON_Data += batterij_percentage;
         JSON_Data += ",\"temp\":";
         JSON_Data += tempC;
         JSON_Data += "}";
   Serial.println(JSON_Data);     
  websockets.broadcastTXT(JSON_Data);
}

void Sonar(){
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  if ((1-(distance-5)/17)*100 >= 0){
    Niveau = (1-(distance-5)/17)*100;
  }
Serial.print(Niveau); Serial.println("%");
}

void Soil() {
  tempC = ss.getTemp();
  Moist = ss.touchRead(0);
  Serial.print("Temperature: "); 
  Serial.print(tempC); 
  Serial.println("*C");
  Serial.print("Vocht: "); 
  Serial.println(Moist);
  Vochtigheid = Moist*100/1024;
}

void batterij_meting(){
  sensorValue = analogRead(sensorPin);

  Serial.println("sensorValue:");
  Serial.println(sensorValue);

  if(sensorValue >= 913){
    batterij_percentage = batterij_metingwaarde[sensorValue-913];
  }else{
    batterij_percentage = 0;
  }
  Serial.println("percentage:");
  Serial.println(batterij_percentage);
}
