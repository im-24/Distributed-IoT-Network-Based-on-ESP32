/* ESP32 HTTP IoT Server - Dashboard Température et Humidité
   Version avec valeurs simulées pour test
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <uri/UriBraces.h>

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6

WebServer server(80);

// Variables pour les données simulées
float temperature = 22.5;
float humidity = 65.0;
unsigned long lastUpdate = 0;

// Page HTML du dashboard
void sendHtml() {
  String response = R"(
    <!DOCTYPE html><html>
      <head>
        <title>ESP32 IoT Dashboard</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
          html { font-family: sans-serif; text-align: center; }
          body { display: inline-flex; flex-direction: column; align-items: center; }
          h1 { margin-bottom: 1.2em; color: #333; }
          .card {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            border-radius: 20px;
            padding: 2em;
            margin: 1em;
            box-shadow: 0 10px 30px rgba(0,0,0,0.2);
            min-width: 300px;
          }
          .temp { font-size: 3.5em; font-weight: bold; color: #ff6b6b; margin: 20px 0; }
          .hum { font-size: 3em; font-weight: bold; color: #4ecdc4; margin: 20px 0; }
          .label { font-size: 1.2em; color: white; margin-bottom: 10px; }
          .icon { font-size: 2.5em; }
          .update { margin-top: 2em; font-size: 0.9em; color: white; }
          .status {
            background: #4caf50;
            color: white;
            padding: 8px 20px;
            border-radius: 25px;
            display: inline-block;
            margin-top: 15px;
            font-size: 0.9em;
          }
        </style>
        <script>
          function fetchData() {
            fetch('/data')
              .then(response => response.json())
              .then(data => {
                document.getElementById('temp').innerHTML = data.temperature.toFixed(1) + ' °C';
                document.getElementById('hum').innerHTML = data.humidity.toFixed(1) + ' %';
                document.getElementById('time').innerHTML = new Date().toLocaleTimeString();
                document.getElementById('status').innerHTML = 'En ligne';
              })
              .catch(err => {
                console.log(err);
                document.getElementById('status').innerHTML = ' Erreur';
              });
          }
          setInterval(fetchData, 2000);
          fetchData();
        </script>
      </head>
      <body>
        <h1>Dashboard IoT ESP32</h1>
        <div class="card">
          <div class="label">Temperature</div>
          <div class="temp" id="temp">--.- °C</div>
          <div class="label">Humidite</div>
          <div class="hum" id="hum">--.- %</div>
          <div class="status" id="status"> En ligne</div>
          <div class="update">last update : <span id="time">--:--:--</span></div>
        </div>
      </body>
    </html>
  )";
  server.send(200, "text/html", response);
}

// Route pour les données JSON
void sendJson() {
  String json = "{";
  json += "\"temperature\": " + String(temperature) + ",";
  json += "\"humidity\": " + String(humidity);
  json += "}";
  server.send(200, "application/json", json);
}

// Génération de valeurs simulées
void generateSimulatedData() {
  // Valeurs qui varient lentement entre 18-28°C et 40-80%
  static float trend = 0;
  trend += 0.1;
  
  temperature = 23.0 + sin(trend) * 3.0;
  humidity = 60.0 + cos(trend * 0.8) * 15.0;
  
  Serial.print(" Température simulée: ");
  Serial.print(temperature);
  Serial.print(" °C   Humidité: ");
  Serial.print(humidity);
  Serial.println(" %");
}

void setup(void) {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=== DÉMARRAGE ESP32 ===");
  
  // Connexion WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Connexion au WiFi ");
  Serial.print(WIFI_SSID);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  
  Serial.println(" Connecté!");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());
  
  // Configuration des routes
  server.on("/", sendHtml);
  server.on("/data", sendJson);
  
  // Démarrage du serveur
  server.begin();
  Serial.println("Serveur HTTP démarré");
  Serial.println("\n=== PRÊT ===");
  Serial.println("Ouvrez votre navigateur à: http://localhost:8180");
  Serial.println("Dashboard disponible\n");
  
  // Première génération
  generateSimulatedData();
}

void loop(void) {
  server.handleClient();
  
  // Mise à jour toutes les 3 secondes
  if (millis() - lastUpdate >= 3000) {
    generateSimulatedData();
    lastUpdate = millis();
  }
  
  delay(2);
}