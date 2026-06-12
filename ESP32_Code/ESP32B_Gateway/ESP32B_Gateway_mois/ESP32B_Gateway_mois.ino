// ============================================================
//  ESP32-B — WiFi Gateway (new board)
//  Responsibilities: WiFi + AWS IoT MQTT + UART bridge
//  Communicates with ESP32-A via Serial2 (UART)
//  Reads soil moisture sensor via GPIO32 (analog)
// ============================================================

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

// ========== Serial2 (UART communication with ESP32-A) ==========
// ESP32-B GPIO16 (RX2) ← ESP32-A TX
// ESP32-B GPIO17 (TX2) → ESP32-A RX
#define UART_RX  16
#define UART_TX  17
#define SOIL_PIN 32

// ========== WiFi Configuration ==========
const char* WIFI_SSID     = "HighFour";
const char* WIFI_PASSWORD = "22222222";

// ========== AWS IoT Configuration ==========
const char* AWS_ENDPOINT  = "a18edjd681hnta-ats.iot.ap-northeast-1.amazonaws.com";
const int   AWS_PORT      = 8883;
const char* CLIENT_ID     = "Wateringesp32";
const char* TOPIC_PUBLISH = "garden/Wateringesp32/telemetry";
const char* TOPIC_CMD     = "garden/Wateringesp32/cmd";

// ========== AWS Certificates ==========
const char* AWS_ROOT_CA = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char* DEVICE_CERT = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUYpzlcHLCgIEiidcYIHF4Z8BWAN0wDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI2MDYwMzA3MjMz
M1oXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMxXu8c82yTTaqkA7pW7
54AhoFKG+lXNy8TK7tomgKKWWXQaS3jF0w2HRD6dKvKbo5slprlQJO1ujnrfhOPm
C72md6vEdWrnsFcozxAtuFOkyjCm9VDH+xSgyupzuGNwzFba7JBDyUrcr7gwi8iD
mvvkCeGQ31MJmmm8NtaebU/Cvg1GieeTHYOGZMvOYB6oSULycoQaQA4xo3LSojhY
lhkADeVRFBq3WolGB1ia4h2bB4vHiQZ0BoxHF3ZZjmU37Ooqswbso9CSLRuS0/KS
MHsVdwfav4IaNxSS8CBYaxWdIa9HCzI55FbDvxJnAzXBC8+6L+bCHPBn9cWh31MT
W2cCAwEAAaNgMF4wHwYDVR0jBBgwFoAUkLqpIzRVXKthEpiLz77dAyaZ3m4wHQYD
VR0OBBYEFLkTap/+1ZF6TzQGku8+exVcKRKDMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBTkWFyMdspk+zUkE9EgFS4tEVX
nJMZrGH7GvFJvIFXxFmiU2bR0bJU/BFpix1JTahahzPjAvDVLUGR50HdlQ/n1P51
zdRE7YYu1mfgmVtPw+A+CSO7+hclzM2LaKDOZmDs26WjF/WFs0qmehqtMcC0Wuj0
LasbVCkhfAioEMNMGNEkow0ZxKT5gFZUhqtyhIkOEskFpsmMUYgFFqnUle8nQSLf
WIaUVqVIAbsFJe+38tPO4Zb/BV/+TBL+IGCHLcRONfpqz5MA1H+RYlP6goSIWXvs
lOxM8EjKl1fRu+sSXh0CKQGHw+QLaMIcI1Cpn3y2uiAMH1TAd9kMIYM6aNDK
-----END CERTIFICATE-----
)EOF";

const char* PRIVATE_KEY = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAzFe7xzzbJNNqqQDulbvngCGgUob6Vc3LxMru2iaAopZZdBpL
eMXTDYdEPp0q8pujmyWmuVAk7W6Oet+E4+YLvaZ3q8R1auewVyjPEC24U6TKMKb1
UMf7FKDK6nO4Y3DMVtrskEPJStyvuDCLyIOa++QJ4ZDfUwmaabw21p5tT8K+DUaJ
55Mdg4Zky85gHqhJQvJyhBpADjGjctKiOFiWGQAN5VEUGrdaiUYHWJriHZsHi8eJ
BnQGjEcXdlmOZTfs6iqzBuyj0JItG5LT8pIwexV3B9q/gho3FJLwIFhrFZ0hr0cL
MjnkVsO/EmcDNcELz7ov5sIc8Gf1xaHfUxNbZwIDAQABAoIBABb6vsk3FMXdaJ88
1ZfNG8mS/n6JY1mG3SVyVM3/inO+SKz/0ADy7jsVPR11e0DxM8LcxxUL6DszNZ6n
yEAAvM1FDSwp2bJ9/5ytoCuHmLT/USWLgQbmllvJgUhAXzn/k1spH3VRyi87Bh/G
hdG0yxcMm/fb3OFvsQe2/MvB4TUY09vFWF6cMVsrtCB5zOzVUBwwsCAMqxfFkDiE
tKLzmtiEyuW+gSFiNGZpct6/2u0FpA/fLv8UMPH2o8eVO6SUZa1maSaYkukfyex7
sPyw/SNxuj0aeBGz4SxrIy4/cLUC5GOcqzX7IdtYJpTd3d6qJtephXLXgxS0oFpk
yi7g5GECgYEA72K3vcp3opPLastgK1iGRCtSibo5dllQw/YyGe86YaiF1v13gz+9
+Y37PIYMzN7MXWAbZ1RXrcRUtfHJGHobrsdassW8uf0m5iczEWpfQsYgzKRB8fUQ
aSbj+uwzz8QwM83Y3oW5Wi6e7lKF7WIpx31XFOOAxblsOqxwOhVgzokCgYEA2oZk
IymF6SOoYf5HiB3vjSIoD6qoehPkRv7aSr8SMYRtl1AB5eQ8olUNwJp3hCjnf2S7
8NlAQyuVFy0ig+N3loKKTXiiax8eMpcWjlicvQ/us70exXyBiqubGmRVjOwDE5Xy
xAc4FlUXVzWvqnnDrQVU005jqXXvHq8kSXtH3m8CgYEA43LbfBPsuedNZooYNPcm
k4zKHpx3TpCsYZUBo4ye3v1dNZmdPXKKEZwqsyX28Y1bhfjz8Ki9qhgaGMnDtWMH
/HfkIGoq2FjE5fcm2YxFOM/BiE8lbTzYu+A8y0Q0MluJJR+XyE4sf5g+X9i+8X7M
jYP1Ap7q0nNMdy8ybq2zTmkCgYEAsXm4Vj5g/YnyNTP5EHmPloYJF0A4+HyzR12w
GEi6E/FDuEJIaFdHT2fYdXZ+Xp2B9hrv8Jng8SyyN1J217qj4189x9GTx4BMXDYN
IaqiEYKhEOUl2qpV4zOgEn0uZfdAQGXl9oMOcETHYhK+Cqw92YWc8MiXdIfNnpQL
2on/Qo0CgYAFiFIYqMGKJXgCCWsaKMoRf+Yd4nJcslkDGHc15T8Aj3Rl1eWRNRc3
1KMz68O3bJT69DATBBLd+Med54NM8EaMv5Ep1kfFH7OYurlMP1TDxb5V4LuzhBAa
AGp2eF79lAh4s0bjOrpeQi1TMgOEgX0lvsOQO0A2PngS1rYK3BIy6g==
-----END RSA PRIVATE KEY-----
)EOF";

// ========== Global Objects ==========
WiFiClientSecure net;
PubSubClient     client(net);

unsigned long lastReconnectAttempt = 0;

// ========== Receive watering command from AWS → compact → forward to ESP32-A ==========
void onMqttMessage(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  Serial.println("[B] MQTT received: " + msg);

  // Parse and re-serialize as single compact line before forwarding to ESP32-A
  // This ensures multi-line formatted JSON from MQTT Test Client is handled correctly
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, msg);
  if (error) {
    Serial.println("[B] Forward JSON parse error, skip");
    return;
  }

  String compact;
  serializeJson(doc, compact);  // Compress to single line
  Serial2.println(compact);     // Send to ESP32-A with newline delimiter
  Serial.println("[B] Forwarded to A: " + compact);
}

// ========== WiFi Connection ==========
void connectWiFi() {
  Serial.print("[B] Connecting WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[B] WiFi connected. IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\n[B] WiFi failed! Retrying later...");
  }
}

// ========== AWS IoT Connection ==========
bool connectAWS() {
  if (client.connected()) return true;

  net.setCACert(AWS_ROOT_CA);
  net.setCertificate(DEVICE_CERT);
  net.setPrivateKey(PRIVATE_KEY);

  client.setServer(AWS_ENDPOINT, AWS_PORT);
  client.setBufferSize(512);
  client.setCallback(onMqttMessage);

  Serial.print("[B] Connecting AWS IoT");
  if (client.connect(CLIENT_ID)) {
    Serial.println("\n[B] AWS IoT connected!");
    client.subscribe(TOPIC_CMD);
    Serial.println("[B] Subscribed: " + String(TOPIC_CMD));

    // Notify ESP32-A that AWS is connected
    Serial2.println("{\"action\":\"status\",\"connected\":\"AWS OK\"}");
    return true;
  } else {
    Serial.println("[B] AWS connect failed. rc=" + String(client.state()));
    return false;
  }
}

// ========== Setup ==========
void setup() {
  Serial.begin(115200);
  delay(500);

  // Soil moisture sensor on GPIO32 (analog)
  pinMode(SOIL_PIN, INPUT);
  Serial.println("[B] Soil sensor ready on GPIO32!");

  // Serial2: communication with ESP32-A
  Serial2.begin(115200, SERIAL_8N1, UART_RX, UART_TX);

  Serial.println("[B] ESP32-B Gateway booting...");

  connectWiFi();
  connectAWS();
}

// ========== Loop ==========
void loop() {

  // Maintain AWS connection
  if (!client.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      if (WiFi.status() != WL_CONNECTED) connectWiFi();
      connectAWS();
    }
  }
  client.loop();

  // Read JSON from ESP32-A → inject soil moisture → publish to AWS
  if (Serial2.available()) {
    String line = Serial2.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) return;

    Serial.println("[B] Received from A: " + line);

    // Parse JSON from ESP32-A
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, line);
    if (error) {
      Serial.println("[B] JSON parse error, skip");
      return;
    }

    if (doc.containsKey("device_id")) {
      // Read soil moisture from GPIO32 (analog) and overwrite placeholder
      const int waterValue = 1350;
      const int dryValue = 3050;
      int soilRaw     = analogRead(SOIL_PIN);
      int percent = map(soilRaw, dryValue, waterValue, 0, 100);
      Serial.println(percent);
      float soilPct   = constrain(percent, 0, 100);
      doc["moisture"] = serialized(String(soilPct, 1));
      Serial.println("[B] Soil: " + String(soilRaw) + " raw / " + String(soilPct, 1) + "%");

      // Send moisture back to ESP32-A for LCD display
      String moistureMsg = "{\"action\":\"sensor\",\"moisture\":" + String((int)soilPct) + "}";
      Serial2.println(moistureMsg);
      Serial.println("[B] Sent moisture to A: " + moistureMsg);

      // Serialize updated JSON and publish to AWS IoT
      String output;
      serializeJson(doc, output);
      bool ok = client.publish(TOPIC_PUBLISH, output.c_str());
      Serial.println(ok ? "[B] Publish OK: " + output : "[B] Publish FAILED");
    }
  }
}
