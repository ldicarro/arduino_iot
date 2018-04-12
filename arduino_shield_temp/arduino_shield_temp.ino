// libraries for the sensors
#include <OneWire.h>
#include <DallasTemperature.h>

// libraries for the ethernet shield
#include <SPI.h>
#include <Ethernet.h>

// setting up variables for sensor breakout
const uint8_t dataPin = 3;
const uint8_t sensorResolution = 9;
const uint8_t sensorIndex = 0;

OneWire oneWire(dataPin);             // which pin to set the bus up on
DallasTemperature sensors(&oneWire);  // create variable to hold sensor
DeviceAddress sensorDeviceAddress;    // get the 64-bit address of the sensor

// each device must have an unique MAC address,
// change these to any valid hex value
uint8_t mac[6] = {0xAA,0x02,0xBC,0x04,0xDE,0x06};

// setting the ip address for the ethernet shield
IPAddress ip(192,168,1,110); // use an ip on your network
EthernetServer server(80);

// definitions for connecting to google.com
IPAddress googleServer(74,125,232,128);  // IP for Google so DNS is not used
//char googleServer[] = "www.google.com"; // not preferred but possible - takes too much memory

EthernetClient client;



void setup() {
  Serial.begin(9600);

  Ethernet.begin(mac,ip); // start the shield with the MAC address defined above
  server.begin(); // start the server
  Serial.print("Server started at: ");  // printing out the ethernet shield's ip address
  Serial.println(Ethernet.localIP());

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


  // trying to connect to the server using ip defined above and port 80
  if (client.connect(googleServer, 80)) {
    Serial.println("connected");
    client.println("GET /search?q=arduino HTTP/1.0");
    client.println();
  } else {
    Serial.println("connection failed");
  }

  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }

  // BONUS: the code below is so your computer can connect
  // to the Arduino using a plain old web browser. Use the
  // IP defined above in your browser to access the page.
  
  // initialize the connection from the outside world
  EthernetClient client = server.available();

  // if there is a connection, we can serve a page
  if(client)
  {
    boolean currentLineIsBlank = true;

    // the client (outside computer) will keep the
    // connection open until it is done receiving data
    while(client.connected())
    {
      if(client.available())
      {
        char c = client.read();
        Serial.write(c);

        if(c == '\n' && currentLineIsBlank)
        {
          // below is plain old HTML with headers
          // written out one line at a time
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println("Refresh: 30");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<body>");
          client.println("<p><img src='http://www.cthackerspace.com/wp-content/uploads/2015/07/CTH-LOGO-avatar-150x150.jpg' /></p>");
          client.println("<h1>Temperature:</h1><br>");
          client.print(temperatureInCelcius);
          client.println("&deg;C<br>");
          client.print(temperatureInFahrenheit);
          client.println("&deg;F");
          client.println("</body>");
          client.println("</html>");
          break;
        }

        if(c == '\n')
        {
          currentLineIsBlank = true;
        }
        else if(c != '\r')
        {
          currentLineIsBlank = false;
        }
      }
    }
  }

  delay(1);
  client.stop();
  // END BONUS!

  // print out values to serial monitor
  // print() = no line break
  // println() = line break
  Serial.print("Temp: ");
  Serial.print(temperatureInCelcius);
  Serial.print("C ");
  Serial.print(temperatureInFahrenheit);
  Serial.println("F");

  delay(15000);     // wait 15 seconds before we try again - don't want to overload the server
}

