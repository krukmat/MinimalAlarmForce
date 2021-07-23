#include <HTTPClient.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <esp_now.h>

const char* ssid = "MIWIFI_2G_2jJ5";
const char* password = "xvFYmqRv";

const String channelId = "1299306"; // Usado para identificar el canal
WiFiClient mqttIPClientWifi;
PubSubClient mqttIPClient( mqttIPClientWifi );
const char* mqtt_ip = "ioticos.org";
const int mqtt_ip_port = 1883;
const char* mqtt_ip_user = "hGR2rL1latTuCnB";
const char* mqtt_ip_password = "ASJ5c61zVvtuib7";
const char *mqtt_ip_topic = "htO9wfUxA50uzDS/output";
const char *mqtt_ip_topic_subscribe = "htO9wfUxA50uzDS/input";
int armed = 1;
typedef struct shoot_on_demand {
  bool shoot;
  String deviceId;
};

shoot_on_demand esp_now_data;

TaskHandle_t taskSendStatus;

void OnRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&esp_now_data, incomingData, sizeof(esp_now_data));
  Serial.print("Señal desde NOW");
  Serial.print(esp_now_data.shoot); 
  if (esp_now_data.shoot == true){
    Serial.print("Disparo de camara");
    sendSignalToCam();
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length);
void mqttReconnect();

void mqttReconnect() {

  while (!mqttIPClient.connected()) {
    Serial.print("Intentando conexión Mqtt...");
    // Creamos un cliente ID
    String clientId = "IOTICOS_H_W_";
    clientId += String(random(0xffff), HEX);
    // Intentamos conectar
    if (mqttIPClient.connect(clientId.c_str(),mqtt_ip_user,mqtt_ip_password)) {
      Serial.println("Conectado!");
      // Nos suscribimos
      if(mqttIPClient.subscribe(mqtt_ip_topic_subscribe)){
        Serial.println("Suscripcion ok");
      }else{
        Serial.println("fallo Suscripciión");
      }
    } else {
      Serial.print("falló :( con error -> ");
      Serial.print(mqttIPClient.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

// https://stackoverflow.com/questions/9072320/split-string-into-string-array
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void mqttCallback(char* topic, byte* payload, unsigned int length){
  String incoming = "";
  Serial.print("Mensaje recibido desde -> ");
  Serial.print(topic);
  Serial.println("");
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  incoming.trim();
  Serial.println("Mensaje -> " + incoming); 

  if (getValue(incoming,';',0) == channelId && getValue(incoming,';',2) == "MQTT"){
    armed = getValue(incoming,';',1).toInt();
  }
}

void bootUp(){
  if (!mqttIPClient.connected()) {
      mqttReconnect();
   }
}

void mqttLoop(){
  bootUp(); 
  mqttIPClient.loop();
}

void taskSendStatusMethod( void * parameter) {
  String statusMsg;
  for(;;) {
    mqttLoop();
    delay(100);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  pinMode(12, INPUT);
  WiFi.mode(WIFI_AP_STA);
  Serial.println(WiFi.macAddress());
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  

  if (esp_now_init() != ESP_OK) {
    Serial.println("There was an error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(OnRecv);


  mqttIPClient.setServer(mqtt_ip, mqtt_ip_port);
  mqttIPClient.setCallback(mqttCallback);
  bootUp();
  
  xTaskCreatePinnedToCore(
      taskSendStatusMethod, /* Function to implement the task */
      "taskSendStatus", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &taskSendStatus,  /* Task handle. */
      0); /* Core where the task should run */
}

void sendSignalToCam(){
  digitalWrite(2, 1);  
  delay(1000);
  digitalWrite(2, 0);
  delay(60000);
}

void loop() {
  // put your main code here, to run repeatedly:
  int movSensor = digitalRead(12);
  if (movSensor == 1 && armed == 1) {
    sendSignalToCam();
    Serial.println("Sensor movement detection!");    
  } else {
    delay(100);
  }
}
