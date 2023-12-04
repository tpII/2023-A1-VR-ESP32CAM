#include <ESP32Servo.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <string>
#include <WiFi.h>
#include <PubSubClient.h>

#define MAX_X 86.02
#define MIN_X -91.45

#define MAX_Y 85.98
#define MIN_Y -88.70

#define MAX_Z 89.49
#define MIN_Z -81.39

void MPU_config(Adafruit_MPU6050 *);
void MPU_calibrate_rotation(Adafruit_MPU6050 *mpu, float *offset, char axis);
float MPU_get_rotation(Adafruit_MPU6050 *mpu, char axis);
float MPU_get_yaw(Adafruit_MPU6050 *mpu);

const char *ssid = "Red";
const char *password = "fran1113";
const char *mqtt_server = "192.168.137.48";

WiFiClient espClient;
PubSubClient client(espClient);

void WIFI_connect();
void MQTT_keepAlive();
void MQTT_reconnect();

float pos_x;
float pos_y;
float pos_z;
float prev_pos_x;
float prev_pos_y;
float prev_pos_z;
float offset_x;
float offset_y;
float offset_z;

Adafruit_MPU6050 mpu;

Servo myservo; // create servo object to control a servo

float get_z(Adafruit_MPU6050 *mpu)
{
  sensors_event_t a, g, temp;
  (*mpu).getEvent(&a, &g, &temp);
  return g.gyro.z;
}

void setup()
{
  Serial.begin(115200);

  WIFI_connect();

  MPU_config(&mpu);

  MPU_calibrate_rotation(&mpu, &offset_x, 'x');
  MPU_calibrate_rotation(&mpu, &offset_y, 'y');
  MPU_calibrate_rotation(&mpu, &offset_y, 'z');
}

void loop()
{

  MQTT_keepAlive();

  pos_y = trunc(MPU_get_rotation(&mpu, 'y')); // la rotación en el eje z controlará el servo inferior
  pos_z = (MPU_get_yaw(&mpu));                // la rotación en el eje z controlará el servo superior

  if ((abs(prev_pos_y - pos_y) > 2) || (abs(prev_pos_z - pos_z) > 2))
  {
    // PARTE A
    prev_pos_y = pos_y;
    prev_pos_z = pos_z;
    std::string msg = std::to_string(int(pos_y)) + "_" + std::to_string(int(pos_z));
    client.publish("MPU", (msg).c_str(), 1);

    /*// OPCION B
    while ((prev_pos_y != pos_y) || prev_pos_z != pos_z)
    {
      if (prev_pos_y != pos_y)
      {
        prev_pos_y++;
      }
      if (prev_pos_z != pos_z)
      {
        prev_pos_z++;
      }
      std::string msg = std::to_string(int(prev_pos_y)) + "_" + std::to_string(int(prev_pos_z));
      client.publish("MPU", (msg).c_str(), 1);
      delay(10);
    }
  }*/
  }
  delay(50);
}

void WIFI_connect()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  client.setServer(mqtt_server, 1883);
}

void MQTT_keepAlive()
{
  if (!client.connected())
  {
    MQTT_reconnect();
  }

  client.loop();
}

void MQTT_reconnect()
{
  while (!client.connected())
  {
    // Serial.print("Conectando al servidor MQTT...");
    if (client.connect("ESP32MPU"))
    {
      Serial.println("conectado");
    }
    else
    {
      Serial.print("falló, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void MPU_calibrate_rotation(Adafruit_MPU6050 *mpu, float *offset, char axis)
{
  // Se toman 100 muestras de la velocidad angular del giroscopio en x y se promedian para devolver un offset de medicion
  Serial.println("\n\nMPU6050 calibration...");

  float total = 0;
  float accel_ang_axis;

  for (int i = 0; i < 100; i++)
  {
    sensors_event_t a, g, temp;
    (*mpu).getEvent(&a, &g, &temp);
    switch (axis)
    {
    case 'x':
      accel_ang_axis = g.gyro.x;
      // accel_ang_axis = atan((a.acceleration.x) / sqrt(pow((a.acceleration.y), 2) + pow((a.acceleration.z), 2))) * (180.0 / 3.14);
      break;
    case 'y':
      accel_ang_axis = atan((a.acceleration.y) / sqrt(pow((a.acceleration.x), 2) + pow((a.acceleration.z), 2))) * (180.0 / 3.14);
      break;
    case 'z':
      accel_ang_axis = atan((a.acceleration.z) / sqrt(pow((a.acceleration.x), 2) + pow((a.acceleration.y), 2))) * (180.0 / 3.14);
      break;
    }
    total = total + accel_ang_axis;
    delay(10);
  }

  Serial.println("\nDone with offset " + String(axis) + ": " + String(total / 100) + "\n");

  *offset = total / 100;
}

void MPU_config(Adafruit_MPU6050 *mpu)
{
  if (!(*mpu).begin())
  {
    Serial.println("Failed to find MPU6050 chip");
    while (1)
    {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  (*mpu).setAccelerometerRange(MPU6050_RANGE_8_G);
  (*mpu).setGyroRange(MPU6050_RANGE_500_DEG);
  (*mpu).setFilterBandwidth(MPU6050_BAND_21_HZ);
}

float MPU_get_rotation(Adafruit_MPU6050 *mpu, char axis)
{
  sensors_event_t a, g, temp;
  (*mpu).getEvent(&a, &g, &temp);
  float rotation;
  switch (axis)
  {
  case 'x':
    rotation = atan((a.acceleration.x) / sqrt(pow((a.acceleration.y), 2) + pow((a.acceleration.z), 2))) * (180.0 / 3.14);
    rotation -= offset_x;
    if (rotation < 0)
    {
      rotation = rotation * ((-90.0) / MIN_X);
    }
    else
    {
      rotation = rotation * (90.0 / MAX_X);
    }
    rotation = map(rotation, -90, 90, 0, 180);
    if (rotation > 125)
    {
      rotation = 125;
    }
    else if (rotation < 0)
    {
      rotation = 0;
    }
    break;
  case 'y':
    rotation = atan((a.acceleration.y) / sqrt(pow((a.acceleration.x), 2) + pow((a.acceleration.z), 2))) * (180.0 / 3.14);
    rotation -= offset_y;
    if (rotation < 0)
    {
      rotation = rotation * ((-90.0) / MIN_Y);
    }
    else
    {
      rotation = rotation * (90.0 / MAX_Y);
    }
    rotation = map(rotation, 90, -90, 0, 180);
    if (rotation > 125)
    {
      rotation = 125;
    }
    else if (rotation < 0)
    {
      rotation = 0;
    }
    break;
  case 'z':
    rotation = atan((a.acceleration.z) / sqrt(pow((a.acceleration.x), 2) + pow((a.acceleration.y), 2))) * (180.0 / 3.14);
    break;
  default:
    break;
  }

  return rotation;
}

float MPU_get_yaw(Adafruit_MPU6050 *mpu)
{
  static float yaw = 0;
  static unsigned long last_time = millis();
  unsigned long actual_time = millis();
  static float max = 0.34;
  static float min = -1.36;
  static float dif = abs(min) + abs(max);
  sensors_event_t a, g, temp;
  (*mpu).getEvent(&a, &g, &temp);
  yaw = yaw + (trunc(g.gyro.x - offset_x) * ((actual_time - last_time) * 0.001));
  last_time = actual_time;
  if (yaw > max)
  {
    yaw = max;
  }
  else if (yaw < min)
  {
    yaw = min;
  }

  float degrees = yaw * (180 / dif);

  if (yaw < 0)
  {
    degrees = yaw * (-90 / min);
  }
  else
  {
    degrees = yaw * (90 / max);
  }
  degrees = degrees + 90;

  return degrees;
}