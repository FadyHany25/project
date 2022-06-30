#include <Wire.h>
#include "Adafruit_MLX90614.h"
#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
//#include <ESPAsyncTCP.h>
//#include <ESPAsyncWebServer.h>
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
#define REPORTING_PERIOD_MS     1000
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
uint32_t tsLastReport = 0;
double temp_amb;
double temp_obj;
double calibration= 8.2;
 
const char ssid[] = "realme 6";                       // Your WiFi credentials.
const char pass[] = "248930a587ed";

ESP8266WebServer server (80);

void setup()
{
  Serial.begin(9600);
   //pinMode(16, OUTPUT); 
  mlx.begin();         //Initialize MLX90614
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initialize with the I2C addr 0x3C (128x64)
  delay(10);
  Serial.println("Connecting to ");
  Serial.println(ssid);

   WiFi.begin(ssid, pass); 
      
   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
 
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
  Serial.println("Temperature Sensor MLX90614");
  display.clearDisplay();
  display.setCursor(25,15);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println(" Thermometer");
  display.setCursor(25,35);
  display.setTextSize(1);
  display.print("Initializing");
  display.display();
  pinMode(16, OUTPUT);
  delay(1500);
}
 
void loop()
{
  //Reading room temperature and object temp
  //for reading Fahrenheit values, use
  //mlx.readAmbientTempF() , mlx.readObjectTempF() )
server.handleClient();
 //mlx.update();
  temp_amb = mlx.readAmbientTempC();
  temp_obj = mlx.readObjectTempC();
  server.handleClient();
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) 
  {
  //Serial Monitor
  Serial.print("Ambient = "); 
  Serial.print(mlx.readAmbientTempC()); 
  Serial.print("*C\tObject = "); 
  Serial.print(mlx.readObjectTempC()+ calibration); 
  Serial.println("*C");
   //tsLastReport = millis();
  
  //Serial.print("Room Temp = ");
  //Serial.println(temp_amb);
  //Serial.print("Object temp = ");
  //Serial.println(temp_obj + calibration);
 
  display.clearDisplay();
  display.setCursor(25,0);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println(" Temperature");
  
  display.setCursor(10,20);
  display.setTextSize(1);
  display.print("Ambient: ");
  display.print(temp_amb);
  display.print((char)247);
  display.print("C");
 
  display.setCursor(10,40);
  display.setTextSize(1);
  display.print("Object: ");
  display.print(temp_obj + calibration);
  display.print((char)247);
  display.print("C");
  
  display.display();
  tsLastReport = millis();
  delay(100);
}
}
void handle_OnConnect() {

  server.send(200, "text/html", SendHTML(temp_amb, temp_obj));
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML(double temp_amb, double temp_obj) {
  String ptr = "<!DOCTYPE html>";
  ptr += "<html>";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>ESP8266 WebServer</title>";
  ptr += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  ptr += "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.7.2/css/all.min.css'>";
  ptr += "<link rel='stylesheet' type='text/css' href='styles.css'>";
  ptr += "<style>";
  ptr += "body { background-color: #fff; font-family: sans-serif; color: #333333; font: 14px Helvetica, sans-serif box-sizing: border-box;}";
  ptr += "#page { margin: 20px; background-color: #fff;}";
  ptr += ".container { height: inherit; padding-bottom: 20px;}";
  ptr += ".header { padding: 20px;}";
  ptr += ".header h1 { padding-bottom: 0.3em; color: #008080; font-size: 45px; font-weight: bold; font-family: Garmond, 'sans-serif'; text-align: center;}";
  ptr += "h2 { padding-bottom: 0.2em; border-bottom: 1px solid #eee; margin: 2px; text-align: left;}";
  ptr += ".header h3 { font-weight: bold; font-family: Arial, 'sans-serif'; font-size: 17px; color: #b6b6b6; text-align: center;}";
  ptr += ".box-full { padding: 20px; border 1px solid #ddd; border-radius: 1em 1em 1em 1em; box-shadow: 1px 7px 7px 1px rgba(0,0,0,0.4); background: #fff; margin: 20px; width: 300px;}";
  ptr += "@media (max-width: 494px) { #page { width: inherit; margin: 5px auto; } #content { padding: 1px;} .box-full { margin: 8px 8px 12px 8px; padding: 10px; width: inherit;; float: none; } }";
  ptr += "@media (min-width: 494px) and (max-width: 980px) { #page { width: 465px; margin 0 auto; } .box-full { width: 380px; } }";
  ptr += "@media (min-width: 980px) { #page { width: 930px; margin: auto; } }";
  ptr += ".sensor { margin: 12px 0px; font-size: 2.5rem;}";
  ptr += ".sensor-labels { font-size: 1rem; vertical-align: middle; padding-bottom: 15px;}";
  ptr += ".units { font-size: 1.2rem;}";
  ptr += "hr { height: 1px; color: #eee; background-color: #eee; border: none;}";
  ptr += "</style>";

  //Ajax Code Start
  ptr += "<script>\n";
  ptr += "setInterval(loadDoc,1000);\n";
  ptr += "function loadDoc() {\n";
  ptr += "var xhttp = new XMLHttpRequest();\n";
  ptr += "xhttp.onreadystatechange = function() {\n";
  ptr += "if (this.readyState == 4 && this.status == 200) {\n";
  ptr += "document.body.innerHTML =this.responseText}\n";
  ptr += "};\n";
  ptr += "xhttp.open(\"GET\", \"/\", true);\n";
  ptr += "xhttp.send();\n";
  ptr += "}\n";
  ptr += "</script>\n";
  //Ajax Code END

  ptr += "</head>";
  ptr += "<body>";
  ptr += "<div id='page'>";
  ptr += "<div class='header'>";
  ptr += "<h1>MLX90614 ESP8266 WebServer</h1>";
  ptr += "</div>";
  ptr += "<div id='content' align='center'>";
  ptr += "<div class='box-full' align='left'>";
  ptr += "<h2>Sensor Readings</h2>";
  ptr += "<div class='sensors-container'>";


  ptr += "<p class='sensor'>";
  ptr += "<i class='fas fa-thermometer-half' style='color:#cc3300'></i>";
  ptr += "<span class='sensor-labels'> temperature object </span>";
  ptr += (int)temp_obj + calibration ;
  ptr += "<sup class='units'>C</sup>";
  ptr += "</p>";
  ptr += "<hr>";

  ptr += "<p class='sensor'>";
  ptr += "<i class='fas fa-thermometer-half' style='color:#f7347a'></i>";
  ptr += "<span class='sensor-labels'> temperature ambient </span>";
  ptr += (int)temp_amb;
  ptr += "<sup class='units'>C</sup>";
  ptr += "</p>";
  ptr += "</div>";
 // ptr += "</div>";
  //ptr += "</div>";
  //ptr += "</div>";
  //ptr += "</div>";
  ptr += "</body>";
  ptr += "</html>";
 return ptr;
 
}
