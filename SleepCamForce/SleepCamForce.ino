/*********
  by ideaSpark

  IMPORTANT!!! 
   - Select Board "ESP32 Wrover Module"
   - Select the Partion Scheme "Huge APP (3MB No OTA)
   - GPIO 0 must be connected to GND to upload a sketch
   - After connecting GPIO 0 to GND, Click "upload" button.During upload,must press the ESP32-CAM on-board RESET button to put your board in flashing mode
   - Then GPIO 0 should not be connected to GND. Then press the ESP32-CAM on-board RESET button to run the code
   
*********/

#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "fd_forward.h"
#include "FS.h"
#include "SD_MMC.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/rtc_io.h"
#include <EEPROM.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>

String serverName = "http://159.89.106.106/upload.php";   // OR REPLACE WITH YOUR DOMAIN NAME


WiFiClient client;
PubSubClient mqttClient( client );
const char* server = "mqtt.thingspeak.com";


char mqttUserName[] = "Sensor_movimiento";                // Use any name.
char mqttPass[] = "FLDP47GIGCD7GHEP";                 // Change to your MQTT API key from Account > MyProfile.
long readChannelID=0000;
char readAPIKey[]="18YFIXSCYYK95BTO";
long writeChannelID=1299306;
char writeAPIKey[]   = "UQWJU47FAVVTZB8F";
int fieldsToPublish[8]={1,0,0,0,0,0,0,0};             // Change to allow multiple fields.
long dataToPublish[8];
String local_address;

const char* ssid = "MIWIFI_5G_2jJ5_EXT";
const char* password = "xvFYmqRv";


//define the number of bytes you want to access
#define EEPROM_SIZE 1

//Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM  32
#define RESET_GPIO_NUM  -1
#define XCLK_GPIO_NUM  0
#define SIOD_GPIO_NUM  26
#define SIOC_GPIO_NUM  27
#define Y9_GPIO_NUM  35
#define Y8_GPIO_NUM  34
#define Y7_GPIO_NUM  39
#define Y6_GPIO_NUM  36
#define Y5_GPIO_NUM  21
#define Y4_GPIO_NUM  19
#define Y3_GPIO_NUM  18
#define Y2_GPIO_NUM  5
#define VSYNC_GPIO_NUM  25
#define HREF_GPIO_NUM  23
#define PCLK_GPIO_NUM  22

int pictureNumber = 0;

void getID(char clientID[], int idLength){
static const char alphanum[] ="0123456789"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";                        // For random generation of the client ID.

    // Generate client ID.
    for (int i = 0; i < idLength ; i++) {
        clientID[ i ] = alphanum[ random( 51 ) ];
    }
    clientID[ idLength ] = '\0';
    
}

void mqttConnect()
{
    char clientID[ 9 ];
    
    // Loop until connected.
    while ( !mqttClient.connected() )
    {

        getID(clientID,8);
       
        // Connect to the MQTT broker.
        Serial.print( "Attempting MQTT connection..." );
        if ( mqttClient.connect( clientID, mqttUserName, mqttPass ) )
        {
            Serial.println( "Connected with Client ID:  " + String( clientID ) + " User "+ String( mqttUserName ) + " Pwd "+String( mqttPass ) );
           
        } else
        {
            Serial.print( "failed, rc = " );
            // See https://pubsubclient.knolleary.net/api.html#state for the failure code explanation.
            Serial.print( mqttClient.state() );
            Serial.println( " Will try again in 5 seconds" );
            delay( 5000 );
        }
    }
} 

void sendData(){
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  int64_t fr_start = esp_timer_get_time();

  fb = esp_camera_fb_get();
  if (!fb) {
        Serial.println("Camera capture failed");
        return;
  }
  long randNumber = random(3000);
  String filename = "file"+String(randNumber);
  while (sendPhotoToServer(fb, filename) == ""){
     Serial.println("retry to post "+filename);
     delay(500);
  }
  savePicture(fb, filename);
  connect2MQTT();
  dataToPublish[0]=randNumber;
  mqttPublish( writeChannelID, writeAPIKey, dataToPublish, fieldsToPublish );      
}

String sendPhotoToServer(camera_fb_t * fb, String filename) {
  String getAll;
  
  Serial.println("Connecting to server: " + serverName);

   if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;
    Serial.println(filename);
    String randomStr = String(random(0xffff), HEX);
    http.begin(serverName+"?filename="+filename+"&rnd="+randomStr);
    Serial.println("Post image to: " + serverName);
    http.addHeader("Content-Type", "image/jpg");
    http.addHeader("Content-Disposition","form-data; name=\"imageFile\"; filename=\"esp32-cam.jpg\"");    
    int httpResponseCode = http.POST(fb->buf,fb->len);   //Send the actual POST request
    http.end();
    Serial.println(httpResponseCode);
    if (httpResponseCode == 200){
      Serial.println("Post exitoso");
      return filename;
    } else {
      Serial.println("Error en POST");
    }
  }
  return "";
}

void savePicture(camera_fb_t * fb, String filename){
  //Path where new picture will be saved in SD Card
  String path = "/"+filename+".jpg";

  fs::FS &fs = SD_MMC; 
  Serial.printf("Picture file name: %s\n", path.c_str());
  
  File file = fs.open(path.c_str(), FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file in writing mode");
  } 
  else {
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path.c_str());
  }
  file.close();
}

/**
 * Process messages received from subscribed channel via MQTT broker.
 *   topic - Subscription topic for message.
 *   payload - Field to subscribe to. Value 0 means subscribe to all fields.
 *   mesLength - Message length.
 */

int mqttSubscriptionCallback( char* topic, byte* payload, unsigned int mesLength ) {
    
    char p[mesLength + 1];
    memcpy( p, payload, mesLength );
    p[mesLength] = NULL;
    Serial.print( "Answer: " );
    Serial.println( String(p) );
}

static void connect2MQTT(){
  mqttClient.setServer( server, 1883 ); // Set the MQTT broker details.
  mqttClient.setCallback( mqttSubscriptionCallback );   // Set the MQTT message handler function.
  
  if (!mqttClient.connected())
  {
      mqttConnect(); // Connect if MQTT client is not connected.
  } 
  mqttClient.loop(); // Call the loop to maintain connection to the server.         
}

void mqttPublish(long pubChannelID, char* pubWriteAPIKey, long dataArray[], int fieldArray[]) {
    int index=0;
    String dataString="";
    
    // 
    while (index<1){
        
        // Look at the field array to build the posting string to send to ThingSpeak.
        if (fieldArray[ index ]>0){
          
            dataString+="&field" + String( index+1 ) + "="+ String(dataArray [ index ]);
        }
        index++;
    }
    dataString+="&status=MQTTPUBLISH";
    
    //
    
    // Create a topic string and publish data to ThingSpeak channel feed.
    String topicString ="channels/" + String( pubChannelID ) + "/publish/"+String( pubWriteAPIKey );
    Serial.println( "Topic channel " + topicString );
    Serial.println( dataString );
    mqttClient.publish( topicString.c_str(), dataString.c_str() );
}
void print_GPIO_wake_up(){
  int GPIO_reason = esp_sleep_get_ext1_wakeup_status();
  Serial.print("GPIO that triggered the wake up: GPIO ");
  Serial.println((log(GPIO_reason))/log(2), 0);
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG,0);//disable brownout detector
  Serial.begin(115200);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 5000000;
  config.pixel_format = PIXFORMAT_JPEG;

  //pinMode(4,INPUT);
  //digitalWrite(4,LOW);
  //rtc_gpio_hold_dis(GPIO_NUM_4);

  pinMode(GPIO_NUM_12, INPUT);
  //pinMode(GPIO_NUM_12, INPUT_PULLUP);

  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  }else{
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  //Init Camera
  esp_err_t err = esp_camera_init(&config);
  if(err != ESP_OK){
    Serial.printf("Camera init failed with error");  
    return;
  }

  Serial.println("Starting SD Card");

  delay(500);
  if(!SD_MMC.begin()){
    Serial.println("SD Card Mount Failed");  
  }

  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD Card attached");
    return;  
  }
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  String WiFiAddr = WiFi.localIP().toString();
  local_address = WiFiAddr;

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
  print_wakeup_reason();

  sendData();
  
  //Turns off the ESP32-CAM white on-board LED (flash) connected to GPIO 4
  //pinMode(4, OUTPUT);
  //digitalWrite(4, LOW);
  //rtc_gpio_hold_en(GPIO_NUM_4);

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 1); //0=LOW, 1=HIGH
  
  Serial.println("Going to sleep now");
  delay(60000);

  
  esp_deep_sleep_start();
}
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void loop() {
  delay(1000);
}
