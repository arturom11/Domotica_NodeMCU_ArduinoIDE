
#include <Arduino.h>
#include "BasicStepperDriver.h"
#include <ESP8266WiFi.h>
#define MOTOR_STEPS 200   //Definimos el número de pasos por vuelta
#define RPM 120           //Definimos la velocidad en este caso dos vueltas por segundo
#define MICROSTEPS 1      //Seteamos que se mueva con paso compleno y no microstepping
#define DIR 2             //Se establece el pin 2 como de dirección
#define STEP 0            //Se establece el pin 0 como de control de step
#define MS0 16             //Se establece el pin 16 para control microstepping
#define MS1 4             //Se establece el pin 4 para control microstepping
#define MS2 5             //Se establece el pin 5 para control microstepping

const char* ssid = "SSID";
const char* password = "password";


IPAddress ip(192, 168, 0, 108);         //set static ip
IPAddress gateway(192, 168, 0, 1);      //set getteway
IPAddress subnet(255, 255, 255, 0);     //set subnet
WiFiServer server(302);//Port 302 to access server

BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP); // Se inicializa el motor llamado stepper con los parámetros ingresados


void setup() {
      Serial.begin(115200);
      delay(10);

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
      
      stepper.begin(RPM, MICROSTEPS);   //pone en funcionamiento el motor, configurando velocidad y si usa o no micropaso
      pinMode(7,INPUT);                 //pin del Final de Carrera
}

void loop()
{
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
  int SubirPasos;

    if (req.indexOf("/cortina/0") != -1){     //Se necesita bajar la cortina del todo
    SubirPasos=0; 
    Mover(SubirPasos);
  }
    else if (req.indexOf("/cortina/25") != -1){     //Se necesita bajar la cortina al 75%
    SubirPasos=(int)200*13*0.25; 
    Mover(SubirPasos);
  }
    else if (req.indexOf("/cortina/50") != -1){     //Se necesita bajar la cortina al 50%
    SubirPasos=(int)200*13*0.50; 
    Mover(SubirPasos);
  }
    else if (req.indexOf("/cortina/75") != -1){     //Se necesita bajar la cortina al 25%
    SubirPasos=(int)200*13*0.75;
    Mover(SubirPasos);
  }
    else if (req.indexOf("/cortina/100") != -1){     //Se necesita subir la cortina del todo
    SubirPasos=(int)200*13; 
    Mover(SubirPasos);
  }
    else {
    Serial.println("invalid request");
    //client.stop();
    //return;
  }
  client.flush();
   // Para la pagina web
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nMotor is now working";  //<html> me marca que se abre código en html

  s += "</html>\n";   //se cierra el código en html

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // Cuando termino el loop el cliente se desconecta
}


void Mover(int SubirPasos){
  while (digitalRead(1)!=HIGH){     //Chequeo paso a paso si llegamos al fin de carrera, es decir llevamos
    stepper.move(-1);               //la cortina a su posición más baja.
    delay(150);                     //Ajusto la velocidad a dos vueltas por segundo
  }
                                    //Si llego al final de carrera, la cortina ya esta abajo
                                    
    stepper.move(SubirPasos);       //Si me pidieron subir, subo la cantidad de pasos necesaria
}
