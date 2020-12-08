#include <ESP8266WiFi.h>
#include <NewPing.h> 

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
#define SERVER_IP "http://144.202.34.148:3333"
#define STASSID "TP-Link_7150"
#define STAPSK  "20860332"

String estadoAnterior;


/*Aqui se configuran los pines donde debemos conectar el sensor*/
#define TRIGGER_PIN  2
#define ECHO_PIN     16
#define MAX_DISTANCE 200

float distancia = 0;
int luz = 0;

/*Crear el objeto de la clase NewPing*/
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
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

void loop() {
  ultrasonico();
  fotoresistencia();
  String estado = "";
  String json;
  int switchVal1=0;
  int switchVal2=0;  


  //Lee el valor de cada interruptor
  switchVal1 = digitalRead(switchPin1);
  switchVal2 = digitalRead(switchPin2);


  if (switchVal1 == HIGH && switchVal2 == HIGH){
    estado = "Led Apagado";
    digitalWrite(led1Pin,LOW);
    delay(500);
  }

  if (switchVal1 == LOW && switchVal2 == LOW){
    estado = "Led Apagado";
    digitalWrite(led1Pin,LOW);
    delay(500);
  }

  if (switchVal1 == HIGH && switchVal2 == LOW){
    estado = "Led Encendido";
    digitalWrite(led1Pin,HIGH);
    delay(500);
  }

  if (switchVal1 == LOW && switchVal2 == HIGH){
    estado = "Led Encendido";
    digitalWrite(led1Pin,HIGH);
    delay(500);
  }
 
 //En esta condicion se evalua si ya existe un valor anterior igual al que se esta enviando
 //Si ya existe uno no entra a enviar el POST
 if (estadoAnterior != estado){
    Serial.println("==== El estado ha cambiado ======");
    Serial.print("Distancia: ");
    Serial.println(distancia);
    Serial.print("Luz: ");
    Serial.println(luz);
    Serial.println("");
   //Se asigna el nuevo valor anterior
    estadoAnterior = estado;
    if ((WiFi.status() == WL_CONNECTED)) {
     WiFiClient client;
     HTTPClient http;
     http.begin(SERVER_IP "/guardarData");
     Serial.print("[HTTP] begin...\n");
     http.addHeader("Content-Type", "application/json");
     

     StaticJsonBuffer<200> jsonBuffer;
     char json[256];
     JsonObject& root = jsonBuffer.createObject();
     root["estado"] = estado;
     root["distancia"]= distancia;
     root["luz"] = luz;
     root.printTo(json, sizeof(json));
     Serial.println(json);
     
     int httpCode = http.POST(json);

     if(httpCode > 0) {
       // HTTP header has been send and Server response header has been handled
       Serial.printf("[HTTP] Codigo del POST: %d\n", httpCode);

       // file found at server
       if (httpCode > 1) {
         const String& payload = http.getString();
         Serial.print("Respuesta:");
         Serial.println(payload);
       }
     } else {
       Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
     }
     http.end();
   }
 }
 Serial.println("El estado no ha cambiado");
  
  delay(1000);
  
}

void ultrasonico(){
  // Esperar 1 segundo entre mediciones
  delay(1000);
  // Obtener medicion de tiempo de viaje del sonido y guardar en variable uS
  float uS = sonar.ping_median();
  // Imprimir la distancia medida a la consola serial
  Serial.print("Distancia: ");
  // Calcular la distancia con base en una constante
  Serial.print(uS / US_ROUNDTRIP_CM);
  Serial.println("cm");
  distancia=  uS / US_ROUNDTRIP_CM;
}

void fotoresistencia(){
  int sensorValue = analogRead(A0);   // read the input on analog pin 0
  luz = sensorValue;
  Serial.print("Luz: ");
  Serial.println(luz);
  delay(1000);

}
