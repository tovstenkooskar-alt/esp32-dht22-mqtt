#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// WiFi параметры (замените на свои)
const char* ssid = "Oleg⁠ ";
const char* password = "⁠91761255";

// MQTT параметры
const char* mqtt_server = "m1.wqtt.ru";
const int mqtt_port = 17426;
const char* mqtt_user = "u_YC2PZ2";
const char* mqtt_password = "gjcvO2LH";
const char* topic = "sensor/data";

// Параметры DHT22
#define DHTPIN 14     // Пин подключения Data (можно изменить)
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Инициализация датчика DHT22
  dht.begin();
  Serial.println("DHT22 инициализирован");

  // Настройка MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Подключение к MQTT
  while (!client.connected()) {
    String clientId = "esp32client_" + String(millis() % 1000);
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("Подключено к MQTT брокеру");
    } else {
      Serial.print("Ошибка подключения к MQTT, код: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Получено сообщение [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void loop() {
  client.loop();

  // Считываем данные с DHT22
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Проверяем корректность данных
  if (isnan(h) || isnan(t)) {
    Serial.println("Ошибка чтения датчика DHT22!");
    return;
  }

  // Формируем строку с данными для отправки в MQTT
  String msg = "Temp:";
  msg += t;
  msg += ", Hum:";
  msg += h;

  // Отправляем данные на MQTT-сервер
  client.publish(topic, msg.c_str());
  Serial.println(msg);

  delay(10000);  // Интервал отправки - 10 секунд
}