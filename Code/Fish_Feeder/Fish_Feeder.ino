#define BLYNK_TEMPLATE_ID "TMPLqWFFH_h4"
#define BLYNK_DEVICE_NAME "Aquarium"
#define BLYNK_AUTH_TOKEN "1pnp4cq3j9jCDYyV2nMRyV9IapDnY2Nd"
#define BLYNK_PRINT Serial

#include "Arduino.h"
#include "ArduinoJson.h"

#include "Adafruit_GFX.h"
#include "Adafruit_ST7789.h"
#include "SPI.h"
#include "Adafruit_I2CDevice.h"

#include "BlynkSimpleEsp32.h"

#include "Button.h"
#include "Light.h"
#include "Sensor.h"
#include "MyServo.h"
#include "Location.h"

#include "WiFiManager.h"

#define TFT_CS 17
#define TFT_RST 4
#define TFT_DC 15

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

WiFiManager myWiFi;
Location myLocation;

MyServo myServo(19);

Button feedFishButton(27);

Button setFeedIntervalButton(12);
bool feedTimeButtonDown = false;
unsigned long feedTimeButtonPressTime; // milliseconds button is pressed

Button increaseFeedIntervalButton(13);
Button decreaseFeedIntervalButton(14);

Light aquriumLamp(16);
Sensor lampSensor(36);

Sensor foodSensor(39);

const char *API_KEY = "82f9fab01dcdbf66477cc29f884573a4"; // API key for the Weather

const char *ssid = ""; // MUST BE FILLED
const char *pass = ""; // MUST BE FILLED

const u16_t APICallInterval = 30000; // Check the weather every x milliseconds.
const u16_t lightDuration = 30000;   // Light the aquarium for x milliseconds

void setLocation(); // Gets the geolocation needed for the OpenWeather API

void getWeather();
void showWeather();

void monitorFeedingInterval(); // Checks wether its time to feed the fish
void monitorButtons(); // Monitors the states of the four buttons

void setFeedInterval(); // Allows user to change the default feeding time
void feedFish(); 

unsigned long getFeedIntervalMills(const u8_t &hours); // Returns the given hour as a milliseconds
void drawToScreen(char *text, uint16_t color);


int8_t feedingTimeInterval = 2; // Default feeding time is twice a day.

unsigned long dailyFeed = 0; // The feeding time as milliseconds
u_int8_t feedingDuration = 30;   // Each feeding interval lasts for 30 seconds
bool changedFeedingTime = true; // Feed once when the intervall has changed 

float temperature = 0.0; // Placeholder for the temperature receved from the API


unsigned long prevAPIMills = 0;
bool recallAPI = true; // Runs the API once after booting

unsigned long prevFeedMills = 0;
unsigned long currentTime = 0; // used to count millis() from start-up



// Used to create a simple progressbar for
u_int16_t cursorX = 110;
u_int16_t cursorY = 0;

BLYNK_WRITE(V1) // This works with Apples Siri!
{

  if (param.asInt())
    feedFish(); // Only feed when HIGH

}

void setup()
{
  Serial.begin(115200);

  tft.init(135, 240);
  tft.fillScreen(ST77XX_BLACK);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println("Connecting to WiFi");

  drawToScreen("Connceting to WiFi", ST77XX_WHITE);

  myWiFi.setup(ssid, pass);

  while (!myWiFi.connected())
  {
    // Showing a simple progress bar
    tft.setTextSize(1);
    tft.setCursor(cursorX, cursorY);

    tft.setTextColor(ST77XX_WHITE);
    tft.setTextWrap(true);
    tft.print(".");

    if (tft.getCursorY() > cursorY)
    {
      cursorY += 8;
      cursorX = 0;
    }

    cursorX += 8;
    delay(250);
  }

  tft.fillScreen(ST77XX_BLACK);

  drawToScreen("Successfully connected to WiFI!", ST77XX_GREEN);

  Serial.print("Your IP is: ");
  Serial.print(myWiFi.getIP()); // The IP used to get the weather

  setLocation(); // Here we get the lattitude & longitude needed for OpenWeather API

  dailyFeed = getFeedIntervalMills(feedingTimeInterval); // Default feed time is twice a day

  Serial.print("dailyFeed: ");
  Serial.print(dailyFeed);

  delay(2000);
}


void loop()
{

  Blynk.run();
  currentTime = millis(); // Used mills insted of delay to keep this loop as tight as possible, dont want to block the buttons

  showWeather();
  monitorFeedingInterval();
  monitorButtons();
}



void setLocation()
{

  auto myIP = myWiFi.getIP();

  auto url = "http://ip-api.com/json/" + myIP; // Getting our Geo-loaction data, witch is needed for the weather API
  Serial.println("URL: " + url);

  auto json = myWiFi.getJSON(url); // The payload as JSON object

  // Extrating the data needed for the Location object
  String countryCode = json["countryCode"].as<String>();
  const char *city = json["city"];
  const float lattitude = json["lat"].as<float>();
  const float longitude = json["lon"].as<float>();

  // Printing to the monitor to check the results are as expected
  Serial.print("CountryCode: ");
  Serial.println(countryCode);

  Serial.print("City: ");
  Serial.println(city);

  Serial.print("Lattitude: ");
  Serial.println(lattitude);
  tft.write(longitude);

  Serial.print("Longitude: ");
  Serial.println(longitude);

  // Setting up the Object
  myLocation.setup(countryCode, city, lattitude, longitude);
}

void getWeather()
{

  //  Setting up the URL to get the weather, here i use the previously acquired data stored myLocation
  auto url = "http://api.openweathermap.org/data/2.5/weather?units=metric&lat=" + String(myLocation.m_lat) + "&lon=" + String(myLocation.m_lon) + "&appid=" + API_KEY;

  // Testing the URL...
  Serial.print("Weather URL: ");
  Serial.println(url);

  auto json = myWiFi.getJSON(url);
  temperature = json["main"]["temp"].as<float>(); // Casting to float
}

void showWeather()
{
  if (currentTime - prevAPIMills >= APICallInterval || recallAPI) // Run once to set the weather, otherwise one would have to wait for 30 seconds before displaying the weather
  {
    getWeather();
    Serial.println("The current temp is: ");
    Serial.println(temperature);

    tft.fillScreen(ST77XX_BLACK);

    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextWrap(true);
    tft.setCursor(0, 0);

    tft.printf("%s\n%.2fC", myLocation.m_city, temperature);
    prevAPIMills = currentTime;

    recallAPI = false;
  }
}

void monitorFeedingInterval()
{
  if (currentTime - prevFeedMills >= dailyFeed || changedFeedingTime) 
  {
    prevFeedMills = currentTime;
    changedFeedingTime = false; // wait until next interval
    feedFish();
    
  }
}

void monitorButtons()
{
  // Checking if any buttons have been pressed
  feedFishButton.readState();
  setFeedIntervalButton.readState();

  if (feedFishButton.isPressed())
  {
    feedFish();
  }

  if (setFeedIntervalButton.stateChanged())
  {

    if (setFeedIntervalButton.isPressed())
    {
      feedTimeButtonPressTime = currentTime;
      feedTimeButtonDown = true;
    }
    else
    {
      feedTimeButtonDown = false;
    }
    delay(50); // Try and reduces "bouncyness"
  }

  if (feedTimeButtonDown == true && currentTime - feedTimeButtonPressTime >= 3000) // Pressed the button for 3 second, show user the setup
  {
    feedTimeButtonDown = false;
    setFeedInterval();
  }

  // update the state
  feedFishButton.updateState();
  setFeedIntervalButton.updateState();
}

void setFeedInterval()
{

  bool showSetup = true;

  tft.fillScreen(ST77XX_BLACK);
  Serial.println("In setup-loop");

  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("Set feeding interval");

  while (showSetup)
  {
    
    // read button states
    setFeedIntervalButton.readState();
    increaseFeedIntervalButton.readState();
    decreaseFeedIntervalButton.readState();

    if (increaseFeedIntervalButton.isPressed())
    {
      Serial.println("increased");
      feedingTimeInterval++;

      if (feedingTimeInterval > 3)
        feedingTimeInterval = 3; // Avoid exceeding max feedtime
    }

    else if (decreaseFeedIntervalButton.isPressed())
    {

      Serial.println("decreased");
      feedingTimeInterval--;

      if (feedingTimeInterval < 1)
        feedingTimeInterval = 1; // Avoid subceeding min feedtime
    }

    if (setFeedIntervalButton.isPressed())
      showSetup = false; // Exit function

    tft.setCursor(0, 15);
    tft.fillRect(0, 15, 20, 20, ST77XX_BLACK); //  A placeholder for the number
    tft.printf("%d", feedingTimeInterval);
    delay(50); // Avoid bouncyness

    setFeedIntervalButton.updateState();
    increaseFeedIntervalButton.updateState();
    decreaseFeedIntervalButton.updateState();
  }

  Serial.print("Exiting setup-loop");

  dailyFeed = getFeedIntervalMills(feedingTimeInterval); // Set the feed interval

  Serial.println("Daily feed set to: ");
  Serial.println(feedingTimeInterval);
  Serial.println("Daily feed mills:");
  Serial.println(dailyFeed);

  tft.fillScreen(ST77XX_BLACK);

  const char *msg = feedingTimeInterval == 1 ? "Set to once a day!" : feedingTimeInterval == 2 ? "Set to twice a day!"
                                                                                               : "Set to 3 times a day!";

  tft.setCursor(0, 0);
  tft.printf("%s", msg);

  recallAPI = true; // Make sure to "re-call" our weather when we get back to the main loop
  
  changedFeedingTime = true; 

  delay(4000); // Wait 4 sec before re-entering main loop
}


unsigned long getFeedIntervalMills(const u8_t &hours)
{
  return (24 / hours) * 3600000; // divide a day to the hours, multipliy the result by miliseconds 
}

void feedFish()
{
  tft.fillScreen(ST77XX_BLACK);

  auto feeding = true;
  bool runServo = true;

  unsigned long prevTime = 0;
  u8_t seconds = 0;

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.setCursor(0, 0);
  tft.printf("Feeding fish");

  while (feeding)
  {

    if (millis() - prevTime >= 1000 || runServo) // Checking the sensors each second, run first before servo to make sure there is enough food
    {

      if (foodSensor.reading() > 200) // Light detected, AKA no more food, so break out of loop. Send msg to owner to buy more? :)
      {
        feeding,runServo = false; // Dont run servo, quit looop
        Serial.println("Container is empty!");
        break;
      }

      //  its bright turn off the lamp, else turn on

      if (lampSensor.reading() > 150)
        aquriumLamp.turnOff();
      else
        aquriumLamp.turnOn();

      Serial.println("Luminosity: ");
      Serial.println(lampSensor.reading());

      Serial.println("FoodLevelPin: ");
      Serial.println(foodSensor.reading());
      prevTime = millis();

      seconds++; // update the seconds

      // Exit loop when reached feedingDuration
      if (seconds == feedingDuration)
        feeding = false;
    }

    // Only want to run the servo once per intervall
    if (runServo)
    {
      myServo.operate();
      runServo = false;
    }
  }

  aquriumLamp.turnOff(); // Make sure to turn off the lights
  recallAPI = true;      // Reload the weather API
}

void drawToScreen(char *text, uint16_t color)
{

  tft.setRotation(3);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

