#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// ==================== CONFIGURATION ====================
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

#define DHTPIN 15
#define DHTTYPE DHT22

// ==================== OBJETS ====================
WebServer server(80);
DHT dht(DHTPIN, DHTTYPE);

// ==================== PAGE HTML ====================
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Dashboard IoT ESP32</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        
        .container {
            max-width: 800px;
            margin: 0 auto;
        }
        
        h1 {
            color: white;
            text-align: center;
            margin-bottom: 30px;
            font-size: 2rem;
        }
        
        .card {
            background: white;
            border-radius: 20px;
            padding: 30px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.2);
            margin-bottom: 20px;
        }
        
        .sensor-card {
            background: white;
            border-radius: 20px;
            padding: 30px;
            text-align: center;
            box-shadow: 0 10px 30px rgba(0,0,0,0.2);
        }
        
        .temp {
            font-size: 4rem;
            font-weight: bold;
            color: #ff6b6b;
            margin: 20px 0;
        }
        
        .hum {
            font-size: 3rem;
            font-weight: bold;
            color: #4ecdc4;
            margin: 20px 0;
        }
        
        .label {
            font-size: 1.2rem;
            color: #666;
            margin-bottom: 10px;
        }
        
        .icon {
            font-size: 3rem;
        }
        
        .update-time {
            text-align: center;
            color: white;
            margin-top: 20px;
            font-size: 0.9rem;
        }
        
        .status {
            background: #4caf50;
            color: white;
            padding: 5px 15px;
            border-radius: 20px;
            display: inline-block;
            margin-top: 20px;
        }
        
        @media (max-width: 768px) {
            h1 { font-size: 1.5rem; }
            .temp { font-size: 2.5rem; }
            .hum { font-size: 2rem; }
            .icon { font-size: 2rem; }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🌡️ Dashboard IoT ESP32</h1>
        
        <div class="sensor-card">
            <div class="icon">🌡️</div>
            <div class="label">Température</div>
            <div class="temp" id="temperature">--.- °C</div>
            
            <div class="icon">💧</div>
            <div class="label">Humidité</div>
            <div class="hum" id="humidity">--.- %</div>
            
            <div class="status" id="status">🟢 En ligne</div>
        </div>
        
        <div class="update-time">
            Dernière mise à jour: <span id="updateTime">--:--:--</span>
        </div>
    </div>

    <script>
        function fetchData() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('temperature').innerHTML = data.temperature + ' °C';
                    document.getElementById('humidity').innerHTML = data.humidity + ' %';
                    
                    const now = new Date();
                    document.getElementById('updateTime').innerHTML = now.toLocaleTimeString();
                })
                .catch(error => {
                    console.error('Erreur:', error);
                    document.getElementById('status').innerHTML = '🔴 Erreur de connexion';
                });
        }
        
        // Chargement initial
        fetchData();
        
        // Mise à jour toutes les 2 secondes
        setInterval(fetchData, 2000);
    </script>
</body>
</html>
)rawliteral";

// ==================== VARIABLES ====================
float temperature = 0;
float humidity = 0;
unsigned long lastRead = 0;

// ==================== LECTURE CAPTEUR ====================
void readSensor() {
  // Lecture du capteur DHT22
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  
  // Vérifier si la lecture est valide
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Erreur de lecture du capteur");
    temperature = 0;
    humidity = 0;
  } else {
    Serial.print("Température: ");
    Serial.print(temperature);
    Serial.print(" °C, Humidité: ");
    Serial.print(humidity);
    Serial.println(" %");
  }
}

// ==================== ROUTES SERVEUR ====================
void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleData() {
  // Créer la réponse JSON manuellement
  String json = "{";
  json += "\"temperature\": " + String(temperature) + ",";
  json += "\"humidity\": " + String(humidity);
  json += "}";
  
  server.send(200, "application/json", json);
}

// ==================== SETUP ====================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=== DÉMARRAGE ESP32 ===");
  
  // Initialisation du capteur
  dht.begin();
  Serial.println("Capteur DHT22 initialisé");
  
  // Connexion WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("Connexion au WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connecté !");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());
  
  // Configuration du serveur web
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
  
  Serial.println("Serveur web démarré");
  Serial.println("\n=== PRÊT ===");
  Serial.print("Ouvrez votre navigateur à: http://");
  Serial.println(WiFi.localIP());
  
  // Première lecture
  readSensor();
}

// ==================== LOOP ====================
void loop() {
  // Gérer les requêtes web
  server.handleClient();
  
  // Lire le capteur toutes les 3 secondes
  if (millis() - lastRead >= 3000) {
    readSensor();
    lastRead = millis();
  }
  
  delay(10);
}