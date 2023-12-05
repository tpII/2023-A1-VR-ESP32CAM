#include "esp_camera.h"
#include <WiFi.h>
#define MQTT_MAX_PACKET_SIZE 36000
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "GlobalFunctions.h"



//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//
//            You must select partition scheme from the board menu that has at least 3MB APP space.
//            Face Recognition is DISABLED for ESP32 and ESP32-S2, because it takes up from 15
//            seconds to process single frame. Face Detection is ENABLED if PSRAM is enabled as well

// ===================
// Select camera model
// ===================
#define CAMERA_MODEL_AI_THINKER  // Has PSRAM
#include "camera_pins.h"

#define LED_PIN 4
#define LEFT_SERVO_PIN 15
#define RIGHT_SERVO_PIN 13

// ===========================
// Enter your WiFi credentials
// ===========================
const char* ssid = "Red";
const char* password = "fran1113";
const char* mqtt_server = "192.168.137.48";

// NTP Server details
const long utcOffsetInSeconds = 3600; // Set UTC offset as per your timezone
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "192.168.137.48", utcOffsetInSeconds, 60000);

WiFiClient espClient;
PubSubClient client(espClient);
Servo servo_left;
Servo servo_right;

void startCameraServer();
//void setupLedFlash(int pin);
void setupCamera();
void reconnect();

int movimiento = 1;

void callback(char* topic, byte* message, unsigned int length) {

  // Convert the message to a String for easier handling
  String msgString = "";
  for (unsigned int i = 0; i < length; i++) {
    msgString += (char)message[i];
    //Serial.print((char)message[i]);
  }
  //Serial.println();

  if ((char)message[0] == 'U') {
    // Rotate both servos in one direction
    servo_left.writeMicroseconds(1700);  // Adjust this value as per your servo's requirements
    servo_right.writeMicroseconds(1200);
  }
  else if ((char)message[0] == 'D') {
    // Rotate both servos in the opposite direction
    servo_left.writeMicroseconds(1200);  // Adjust this value as per your servo's requirements
    servo_right.writeMicroseconds(1700);
  }
  else if ((char)message[0] == 'L') {
    // Rotate only one of the servos
    servo_left.writeMicroseconds(1480);  // Adjust this value as per your servo's requirements
    servo_right.writeMicroseconds(1200);  // Stop the second servo
  }
  else if ((char)message[0] == 'R') {
    // Rotate the other servo
    servo_left.writeMicroseconds(1700);  // Stop the first servo
    servo_right.writeMicroseconds(1480);  // Adjust this value as per your servo's requirements
  }
  else if ((char)message[0] == 'S'){
    servo_left.writeMicroseconds(1480);
    servo_right.writeMicroseconds(1480);
  }

  // Find the comma in the message to separate the command from the timestamp
  int commaIndex = msgString.indexOf(',');
  if (commaIndex != -1) {
    // Extract the timestamp part of the message
    String timestamp = msgString.substring(commaIndex + 1);

    // Prepare the acknowledgment message
    String ackMsg = "ACK," + timestamp;

    // Convert the acknowledgment message to a C-style string and publish
    client.publish("ack_topic", ackMsg.c_str());
  }
}

int minPulseWidth = 1000;
int maxPulseWidth = 2000;

unsigned long lastNtpTime = 0;
unsigned long lastMillis = 0;

void updateTime() {
  if (timeClient.update() || lastNtpTime == 0) {
    lastNtpTime = timeClient.getEpochTime();
    lastMillis = millis();
  }
}

String getFormattedTimeWithMillis() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return "";
  }

  long millisec = millis() % 1000;
  char timeString[50];
  sprintf(timeString, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
          (timeinfo.tm_year + 1900), (timeinfo.tm_mon + 1), timeinfo.tm_mday,
          timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, millisec);

  return String(timeString);
}


void setup() {
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  Serial.println();

  setupCamera();


  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();
  timeClient.forceUpdate(); // Force an update to sync time immediately

  // Set system time to NTP time
  struct timeval now = { .tv_sec = timeClient.getEpochTime(), .tv_usec = 0 };
  settimeofday(&now, NULL);

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  servo_left.attach(LEFT_SERVO_PIN, minPulseWidth, maxPulseWidth);  // Assuming servo 1 is connected to GPIO 13
  servo_right.attach(RIGHT_SERVO_PIN, minPulseWidth+ 100, maxPulseWidth + 100);  // Assuming servo 2 is connected to GPIO 15
}

void loop() {
  // Do nothing. Everything is done in another task by the web server
  if (!client.connected()) {
    reconnect();
  };

  client.loop(); // This line ensures the MQTT client processes any incoming messages and maintains the connection.
  timeClient.update();
  updateTime();
}

void setupCamera() {
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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t* s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);        // flip it back
    s->set_brightness(s, 1);   // up the brightness just a bit
    s->set_saturation(s, -2);  // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_QVGA);
  }
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Auto")) {
      Serial.println("connected");
      Serial.println(client.subscribe("test"));
      Serial.println("connected to topic test");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
