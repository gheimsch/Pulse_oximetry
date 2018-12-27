#include <U8g2lib.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

#define REPORTING_PERIOD_MS     500

 U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
//PulseOximeter pox;
MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

//byte pulseLED = 13; //Must be on PWM pin
//byte readLED = 11; //Blinks with each data read
//
bool calculation_complete=false;
bool calculating=false;
bool initialized=false;
byte beat=0;

void show_beat() 
{
  u8g2.setFont(u8g2_font_cursor_tf);
  u8g2.setCursor(8,10);
  if (beat==0) {
    u8g2.print("_");
    beat=1;
  } 
  else
  {
    u8g2.print("^");
    beat=0;
  }
  u8g2.sendBuffer();
}

void initial_display() 
{
  if (not initialized) 
  {
    u8g2.clearBuffer();
    show_beat();
    u8g2.setCursor(24,12);
    u8g2.setFont(u8g2_font_profont15_mr);
    u8g2.print("Place finger");  
    u8g2.setCursor(0,30);
    u8g2.print("on the sensor");
    u8g2.sendBuffer(); 
    initialized=true;
  }
}

void display_calculating(int j)
{
  if (not calculating) {
    u8g2.clearBuffer();
    calculating=true;
    initialized=false;
  }
  show_beat();
  u8g2.setCursor(24,12);
  u8g2.setFont(u8g2_font_profont15_mr);
  u8g2.print("Measuring..."); 
  u8g2.setCursor(0,30);
  u8g2.print(beatsPerMinute);
  u8g2.print(" Bpm");
  u8g2.sendBuffer();
}

void display_values()
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_profont15_mr);
 
  u8g2.setCursor(0,30);
  u8g2.print(beatsPerMinute);
  u8g2.print(" Bpm _ ");
  u8g2.setCursor(65,30);  
  u8g2.print(beatAvg);
  u8g2.sendBuffer();
}

void setup()
{
    Serial.begin(115200);
    
//    pinMode(pulseLED, OUTPUT);
//    pinMode(readLED, OUTPUT);

    u8g2.begin();

    // Initialize sensor
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
    {
      Serial.println(F("MAX30105 was not found. Please check wiring/power."));
      while (1);
    }    

    particleSensor.setup(); //Configure sensor with default settings
    particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
    particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
    
    initial_display();
}


void loop()
{

  long irValue = particleSensor.getIR();
  float temperature = particleSensor.readTemperature();
//  digitalWrite(readLED, !digitalRead(readLED)); //Blink onboard LED with every data read

  if (checkForBeat(irValue) == true)
  {
    calculation_complete=true;
    calculating=false;
    initialized=false;

//    digitalWrite(pulseLED, !digitalRead(pulseLED)); //Blink onboard LED with every Beat
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    if (delta < 10000) {
      beatsPerMinute = 60 / (delta / 1000.0);
    } else {
      calculation_complete=false;
      beatsPerMinute=0;
      initial_display();
    }
    
    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
      display_values();
    }
  } else {
    if (irValue < 50000) {
      calculating=false;
      initial_display();
    } else {
      display_calculating(5);
    }
  }

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);
  Serial.print(", initialized=");
  Serial.print(initialized);
  Serial.print(", calculating=");
  Serial.print(calculating);
  Serial.print(", calculation_complete=");
  Serial.print(calculation_complete);
  Serial.println();



}
