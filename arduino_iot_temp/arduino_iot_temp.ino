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

OneWire oneWire(dataPin);
DallasTemperature sensors(&oneWire);
DeviceAddress sensorDeviceAddress;

// each device must have an unique MAC address,
// change these to any valid hex value
uint8_t mac[6] = {0xAA,0x02,0xBC,0x04,0xDE,0x06};

// setting the ip address for the ethernet shield
IPAddress ip(192,168,254,50); // use an ip on your network
EthernetServer server(80);

// definitions for connecting to io.adafruit.com
IPAddress ioServer(52,5,238,97);
EthernetClient ioClient;


void setup() {
  Serial.begin(9600);

  Ethernet.begin(mac); // start the shield with the MAC address defined above
  server.begin(); // start the server
  Serial.print("Server started at: ");  // printing out the ethernet shield's ip address
  Serial.println(Ethernet.localIP());

  sensors.begin(); // start the sensor
  sensors.getAddress(sensorDeviceAddress, 0); // this is useful if there are multiple sensors
  sensors.setResolution(sensorDeviceAddress,sensorResolution);  // setting how sensitive the returned data is
}

void loop() {
  // get the data from the sensor
  sensors.requestTemperatures();
  // convert the data to human readable values
  String temperatureInCelcius = String(sensors.getTempCByIndex(sensorIndex),2);
  String temperatureInFahrenheit = String(sensors.getTempFByIndex(sensorIndex),2);


  // trying to connect to the server using ip defined above and port 80
  if(ioClient.connect(ioServer,80))
  {
    // building the data we will send to the io.adafruit.com
    String data = String("{\"value\": \"" + temperatureInFahrenheit + "\" }");

    // printing out POST request with headers and data
    ioClient.println("POST /api/feeds/[name of feed]/data HTTP/1.1"); // replace [name of feed] with your own feed name
    ioClient.println("Host: io.adafruit.com");
    ioClient.println("Content-Type: application/json");
    ioClient.println("X-AIO-Key: xxxxx"); // use the key generated on io.adafruit.com
    ioClient.print("Content-Length: ");
    ioClient.println(data.length());
    ioClient.println();
    ioClient.println(data);
  }
  else
  {
    // could not find server
    Serial.println("error connecting to server");
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
          client.println("<head>");
          client.println("<link rel='stylesheet' href='link/to/your/external/stylesheet.css'>");
          client.println("</head>");
          client.println("<body>");
          client.println("<div class='hero'>");
          client.println("<fieldset class='hero-container'>");
          client.println("<p><img src='http://www.cthackerspace.com/wp-content/uploads/2015/07/CTH-LOGO-avatar-150x150.jpg' /></p>");
          client.println("<h1>Temperature:</h1><br>");
          client.print(temperatureInCelcius);
          client.println("&deg;C<br>");
          client.print(temperatureInFahrenheit);
          client.println("&deg;F");
          client.println("</fieldset>");
          client.println("</div>");
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
  Serial.print("Temp: ");
  Serial.print(temperatureInCelcius);
  Serial.print("C ");
  Serial.print(temperatureInFahrenheit);
  Serial.println("F");

  ioClient.stop();  // stop the client to close the connection
  delay(15000);     // wait 15 seconds before we try again - don't want to overload the server
}

