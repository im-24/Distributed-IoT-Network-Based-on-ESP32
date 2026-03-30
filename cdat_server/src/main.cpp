/* ESP32 HTTP IoT Server - Dashboard Température et Humidité
   Basé sur l'exemple original pour Wokwi.com
   Compatible PlatformIO + Wokwi
   Accès : http://localhost:8180 (avec wokwi.toml)
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <uri/UriBraces.h>
#include <DHT.h>

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6

WebServer server(80);

// === Configuration du capteur DHT22 ===
#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// === Variables globales ===
float temperature = 0;
float humidity = 0;
unsigned long lastSensorRead = 0;

// === Page HTML (dashboard) ===
void sendHtml() {
  String response = R"(
    <!DOCTYPE html><html>
      <head>
        <title>ESP32 IoT Dashboard</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
          html { font-family: sans-serif; text-align: center; }
          body { display: inline-flex; flex-direction: column; align-items: center; }
          h1 { margin-bottom: 1.2em; }
          .card {
            background: #f5f5f5;
            border-radius: 20px;
            padding: 2em;
            margin: 1em;
            box-shadow: 0 4px 8px rgba(0,0,0,0.1);
          }
          .temp { font-size: 3em; color: #e67e22; }
          .hum { font-size: 2.5em; color: #2980b9; }
          .btn { background-color: #5B5; border: none; color: #fff; padding: 0.5em 1em;
                 font-size: 1.2em; text-decoration: none; border-radius: 8px; }
          .update { margin-top: 2em; font-size: 0.8em; color: #666; }
        </style>
        <script>
          function fetchData() {
            fetch('/data')
              .then(response => response.json())
              .then(data => {
                document.getElementById('temp').innerHTML = data.temperature + ' °C';
                document.getElementById('hum').innerHTML = data.humidity + ' %';
                document.getElementById('time').innerHTML = new Date().toLocaleTimeString();
              })
              .catch(err => console.log(err));
          }
          setInterval(fetchData, 2000);
          fetchData();
        </script>
      </head>
      <body>
        <h1>🌡️ Dashboard IoT ESP32</h1>
        <div class="card">
          <div class="temp">--.- °C</div>
          <div class="hum">--.- %</div>
          <div class="update">Dernière mise à jour : <span id="time">--:--:--</span></div>
        </div>
        <a href="/" class="btn">↻ Rafraîchir</a>
      </body>
    </html>
  )";
  server.send(200, "text/html", response);
}

// === Route pour les données JSON ===
void sendJson() {
  String json = "{";
  json += "\"temperature\": " + String(temperature) + ",";
  json += "\"humidity\": " + String(humidity);
  json += "}";
  server.send(200, "application/json", json);
}

// === Lecture du capteur ===
void readSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("❌ Erreur lecture DHT22");
    return;
  }

  temperature = t;
  humidity = h;

  Serial.print("🌡️ Température: ");
  Serial.print(temperature);
  Serial.print(" °C  💧 Humidité: ");
  Serial.print(humidity);
  Serial.println(" %");
}

void setup(void) {
  Serial.begin(115200);
  dht.begin();

  // Connexion WiFi (identique au code original)
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // === Routes conservées ===
  server.on("/", sendHtml);
  server.on("/data", sendJson);

  server.begin();
  Serial.println("HTTP server started (http://localhost:8180)");

  // Lecture initiale
  delay(2000);
  readSensor();
}

void loop(void) {
  server.handleClient();

  // Lecture toutes les 3 secondes
  if (millis() - lastSensorRead >= 3000) {
    readSensor();
    lastSensorRead = millis();
  }

  delay(2);
}