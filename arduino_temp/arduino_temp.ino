// libraries for the sensors
#include <OneWire.h>
#include <DallasTemperature.h>

// setting up variables for sensor breakout
const uint8_t dataPin = 3;
const uint8_t sensorResolution = 9;
const uint8_t sensorIndex = 0;

OneWire oneWire(dataPin);             // which pin to set the bus up on
DallasTemperature sensors(&oneWire);  // create variable to hold sensor
DeviceAddress sensorDeviceAddress;    // get the 64-bit address of the sensor


void setup() {
  Serial.begin(9600);

  sensors.begin(); // init the one-wire bus
  sensors.getAddress(sensorDeviceAddress, 0); // inits the the sensor at address 0 (the only one we have)
  sensors.setResolution(sensorDeviceAddress,sensorResolution);  // setting how sensitive the returned data is in bits (9, 10, 11, or 12 bits)
}

void loop() {
  // get the data from all sensors connected to the Arduino
  sensors.requestTemperatures();

  // convert the data to human readable values, the values are converted to strings from floats
  // so they can be printed to the Serial Monitor
  String temperatureInCelcius = String(sensors.getTempCByIndex(sensorIndex),2);
  String temperatureInFahrenheit = String(sensors.getTempFByIndex(sensorIndex),2);


  // print out values to serial monitor
  // print() = no line break
  // println() = line break
  Serial.print("Temp: ");
  Serial.print(temperatureInCelcius);
  Serial.print("C ");
  Serial.print(temperatureInFahrenheit);
  Serial.println("F");

  delay(1000);     // wait 1 second
}

