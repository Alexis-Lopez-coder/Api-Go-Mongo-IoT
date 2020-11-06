#include <ESP8266WiFi.h> 

#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

int switchPin1 = 14; //D5
int switchPin2 = 12; //D6 
int led1Pin =  5; //D1


//Conectar a WiFi
#define SERVER_IP "192.168.122.1:8080"
#define STASSID "TP-Link_7150"
#define STAPSK  "20860332"

String estadoAnterior;

 
void setup()
{
  Serial.begin(9600);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  Serial.println("Conectando a: ");
  Serial.println(STASSID);

  // Esperar a que nos conectemos
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(200);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado a: ");
  Serial.println(WiFi.SSID()); 
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  
  // Set up the switch pins to be an input:
  pinMode(switchPin1, INPUT);
  pinMode(switchPin2, INPUT);

  // Set up the LED pins to be an output:
  pinMode(led1Pin,OUTPUT);
}

void loop()
{

  String estado = "";
  String json;
   // variables to hold the switch state
  int switchVal1=0;
  int switchVal2=0;  


  //Lee el valor de cada interruptor
  switchVal1 = digitalRead(switchPin1);
  switchVal2 = digitalRead(switchPin2);


  if (switchVal1 == HIGH && switchVal2 == HIGH){
    Serial.println("Apagado--> Enviar valor");
    estado = "Led Apagado";
    digitalWrite(led1Pin,LOW);
    delay(500);
  }

  if (switchVal1 == LOW && switchVal2 == LOW){
    Serial.println("Apagado--> Enviar valor");
    estado = "Led Apagado";
    digitalWrite(led1Pin,LOW);
    delay(500);
  }

  if (switchVal1 == HIGH && switchVal2 == LOW){
    Serial.println("Encendido--> Enviar valor");
    estado = "Led Encendido";
    digitalWrite(led1Pin,HIGH);
    delay(500);
  }

  if (switchVal1 == LOW && switchVal2 == HIGH){
    Serial.println("Encendido--> Enviar valor");
    estado = "Led Encendido";
    digitalWrite(led1Pin,HIGH);
    delay(500);
  }
 
 //En esta condicion se evalua si ya existe un valor anterior igual al que se esta enviando
 //Si ya existe uno no entra a enviar el POST
 if (estadoAnterior != estado){
    Serial.println("El estado ha cambiado");
   //Se asigna el nuevo valor anterior
    estadoAnterior = estado;
    if ((WiFi.status() == WL_CONNECTED)) {
     WiFiClient client;
     HTTPClient http;

     Serial.print("[HTTP] begin...\n");



     http.begin("http://" SERVER_IP "/guardarData"); //HTTP
     http.addHeader("Content-Type", "application/json");

     //JSON
     StaticJsonBuffer<200> jsonBuffer;
     char json[256];
     JsonObject& root = jsonBuffer.createObject();
     root["estado"] = estado;
     root.printTo(json, sizeof(json));
     Serial.println(json);
     //enviar paquete
     int httpCode = http.POST(json);   //Send the request
     //http.end();

     /*StaticJsonDocument<200> doc;
     // Add values in the document
     doc["estado"] = estado;*/

     /*serializeJson(doc, json);
     Serial.print("Valor JSON---- ");
     Serial.println(json);
     // start connection and send HTTP header and body
     int httpCode = http.POST(json);*/

     if(httpCode > 0) {
       // HTTP header has been send and Server response header has been handled
       Serial.printf("[HTTP] Codigo del POST: %d\n", httpCode);

       // file found at server
       if (httpCode == HTTP_CODE_CREATED) {
         const String& payload = http.getString();
         Serial.println("received payload:\n<<");
         Serial.println(payload);
         Serial.println(">>");
       }
     } else {
       //Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
     }
     http.end();
   }
 }
 Serial.println("El estado no ha cambiado. No se envía");
  
  delay(1000);


  
 
}
