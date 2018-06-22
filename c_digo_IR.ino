
#include <ESP8266WiFi.h>

const char* ssid = "SSID";
const char* password = "password";

// Instacio el Server seteado para que escuche por el puerto 301

IPAddress ip(192, 168, 0, 109); //set static ip
IPAddress gateway(192, 168, 0, 1); //set getteway
IPAddress subnet(255, 255, 255, 0);//set subnet
WiFiServer server(301);//Port 301 to access server
 
void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO
  pinMode(14, OUTPUT);               //Controlador A=D5=GP 14
  digitalWrite(14, 0);               
  pinMode(12, OUTPUT);               //Controlador B=D6=GP 12
  digitalWrite(12, 0);              
  pinMode(13, OUTPUT);               //Controlador INH=D7=GP 13
  digitalWrite(13, 1);              
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Se conectó alguien?
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Hay alguien conectado!
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Leo el pedido
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Evalúo el pedido
  String pedido="nothing";
  if (req.indexOf("/control/ch+") != -1){              //pedí subir canal
    pedido = "ch+";
    digitalWrite(14, 0);               
    digitalWrite(12, 0);
    digitalWrite(13, 0);
    delay(100);
    digitalWrite(13, 1);}
  else if (req.indexOf("/control/ch-") != -1){         //pedí bajar canal
    pedido = "ch-";
    digitalWrite(14, 0);               
    digitalWrite(12, 1);
    digitalWrite(13, 0);
    delay(100);
    digitalWrite(13, 1);}
  else if (req.indexOf("/control/on-off") != -1){         //pedí apagar o prender
    pedido = "on-off";
    digitalWrite(14, 1);               
    digitalWrite(12, 0);
    digitalWrite(13, 0);
    delay(100);
    digitalWrite(13, 1);}
  else if (req.indexOf("/control/volumen-") != -1){         //pedí bajar volumen
    pedido = "vol-";
    digitalWrite(14, 1);               
    digitalWrite(12, 1);
    digitalWrite(13, 0);
    delay(100);
    digitalWrite(13, 1);}
  else {
    Serial.println("invalid request");
    //client.stop();
    //return;
  }
  
  client.flush();

  // Para la pagina web
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nControl has ";  //<html> me marca que se abre código en html
  s += pedido;
  s += "</html>\n";   //se cierra el código en html

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // Cuando termino el loop el cliente se desconecta
}

