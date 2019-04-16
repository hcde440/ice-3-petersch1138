
/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Adafruit_MPL115A2.h>
/************************ Example Starts Here *******************************/

// set up the 'temperature', 'pressure', and 'humidity' feeds
AdafruitIO_Feed *temperature = io.feed("temperature");
AdafruitIO_Feed *humidity = io.feed("humidity");
AdafruitIO_Feed *pressure = io.feed("pressure");

float pre; // initialize variables for pressure, temp, humidity
float tem;
float hum;
// pin connected to DH22 data line
#define DATA_PIN 12

// create DHT22 instance
DHT_Unified dht(DATA_PIN, DHT22);
Adafruit_MPL115A2 mpl; // initialize mpl sensor

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // initialize oled 


// Peter Schultz HCDE 440 April 4/16/19
// This script will get the temp, pressure, and humidity from the dht and mpl sensors
// and display them both on an OLED display, and send them to be displayed on adafruit IO

void setup() {
  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);
  Serial.print("This board is running: ");
  Serial.println(F(__FILE__));
  Serial.print("Compiled: ");
  Serial.println(F(__DATE__ " " __TIME__));
   
  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  Serial.println("initializing dht");
  dht.begin(); // begin dht (non i2c) sensor

  Serial.println("initializing mpl");
  mpl.begin(); // begin mpl (i2c) sensor
  pre = 0;
  hum = 0;
  tem = 0;

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  sensors_event_t event;// initialize sensor event

  dht.humidity().getEvent(&event);// gets humidity from dht sensor
  hum = event.relative_humidity; // gets humidity from sensor event
  Serial.print("humidity: ");
  Serial.print(hum);
  Serial.println("%");
  
  dht.temperature().getEvent(&event);// gets temp from dht sensor
  tem = event.temperature;// gets temp from sensor event

  Serial.print("celsius: ");
  Serial.print(tem);
  Serial.println("C");
  pre = mpl.getPressure();  // gets pressure from mpl sensor
  Serial.print("Pressure: "); Serial.print(pre, 4); Serial.println(" kPa");

  pressure->save(pre); // save variables to be sent to adafruit IO
  temperature->save(tem);
  humidity->save(hum);

  Serial.println("attempting to display");

  testscrolltext(); // displays text on OLED
  delay(5000);
}

void testscrolltext(void) { // this will display the tempa nd pressure on the OLED
  String stringPre = String(pre);
  String stringTem = String(tem);// converts to string
  
  display.clearDisplay();//clears display 

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.println(stringTem + " C " + stringPre + " kPa");
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(1000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(1000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(1000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(1000);
  display.stopscroll();
  delay(1000);
}
