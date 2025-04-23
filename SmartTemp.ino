#include <WiFi.h>
#include <DHT.h>
#include <Ultrasonic.h>
#include <ESPAsyncWebServer.h>

// Pin definitions
#define DHTPIN 4            // Pin for DHT22
#define DHTTYPE DHT22       // DHT22 sensor type
#define ECHO_PIN 5          // Echo pin for ultrasonic sensor
#define TRIG_PIN 18         // Trigger pin for ultrasonic sensor
#define FAN_PIN 19          // Pin for the fan (relay or transistor)

// Set up DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Set up ultrasonic sensor
Ultrasonic ultrasonic(TRIG_PIN, ECHO_PIN);

// Initialize web server
AsyncWebServer server(80);

// WiFi credentials
const char* ssid = "your_SSID";     // Replace with your WiFi SSID
const char* password = "your_PASSWORD"; // Replace with your WiFi password

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set up DHT sensor
  dht.begin();
  
  // Set up fan pin
  pinMode(FAN_PIN, OUTPUT);
  
  // Set up WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  
  // Serve HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    long distance = ultrasonic.read();
    
    String html = "<html><head><style>"
    "body { font-family: Arial, sans-serif; background-color: #f0f0f0; margin: 0; padding: 20px; }"
    "h1 { color: #4CAF50; text-align: center; }"
    "p { font-size: 18px; color: #333; }"
    "table { width: 100%; margin-top: 20px; border-collapse: collapse; }"
    "th, td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }"
    "tr:hover { background-color: #f1f1f1; }"
    "button { background-color: #4CAF50; color: white; padding: 10px 20px; border: none; cursor: pointer; font-size: 16px; }"
    "button:hover { background-color: #45a049; }"
    "</style></head><body><h1>ESP32 Sensor Web Server</h1>";
    html += "<table><tr><th>Sensor</th><th>Value</th></tr>";
    html += "<tr><td>Temperature</td><td>" + String(temperature) + "°C</td></tr>";
    html += "<tr><td>Humidity</td><td>" + String(humidity) + "%</td></tr>";
    html += "<tr><td>Distance</td><td>" + String(distance) + " cm</td></tr>";
    html += "</table>";
    
    // Control fan based on temperature or distance (example)
    if (temperature > 30 || distance < 10) {
      digitalWrite(FAN_PIN, HIGH);  // Turn on fan
      html += "<p>Fan: ON</p>";
    } else {
      digitalWrite(FAN_PIN, LOW);   // Turn off fan
      html += "<p>Fan: OFF</p>";
    }
    
    html += "<button onclick='location.reload()'>Refresh</button>";
    html += "</body></html>";
    
    request->send(200, "text/html", html);
  });

  // Start the server
  server.begin();
}

void loop() {
  // Nothing needed in loop
}
