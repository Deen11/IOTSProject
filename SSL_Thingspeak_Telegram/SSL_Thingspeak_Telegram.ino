#include <WiFi.h>
#include <HTTPClient.h>
#include <ThingSpeak.h>
#include <WiFiClientSecure.h> 
#include <LiquidCrystal.h>
#include <analogWrite.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h> // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ESP32Servo.h>

const char* ssid = "iNokia breakable";
const char* password = "Syuriken123";
const char BotToken[] = "1464265844:AAHcL8_HeZmsE5wmKFSWCugKV-XW2DQutFQ"; 
#define BOTtoken "1464265844:AAHcL8_HeZmsE5wmKFSWCugKV-XW2DQutFQ" // your Bot Token (Get from Botfather)
#define CHAT_ID "432162617"
const int ledPin = 4;

WiFiClientSecure client;

UniversalTelegramBot bot(BOTtoken, client);

// Servo Pin
int servoPin = 5;
Servo myservo;
int pos = 0;

// Signal Pin
int signalPin = 18;


// Temperature Sensor
const int sensor= 34; 
// IR Sensor
int sensorPin = 35; // select the input pin
int sensorValue = 0; // variable for reading the pin status

String tempc; //variable to store temperature in degree Celsius
int check; //variable to check temperature for servo open/close
double vout; //temporary variable to hold sensor reading

String permission;

const char* url = "https://api.thingspeak.com/update";
const char* fingerprint = "271892dda426c30709b97ae6c521b95b48f716e1";
unsigned long channelID = 1289809;
String writeAPIKey = "TEZZMVPMAF55O1VY";

void setup() {
  // put your setup code here, to run once:
 
  pinMode (ledPin, OUTPUT);
   digitalWrite (ledPin, LOW);  // turn off the LED

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 500, 2400);
  pinMode(servoPin, OUTPUT);
  pinMode(signalPin, OUTPUT);
  
  unsigned short count = 0;
  delay(2000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ... ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    count++;
    Serial.print(String(count));
    if (count >= 30)
      WifiRestart();
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  ThingSpeak.begin(client);
}

void WifiRestart() {
  Serial.println("Turning WiFi off ...");
  WiFi.mode(WIFI_OFF);
  Serial.println("Sleeping for 5 seconds ...");
  delay(5000);
  Serial.println("Trying to reconnect to WiFi ...");
  WiFi.mode(WIFI_STA);
}

void writeData(String tempc, int sensorValue, int check) {
  int response_code = 0;
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    Serial.print("[HTTPS] begin ... \n");
    http.begin(url,fingerprint);
    http.addHeader("Content-Type","application/x-www-form-urlencoded");
    String httpRequestData = "api_key=" + writeAPIKey + "&field1=" + String(tempc)+ "&field2=" + int(sensorValue);
    int httpCode = http.POST(httpRequestData);
    Serial.print("HTTP Response code: ");
    Serial.println(httpCode);
    if (httpCode == 200 ) {
    Serial.println("Temperature: " + String(tempc));
    if ((check)>36){
    bot.sendMessage(CHAT_ID, "Person has Fever: "+ tempc + ".C ⬇", "");
  digitalWrite (ledPin, HIGH);  // turn on the LED
  delay(5000);  // wait for half a second or 500 milliseconds
  digitalWrite (ledPin, LOW);  // turn off the LED
    }
    else{
    bot.sendMessage(CHAT_ID, "Acceptable Temperature: " + tempc + ".C ⬇", "");
    }

    }
    if (httpCode != 200){
      Serial.println("Data upload failed ...... ");
      Serial.println("Problem updating channel. HTTP error code " + String(httpCode));
    }
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}

void loop() {
   digitalWrite (ledPin, LOW);  // turn off the LED

   sensorValue = analogRead (sensorPin);
   delay(3000);
   if(sensorValue <= 100) 
   { // check if the input is HIGH
      readwritetemp(sensorValue);
      delay(5000);
      opendoor(check);
      return;
   }
    
   else 
   { 
    Serial.print("No one detected");
    Serial.println();
    digitalWrite(signalPin, 0);
    return;
   } 
  
}

void readwritetemp(int sensorValue){
  vout=analogRead(sensor); //Reading the value from sensor
  vout=(vout/2048)*3300;
  tempc = vout*0.1;
  check = vout*0.1;
  Serial.println();
  writeData(tempc,sensorValue,check);
  digitalWrite(signalPin, 1);
  delay(5000);
}
void opendoor(int check){
  if(check > 36.0){
    permission = "High Temp!";

  }
  else
  {
    permission = "Acceptable Temp";
    for (pos = 0; pos <= 180; pos += 1) {
    myservo.write(pos);    // tell servo to go to position in variable 'pos'
    delay(40);
    }
  }
}

  
  
