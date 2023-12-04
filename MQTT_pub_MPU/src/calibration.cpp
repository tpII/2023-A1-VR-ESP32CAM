// #include <ESP32Servo.h>
// #include <Adafruit_MPU6050.h>
// #include <Adafruit_Sensor.h>
// #include <Wire.h>

// #include <WiFi.h>
// #include <PubSubClient.h>

// #define MAX_X 82.37
// #define MIN_X -93.55

// #define MAX_Y 82.37
// #define MIN_Y -93.55

// void MPU_config(Adafruit_MPU6050 *);
// float MPU_calibrate(Adafruit_MPU6050 *, char axis);
// void MPU_calibrate_rotation(Adafruit_MPU6050 *mpu, float *offset, char axis);
// void MPU_get_max(Adafruit_MPU6050 *mpu, char axis);

// void SERVO_config(Servo *myservo);

// Adafruit_MPU6050 mpu;

// Servo myservo; // create servo object to control a servo

// int pos = 0;
// #if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
// int servoPin = 17;
// #elif defined(CONFIG_IDF_TARGET_ESP32C3)
// int servoPin = 7;
// #else
// int servoPin = 18;
// #endif

// float rotation = 0;
// int pos_servo = 90;
// float offset_x;
// float offset_y;
// float offset_z;

// void setup()
// {
//     SERVO_config(&myservo);

//     Serial.begin(115200);

//     MPU_config(&mpu);

//     MPU_calibrate_rotation(&mpu, &offset_x, 'x');
//     MPU_calibrate_rotation(&mpu, &offset_y, 'y');
//     MPU_calibrate_rotation(&mpu, &offset_y, 'z');
// }

// unsigned long lastMillis;
// unsigned long currentMillis;

// void loop()
// {
//     MPU_get_max(&mpu, 'x');

//     MPU_get_max(&mpu, 'y');

//     MPU_get_max(&mpu, 'z');
//     while (1)
//         ;

//     delay(50);
// }

// void MPU_calibrate_rotation(Adafruit_MPU6050 *mpu, float *offset, char axis)
// {
//     // Se toman 100 muestras de la velocidad angular del giroscopio en x y se promedian para devolver un offset de medicion
//     Serial.println("\n\nMPU6050 calibration...");

//     float total = 0;
//     float accel_ang_axis;

//     for (int i = 0; i < 100; i++)
//     {
//         sensors_event_t a, g, temp;
//         (*mpu).getEvent(&a, &g, &temp);
//         switch (axis)
//         {
//         case 'x':
//             accel_ang_axis = atan((a.acceleration.x) / sqrt(pow((a.acceleration.y), 2) + pow((a.acceleration.z), 2))) * (180.0 / 3.14);
//             break;
//         case 'y':
//             accel_ang_axis = atan((a.acceleration.y) / sqrt(pow((a.acceleration.x), 2) + pow((a.acceleration.z), 2))) * (180.0 / 3.14);
//             break;
//         case 'z':
//             accel_ang_axis = atan((a.acceleration.z) / sqrt(pow((a.acceleration.x), 2) + pow((a.acceleration.y), 2))) * (180.0 / 3.14);
//             break;
//         default:
//             break;
//         }
//         total = total + accel_ang_axis;
//         delay(10);
//     }

//     Serial.println("\nDone with offset " + String(axis) + ": " + String(total / 100) + "\n\n");

//     *offset = total / 100;
// }

// float MPU_calibrate(Adafruit_MPU6050 *mpu, char axis)
// {
//     // Se toman 200 muestras de la velocidad angular del giroscopio en x y se promedian para devolver un offset de medicion
//     Serial.println("MPU6050 calibration...");

//     float total = 0;

//     for (int i = 0; i < 100; i++)
//     {
//         sensors_event_t a, g, temp;
//         (*mpu).getEvent(&a, &g, &temp);
//         // use axis to select the axis to calibrate
//         switch (axis)
//         {
//         case 'x':
//             total = total + a.acceleration.x;
//             break;
//         case 'y':
//             total = total + a.acceleration.y;
//             break;
//         case 'z':
//             total = total + a.acceleration.z;
//             break;
//         }
//         delay(10);
//     }

//     Serial.println("Done with offset " + String(axis) + ": " + String(total / 100));

//     return total / 100;
// }

// void MPU_config(Adafruit_MPU6050 *mpu)
// {
//     if (!(*mpu).begin())
//     {
//         Serial.println("Failed to find MPU6050 chip");
//         while (1)
//         {
//             delay(10);
//         }
//     }
//     Serial.println("MPU6050 Found!");

//     (*mpu).setAccelerometerRange(MPU6050_RANGE_8_G);
//     (*mpu).setGyroRange(MPU6050_RANGE_500_DEG);
//     (*mpu).setFilterBandwidth(MPU6050_BAND_21_HZ);

//     Serial.print("Accelerometer range set to: ");
//     switch ((*mpu).getAccelerometerRange())
//     {
//     case MPU6050_RANGE_2_G:
//         Serial.println("+-2G");
//         break;
//     case MPU6050_RANGE_4_G:
//         Serial.println("+-4G");
//         break;
//     case MPU6050_RANGE_8_G:
//         Serial.println("+-8G");
//         break;
//     case MPU6050_RANGE_16_G:
//         Serial.println("+-16G");
//         break;
//     }
//     Serial.print("Gyro range set to: ");
//     switch ((*mpu).getGyroRange())
//     {
//     case MPU6050_RANGE_250_DEG:
//         Serial.println("+- 250 deg/s");
//         break;
//     case MPU6050_RANGE_500_DEG:
//         Serial.println("+- 500 deg/s");
//         break;
//     case MPU6050_RANGE_1000_DEG:
//         Serial.println("+- 1000 deg/s");
//         break;
//     case MPU6050_RANGE_2000_DEG:
//         Serial.println("+- 2000 deg/s");
//         break;
//     }
//     Serial.print("Filter bandwidth set to: ");
//     switch ((*mpu).getFilterBandwidth())
//     {
//     case MPU6050_BAND_260_HZ:
//         Serial.println("260 Hz");
//         break;
//     case MPU6050_BAND_184_HZ:
//         Serial.println("184 Hz");
//         break;
//     case MPU6050_BAND_94_HZ:
//         Serial.println("94 Hz");
//         break;
//     case MPU6050_BAND_44_HZ:
//         Serial.println("44 Hz");
//         break;
//     case MPU6050_BAND_21_HZ:
//         Serial.println("21 Hz");
//         break;
//     case MPU6050_BAND_10_HZ:
//         Serial.println("10 Hz");
//         break;
//     case MPU6050_BAND_5_HZ:
//         Serial.println("5 Hz");
//         break;
//     }
// }

// void MPU_get_max(Adafruit_MPU6050 *mpu, char axis)
// {
//     Serial.println("Mueve en el eje " + String(axis));

//     float min = 0;
//     float max = 0;

//     int aux = 0;

//     float accel_ang_x = 0;
//     float accel_ang_y = 0;
//     float accel_ang_z = 0;

//     // Tomo muestras hasta que recibe algo por serial, luego imprimo el maximo y el minimo
//     while (++aux < 300)
//     {
//         sensors_event_t a, g, temp;
//         (*mpu).getEvent(&a, &g, &temp);
//         switch (axis)
//         {
//         case 'x':
//             accel_ang_x = atan((a.acceleration.x) / sqrt(pow((a.acceleration.y), 2) + pow((a.acceleration.z), 2))) * (180.0 / 3.14);
//             if ((accel_ang_x - offset_x) > max)
//             {
//                 max = (accel_ang_x - offset_x);
//             }
//             else if ((accel_ang_x - offset_x) < min)
//             {
//                 min = (accel_ang_x - offset_x);
//             }
//             break;
//         case 'y':
//             accel_ang_y = atan((a.acceleration.y) / sqrt(pow((a.acceleration.x), 2) + pow((a.acceleration.z), 2))) * (180.0 / 3.14);
//             if ((accel_ang_y - offset_y) > max)
//             {
//                 max = (accel_ang_y - offset_y);
//             }
//             else if ((accel_ang_y - offset_y) < min)
//             {
//                 min = (accel_ang_y - offset_y);
//             }
//             break;
//         case 'z':
//             accel_ang_z = atan((a.acceleration.z) / sqrt(pow((a.acceleration.x), 2) + pow((a.acceleration.y), 2))) * (180.0 / 3.14);
//             if ((accel_ang_z - offset_z) > max)
//             {
//                 max = (accel_ang_z - offset_z);
//             }
//             else if ((accel_ang_z - offset_z) < min)
//             {
//                 min = (accel_ang_z - offset_z);
//             }
//             break;
//         default:
//             break;
//         }

//         delay(10);
//     }

//     Serial.println("Max " + String(axis) + ": " + String(max));
//     Serial.println("Min " + String(axis) + ": " + String(min));
// }

// void SERVO_config(Servo *myservo)
// {
//     // Allow allocation of all timers
//     ESP32PWM::allocateTimer(0);
//     ESP32PWM::allocateTimer(1);
//     ESP32PWM::allocateTimer(2);
//     ESP32PWM::allocateTimer(3);
//     (*myservo).setPeriodHertz(50);        // standard 50 hz servo
//     (*myservo).attach(servoPin, 0, 3000); // attaches the servo on pin 18 to the servo object
//                                           // for an accurate 0 to 180 sweep
// }