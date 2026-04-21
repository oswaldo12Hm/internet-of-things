#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "ALUMNOS TEC";
const char* password = "";

WebServer server(80);

float latitude = 33.52;
float longitude = 151.12;

String myJson = "";
const int ledPin = 14;
int ledState = LOW;

const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<meta charset="UTF-8">
<head>
    <title>Clima con ESP32!</title>
</head>
<body>
    <h1>Consulta de temperatura</h1>
    
    <form action="/setcoords" method="GET">
        latitud: <input type="text" name="lat" value="18.37"><br><br>
        longitud: <input type="text" name="lon" value="-97.37"><br><br>
        <input type="submit" value="Consultar">
    </form>

    <p>Temperatura actual:</p>
    <p id="tempValue">-----</p>

    <p>Estado del LED (noche = encendido):</p>
    <p id="ledStatus">-----</p>

    <h2>Respuesta del Json:</h2>
    <pre id="jsonResponse">-----</pre>

    <script>
        function updateWeather() {
            fetch('/weather')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('tempValue').innerText = data.temperature + ' °C';
                    document.getElementById('ledStatus').innerText = data.led == 1 ? 'Encendido' : 'Apagado';
                    document.getElementById('jsonResponse').innerText = JSON.stringify(data.json, null, 2);
                });
        }
        updateWeather();
        setInterval(updateWeather, 5000);
    </script>
</body>
</html>
)rawliteral";

void handleRoot(){
  server.send(200, "text/html", htmlPage);
}

void handleSetCoords(){
  if(server.hasArg("lat")) latitude = server.arg("lat").toFloat();
  if(server.hasArg("lon")) longitude = server.arg("lon").toFloat();
  
  server.sendHeader("Location", "/");
  server.send(303);  // ✅ faltaba ;
}

void handleWeather(){
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;

    String url = "https://api.open-meteo.com/v1/forecast?latitude=" + 
                  String(latitude, 2) +
                  "&longitude=" + String(longitude, 2) + 
                  "&current_weather=true";

    http.begin(url);
    int httpCode = http.GET();
    
    if(httpCode > 0){
      myJson = http.getString();
      StaticJsonDocument<2048> doc;
      DeserializationError error = deserializeJson(doc, myJson);

      if(!error){
        float temperatura = doc["current_weather"]["temperature"];
        int isDay = doc["current_weather"]["is_day"];

        // ✅ Lógica correcta LED
        if(isDay == 0){
          ledState = HIGH; // noche → encendido
        } else {
          ledState = LOW;  // día → apagado
        }

        digitalWrite(ledPin, ledState); // ✅ faltaba ;

        String response;
        StaticJsonDocument<2048> out;

        out["temperature"] = temperatura;
        out["led"] = ledState;
        out["json"] = doc["current_weather"];

        serializeJson(out, response);
        server.send(200, "application/json", response);

        http.end();
        return;
      }
    }
    http.end();
  }

  server.send(200, "application/json",
  "{\"error\":\"No se pudieron obtener los datos\"}");
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT); // ✅ faltaba ;

  WiFi.begin(ssid, password);

  Serial.print("Conectando a WIFI ...");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado!");
  Serial.println("IP del servidor: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/setcoords", handleSetCoords);
  server.on("/weather", handleWeather);
  server.begin();
}

void loop() {
  server.handleClient();
}