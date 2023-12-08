#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LSM9DS1.h>
#include <Adafruit_Sensor.h>  // not used in this demo but required!
#include <cmath>

// i2c
Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();

#define LSM9DS1_SCK A5
#define LSM9DS1_MISO 12
#define LSM9DS1_MOSI A4
#define LSM9DS1_XGCS 6
#define LSM9DS1_MCS 5
// You can also use software SPI
//Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1(LSM9DS1_SCK, LSM9DS1_MISO, LSM9DS1_MOSI, LSM9DS1_XGCS, LSM9DS1_MCS);
// Or hardware SPI! In this case, only CS pins are passed in
//Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1(LSM9DS1_XGCS, LSM9DS1_MCS);


void setupSensor()
{
  // 1.) Set the accelerometer range
  //lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_2G);
  //lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_4G);
  //lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_8G);
  lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_16G);
  
  // 2.) Set the magnetometer sensitivity
  //lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
  //lsm.setupMag(lsm.LSM9DS1_MAGGAIN_8GAUSS);
  //lsm.setupMag(lsm.LSM9DS1_MAGGAIN_12GAUSS);
  //lsm.setupMag(lsm.LSM9DS1_MAGGAIN_16GAUSS);

  // 3.) Setup the gyroscope
  //lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);
  //lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_500DPS);
  //lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_2000DPS);
}


void setup() 
{
  Particle.connect();

  Serial.begin(115200);

  while (!Serial) {
    delay(1); // will pause Zero, Leonardo, etc until serial console opens
  }
  
  Serial.println("LSM9DS1 data read demo");
  
  // Try to initialise and warn if we couldn't detect the chip
  if (!lsm.begin())
  {
    Serial.println("Oops ... unable to initialize the LSM9DS1. Check your wiring!");
    while (1);
  }
  Serial.println("Found LSM9DS1 9DOF");

  // helper to just set the default scaling we want, see above!
  setupSensor();
}

int count = 0;
float max_x = 0;
float max_y = 0;
float max_z = 0;
float pmax_x = 0;
float pmax_y = 0;
float pmax_z = 0;

void loop() 
{
  lsm.read();  /* ask it to read in the data */ 

  /* Get a new sensor event */ 
  sensors_event_t a, m, g, temp;

  lsm.getEvent(&a, &m, &g, &temp); 


  if (fabs(a.acceleration.x) > fabs(max_x)) {
    max_x = fabs(a.acceleration.x);
  }

  if (fabs(a.acceleration.y) > fabs(max_y)) {
    max_y = fabs(a.acceleration.y);
  }

  if (fabs(a.acceleration.z) > fabs(max_z)) {
    max_z = fabs(a.acceleration.z);
  }

  if(count > 2000) {
    count = 0;
    if (fabs(max_x) > fabs(pmax_x)) {
      pmax_x = max_x;
    }
    if (fabs(max_y) > fabs(pmax_y)) {
      pmax_y = max_y;
    }
    if (fabs(max_z) > fabs(pmax_z)) {
      pmax_z = max_z;
    }
    Serial.println("Max values over last 2000 ticks: "); 
    Serial.print("max_x: "); Serial.print(max_x); Serial.print("  max_y: "); Serial.print(max_y); Serial.print("  max_z: "); Serial.println(max_z);
    Particle.publish("max_x", String(max_x));
    Particle.publish("max_y", String(max_y));
    Particle.publish("max_z", String(max_z));
    Serial.println("");
    Serial.println("Max values over all ticks: ");
    Serial.print("pmax_x: "); Serial.print(pmax_x); Serial.print("  pmax_y: "); Serial.print(pmax_y); Serial.print("  pmax_z: "); Serial.println(pmax_z);
    Serial.println("");
    Serial.println("-----------------------------------------------------------------------------------------------------");
    Serial.println("");
    max_x = 0;
    max_y = 0;
    max_z = 0;
  }
  count++;
}