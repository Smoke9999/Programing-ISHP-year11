// RTC
#include "RTClib.h"

RTC_PCF8523 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// EINK
#include "Adafruit_ThinkInk.h"

// SD card
#include "FS.h"
#include "SD.h"

#define EPD_CS      15
#define EPD_DC      33
#define SRAM_CS     32
#define EPD_RESET   -1 // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY    -1 // can set to -1 to not use a pin (will wait a fixed delay)

// 2.13" Monochrome displays with 250x122 pixels and SSD1675 chipset
ThinkInk_213_Mono_B72 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);
// Motor sheild
#include <Adafruit_MotorShield.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor = AFMS.getMotor(4);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }

  // RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  // The following line can be uncommented if the time needs to be reset.
  //  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  rtc.start();

  //SD card
  setupSD();



  //EINK
  display.begin(THINKINK_MONO);
  display.clearBuffer();


  setupSD();
  AFMS.begin();
  myMotor->setSpeed(255);
  logEvent("System Initialisation...");
}


void loop() {

  // Gets the current date and time, and writes it to the Eink display.
  String currentTime = getDateTimeAsString();

  drawText("The Current Time and\nDate is", EPD_BLACK, 2, 0, 0);

  // writes the current time on the bottom half of the display (y is height)
  drawText(currentTime, EPD_BLACK, 2, 0, 75);

  // Draws a line from the leftmost pixel, on line 50, to the rightmost pixel (250) on line 50.
  display.drawLine(0, 50, 250, 50, EPD_BLACK);
  display.display();


  // waits 180 seconds (3 minutes) as per guidelines from adafruit.
  delay(180000);
  display.clearBuffer();
}

void drawText(String text, uint16_t color, int textSize, int x, int y) {
  display.setCursor(x, y);
  display.setTextColor(color);
  display.setTextSize(textSize);
  display.setTextWrap(true);
  display.print(text);

}

String getDateTimeAsString() {
  DateTime now = rtc.now();

  //Prints the date and time to the Serial monitor for debugging.
  /*
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
  */

  // Converts the date and time into a human-readable format.
  char humanReadableDate[20];
  sprintf(humanReadableDate, "%02d:%02d:%02d %02d/%02d/%02d",  now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());

  return humanReadableDate;
}

void setupSD() {
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("SD Started");
}



void logEvent(String dataToLog) {
  /*
     Log entries to a file on an SD card.
  */
  // Get the updated/current time
  DateTime rightNow = rtc.now();

  // Open the log file
  File logFile = SD.open("/logEvents.csv", FILE_APPEND);
  if (!logFile) {
    Serial.print("Couldn't create log file");
    abort();
  }

  // Log the event with the date, time and data
  logFile.print(rightNow.year(), DEC);
  logFile.print(",");
  logFile.print(rightNow.month(), DEC);
  logFile.print(",");
  logFile.print(rightNow.day(), DEC);
  logFile.print(",");
  logFile.print(rightNow.hour(), DEC);
  logFile.print(",");
  logFile.print(rightNow.minute(), DEC);
  logFile.print(",");
  logFile.print(rightNow.second(), DEC);
  logFile.print(",");
  logFile.print(dataToLog);

  // End the line with a return character.
  logFile.println();
  logFile.close();
  Serial.print("Event Logged: ");
  Serial.print(rightNow.year(), DEC);
  Serial.print(",");
  Serial.print(rightNow.month(), DEC);
  Serial.print(",");
  Serial.print(rightNow.day(), DEC);
  Serial.print(",");
  Serial.print(rightNow.hour(), DEC);
  Serial.print(",");
  Serial.print(rightNow.minute(), DEC);
  Serial.print(",");
  Serial.print(rightNow.second(), DEC);
  Serial.print(",");
  Serial.println(dataToLog);
}
