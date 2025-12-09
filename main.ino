#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ---------- WiFi / MQTT ----------

const char* WIFI_SSID     = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

const char* MQTT_SERVER = "broker.hivemq.com";
const uint16_t MQTT_PORT = 1883;

const char* MQTT_TOPIC_DATA = "breno/iot/bpm";   // dados (bpm, adc, dt_ms)
const char* MQTT_TOPIC_CMD  = "breno/iot/cmd";   // comandos para atuador

WiFiClient espClient;
PubSubClient mqtt(espClient);

// ---------- Hardware (diagrama do Wokwi) ----------

const int POT_PIN    = 34;  // potenciômetro

const int LED_R_PIN  = 14;  // LED RGB - R
const int LED_G_PIN  = 12;  // LED RGB - G
const int LED_B_PIN  = 27;  // LED RGB - B

const int BUZZER_PIN = 25;  // buzzer

unsigned long lastSampleMillis  = 0;
unsigned long lastPublishMillis = 0;

// ---------- LED RGB (cátodo comum) ----------

void setColor(bool r, bool g, bool b) {
  digitalWrite(LED_R_PIN, r ? HIGH : LOW);
  digitalWrite(LED_G_PIN, g ? HIGH : LOW);
  digitalWrite(LED_B_PIN, b ? HIGH : LOW);
}

// ---------- WiFi ----------

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Conectando ao WiFi ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("WiFi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ---------- MQTT ----------

void connectMQTT() {
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);

  while (!mqtt.connected()) {
    Serial.print("Conectando ao MQTT... ");

    String clientId = "esp32-breno-";
    clientId += String(random(0xffff), HEX);

    if (mqtt.connect(clientId.c_str())) {
      Serial.println("conectado");
      mqtt.subscribe(MQTT_TOPIC_CMD);  // recebe comandos para atuador
    } else {
      Serial.print("falhou, rc=");
      Serial.print(mqtt.state());
      Serial.println(" - tentando de novo em 2s");
      delay(2000);
    }
  }
}

// callback para comandos MQTT (atuador)
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, MQTT_TOPIC_CMD) != 0) {
    return;
  }

  if (length == 0) return;

  unsigned long t0 = millis();

  char c = (char)payload[0];

  if (c == '1') {
    tone(BUZZER_PIN, 1000);  // liga buzzer
  } else {
    noTone(BUZZER_PIN);      // desliga buzzer
  }

  unsigned long t1 = millis();
  unsigned long lat_cmd_atuador = t1 - t0;

  Serial.print("LAT_CMD_ATUADOR=");
  Serial.print(lat_cmd_atuador);
  Serial.println(" ms");
}

// ---------- SETUP ----------

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("ESP32 + MQTT - Monitor de Batimentos");

  pinMode(POT_PIN, INPUT);

  pinMode(LED_R_PIN, OUTPUT);
  pinMode(LED_G_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  setColor(false, false, false);
  noTone(BUZZER_PIN);

  connectWiFi();

  mqtt.setCallback(mqttCallback);
  connectMQTT();

  lastSampleMillis  = millis();
  lastPublishMillis = millis();
}

// ---------- LOOP ----------

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqtt.connected()) {
    connectMQTT();
  }

  mqtt.loop();

  unsigned long now   = millis();
  unsigned long dt_ms = now - lastSampleMillis;
  lastSampleMillis    = now;

  int adc = analogRead(POT_PIN);
  int bpm = map(adc, 0, 4095, 40, 180);

  // LED por faixa de BPM
  if (bpm < 80) {
    setColor(false, false, true);   // azul
  } else if (bpm < 120) {
    setColor(false, true, false);   // verde
  } else if (bpm < 140) {
    setColor(true, true, false);    // amarelo
  } else {
    setColor(true, false, false);   // vermelho
  }

  // buzzer de alerta
  if (bpm >= 140) {
    tone(BUZZER_PIN, 1000);
  } else {
    noTone(BUZZER_PIN);
  }

  // imprime no Serial (útil para debug e tempo de amostragem)
  Serial.print("ADC=");
  Serial.print(adc);
  Serial.print(" | BPM=");
  Serial.print(bpm);
  Serial.print(" | dt_ms=");
  Serial.print(dt_ms);
  Serial.println(" ms");

  // publica no MQTT a cada 1s e mede tempo sensor -> publish
  if (now - lastPublishMillis >= 1000) {
    lastPublishMillis = now;

    unsigned long t0 = millis();

    String payload = String("{\"bpm\":") + bpm +
                     ",\"adc\":" + adc +
                     ",\"dt_ms\":" + dt_ms + "}";

    bool ok = mqtt.publish(MQTT_TOPIC_DATA, payload.c_str());

    unsigned long t1 = millis();
    unsigned long lat_sensor_mqtt = t1 - t0;

    Serial.print("LAT_SENSOR_MQTT=");
    Serial.print(lat_sensor_mqtt);
    Serial.println(" ms");

    Serial.print("MQTT -> ");
    Serial.print(payload);
    Serial.println(ok ? " [OK]" : " [ERRO]");
  }

  delay(300);
}
