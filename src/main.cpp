#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
 
ESP8266WiFiMulti wifiMulti;

// 用于响应HTTP请求,监听80端口
ESP8266WebServer esp8266_server(80);

// 连接控制继电器的针脚
const int RELAY = 5;
bool flag = 0;


void handleRoot() {       
  esp8266_server.send(
    200, 
    "text/html", 
    " <!DOCTYPE html>\
      <html>\
      <head>\
        <meta charset=\"utf-8\">\
        <meta content=\"width=device-width,initial-scale=1.0,maximum-scale=1.0,user-scalable=no\" name=\"viewport\">\
        <meta content=\"yes\" name=\"apple-mobile-web-app-capable\">\
        <meta content=\"black\" name=\"apple-mobile-web-app-status-bar-style\">\
        <meta content=\"telephone=no\" name=\"format-detection\">\
        <meta content=\"email=no\" name=\"format-detection\">\
        <title>WIFI开关</title>\
        <link rel=\"stylesheet\" href=\"index.css\">\
      </head>\
        <body>\
          <div style=\"padding:10px\">\
            <form action=\"/relay\" method=\"POST\"><input style=\"height:60px;width:100%\" type=\"submit\" value=\"Toggle\"></form>\
          </div>\
        </body>\
      </html>"
  );
}

// 控制继电器开闭
void handleRelay() {
  // 点亮或熄灭ESP8266板上的LED
  digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
  
  // 通过高低电平控制GPIO5上的继电器
  Serial.print("flag:\t");
  Serial.println(flag);
  if (!flag){
    digitalWrite(RELAY,HIGH);
    Serial.println("output HIGH");
  }
  else{
    digitalWrite(RELAY,LOW);
    Serial.println("output LOW");
  }
  flag = !flag;
  
  esp8266_server.sendHeader("Location","/");
  esp8266_server.send(303);
}
 
// 设置处理404情况的函数'handleNotFound'
void handleNotFound(){
  esp8266_server.send(404, "text/plain", "404 Not found");
}
 
void setup(void){
  Serial.begin(115200);

  // 设置多个可连接的wifi账号密码，进行循环连接，连接成功后结束循环
  wifiMulti.addAP("Center_5L", "qzzg_in_6413");
  wifiMulti.addAP("wifi1", "xxxxxxxx");
  wifiMulti.addAP("wifi2", "xxxxxxxx");

  Serial.println("Connecting ...");
  
  int i = 0;                                 
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(1000);
    Serial.print(i++); 
    Serial.print(' ');
  }

  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
 
  esp8266_server.begin();
  esp8266_server.on("/", HTTP_GET, handleRoot);
  esp8266_server.on("/relay", HTTP_POST, handleRelay);
  esp8266_server.onNotFound(handleNotFound);
 
  Serial.println("HTTP esp8266_server started");

  // 设置连接继电器的GPIO5为输出模式
  pinMode(RELAY,OUTPUT);
  // 设置esp8266板上的LED引脚为输出模式以便控制LED, 作为参照(可以不用)
  pinMode(LED_BUILTIN, OUTPUT); 
}
 
void loop(void){
  esp8266_server.handleClient();
}
