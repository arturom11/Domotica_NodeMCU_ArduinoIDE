#include <ESP8266WiFi.h>

const char* ssid = "SSID";
const char* password = "password";

// Instacio el Server seteado para que escuche por el puerto 301

IPAddress ip(192, 168, 0, 107); //set static ip
IPAddress gateway(192, 168, 0, 1); //set getteway
IPAddress subnet(255, 255, 255, 0);//set subnet
WiFiServer server(301);//Port 301 to access server
 
void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO2
  pinMode(5, OUTPUT);               //Luz=D1=GP 5
  digitalWrite(5, 1);               //Luz apagada (relé normalmente abierto)
  pinMode(4, OUTPUT);               //Enchufe=D2=GP 4
  digitalWrite(4, 1);               //Enchufe encendido (relé normalmente cerrado)
  
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
  int luz;
  int enchufe;
  if (req.indexOf("/luz/0") != -1){              //pedí apagar luz
    luz = 0;
    digitalWrite(5, 1);}
  else if (req.indexOf("/luz/1") != -1){         //pedí prender luz
    luz = 1;
    digitalWrite(5, 0);}
  else if (req.indexOf("/enchufe/0") != -1){         //pedí apagar enchufe
    enchufe = 0;
    digitalWrite(4, 0);}
  else if (req.indexOf("/enchufe/1") != -1){         //pedí prender enchufe
    enchufe = 1;
    digitalWrite(4, 1);}
  else {
    Serial.println("invalid request");
    //client.stop();
    //return;
  }
  
  client.flush();

  // Para la pagina web
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nLuz is now ";  //<html> me marca que se abre código en html
  s += (luz)?"ON":"OFF";
  s += "\r\nEnchufe is now ";
  s += (enchufe)?"ON":"OFF";
  s += "</html>\n";   //se cierra el código en html

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // Cuando termino el loop el cliente se desconecta
}

