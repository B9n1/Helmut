#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LSM9DS1.h>
#include <Adafruit_Sensor.h>  // not used in this demo but required!
#include <cmath>
#include <HttpClient.h>

HttpClient http;
http_request_t request;
http_response_t response;

http_header_t headers[] = {
    { "Accept" , "*/*"},
    { "Content-Type", "application/json" },
    { NULL, NULL }
};

// i2c
Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();

#define LSM9DS1_SCK A5
#define LSM9DS1_MISO 12
#define LSM9DS1_MOSI A4
#define LSM9DS1_XGCS 6
#define LSM9DS1_MCS 5


void setupSensor()
{

  lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_16G);

}



void setup() 
{
  Particle.connect();

  Serial.begin(115200);

  while (!Serial) {
    delay(1); // will pause Zero, Leonardo, etc until serial console opens
  }
  
  // Try to initialise and warn if we couldn't detect the chip
  if (!lsm.begin())
  {
    Serial.println("Unable to initialize the LSM9DS1. Check your wiring!");
    while (1);
  }
  Serial.println("Found LSM9DS1 9DOF");

  // helper to just set the default scaling we want, see above!
  setupSensor();
}

//helper function to send sensor data to webserver
void sendSensorData(float max_x, float max_y, float max_z) {
  request.hostname = "192.168.2.111"; //needs to be changed to the IP of the server
  request.port = 3002;
  request.path = "/sensor-data";
  request.body = "{\"max_x\":\"" + String(max_x) + "\", \"max_y\":\"" + String(max_y) + "\", \"max_z\":\"" + String(max_z) + "\"}";
  http.post(request, response, headers);
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

  if(count > 300) {
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
    sendSensorData(max_x, max_y, max_z);
    max_x = 0;
    max_y = 0;
    max_z = 0;
  }
  count++;
}