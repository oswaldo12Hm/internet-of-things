#include <WiFi.h>
#include <WebServer.h>

const char* ssid  = "ALUMNOS TEC";
const char* password = "";
const int ledPin = 14; // Consistencia en minúsculas

// Inicializar server 
WebServer server(80);

// Prototipos de funciones (opcional pero recomendado)
void handleRoot();
void handleOn();
void handleOff();

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.begin(115200); // S mayúscula
  delay(400);
  
  WiFi.begin(ssid, password); // WiFi con ambas mayúsculas
  Serial.print("Conectando a Wi-Fi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConectado");
  Serial.print("IP del Servidor: ");
  Serial.println(WiFi.localIP());

  // Rutas
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff); // Corregido el nombre "hanldeOff"
  
  server.begin();
}

void loop() {
  server.handleClient();
}

// Implementación de funciones
void handleRoot() {
  String html = "<h1>!Controlar Led!</h1>"; // Falta punto y coma
  html += "<p><a href=\"/on\"><button> ON </button></a></p>";
  html += "<p><a href=\"/off\"><button> OFF </button></a></p>";
  server.send(200, "text/html", html);
}

void handleOn() {
  digitalWrite(ledPin, HIGH); // Era digitalWrite, no digitalWriter
  server.send(200, "text/html", "LED ENCENDIDO <br><br><a href=\"/\">Volver</a>");
}

void handleOff() {
  digitalWrite(ledPin, LOW); // Era digitalWrite
  server.send(200, "text/html", "LED APAGADO <br><br><a href=\"/\">Volver</a>");
}