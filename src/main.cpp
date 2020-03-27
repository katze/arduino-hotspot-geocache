#include <Arduino.h>
#include <DNSServer.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include "ESPAsyncWebServer.h"


#include <FS.h>

DNSServer dnsServer;
AsyncWebServer server(80);

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
     request->send(SPIFFS, "/index.html", "text/html");
  }
};


void setup(){
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  //----------------------------------------------------SPIFFS
  if(!SPIFFS.begin())
  {
    Serial.println("Erreur SPIFFS...");
    return;
  }

  //your other setup stuff...
  WiFi.softAP("GCXXXXX 📦");
  dnsServer.start(53, "*", WiFi.softAPIP());


  server.on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/w3.css", "text/css");
  });

  server.on("/app.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/app.js", "text/javascript");
  });

  server.on("/checkAnswer", HTTP_GET, [](AsyncWebServerRequest *request){
    String answer;
    if (request->hasParam("answer")) {
        answer = request->getParam("answer")->value();
    } else {
        answer = "No message sent";
        request->send(400, "application/json", "{\"text\":\"Rien recu\"}");
    }



    Serial.print("réponse : ");
    Serial.println(answer);

    if(answer == "blanc"){
      digitalWrite(LED_BUILTIN, LOW);
      request->send(200, "application/json", "{\"message\":\"Bravo\"}");
    }else{
      digitalWrite(LED_BUILTIN, HIGH);
      request->send(200, "application/json", "{\"message\":\"Non, mauvaise réponse\"}");
    }

   


    request->send(200);
  });


  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  server.begin();
}

void loop(){
  dnsServer.processNextRequest();
}