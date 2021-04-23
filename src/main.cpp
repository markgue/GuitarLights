#include <Arduino.h>
#include <FastLED.h>
#include <math.h>
#include <arduinoFFT.h>
#define NUM_LEDS 100
#define DATA_PIN 7
#define ANALOG_PIN 5
#define SAMPLES 64             //SAMPLES-pt FFT. Must be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 2400 //Ts = Based on Nyquist, must be 2 times the highest expected frequency.

arduinoFFT FFT = arduinoFFT();
CRGB leds[NUM_LEDS];

unsigned int samplingPeriod;
unsigned long microSeconds;

unsigned int ampThreshold = 10;
unsigned int fadeSpeed = 15;
unsigned int minFrequency = 82;
unsigned int maxFrequency = 1100;
double logMinFrequency = log((double)minFrequency);
double logMaxFrequency = log((double)maxFrequency);

byte ledHue = 0;
byte ampFade = 0;
unsigned int maxAmp = 0;
unsigned int checkMaxAmp;

double vReal[SAMPLES]; //create vector of size SAMPLES to hold real values
double vImag[SAMPLES]; //create vector of size SAMPLES to hold imaginary values

void setup()
{
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  samplingPeriod = round(1000000 * (1.0 / SAMPLING_FREQUENCY)); //Period in microseconds
  //Serial.begin(9600);

  // for (int i = 0; i < NUM_LEDS; i++)
  // {
  //   leds[i] = CRGB(0, 255, 0);
  // }
  // FastLED.show();
}

void loop()
{
  maxAmp = 0;
  /*Sample SAMPLES times*/
  for (int i = 0; i < SAMPLES; i++)
  {
    microSeconds = micros(); //Returns the number of microseconds since the Arduino board began running the current script.

    vReal[i] = analogRead(ANALOG_PIN) - 512; //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
    vImag[i] = 0;                      //Makes imaginary term 0 always

    if (abs(vReal[i]) > maxAmp)
    {
      maxAmp = abs(vReal[i]);
    }
    /*remaining wait time between samples if necessary*/
    while (micros() < (microSeconds + samplingPeriod))
    {
      //do nothing
    }

    //Serial.print(vReal[i]);
    //Serial.print("\n");
  }
  checkMaxAmp = maxAmp / 2;

  /*Perform FFT on samples*/
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_RECTANGLE, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

  /*Find peak frequency and print peak*/
  double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);

  if (peak < maxFrequency && checkMaxAmp > ampFade)
  {
    ledHue = min(255, (abs(log(peak) - logMinFrequency) / (logMaxFrequency-logMinFrequency)) * 255);
  }

  // Fade intesity
  if (checkMaxAmp > ampFade && checkMaxAmp > ampThreshold)
  {
    ampFade = checkMaxAmp;
  }
  else
  {
    if (ampFade <= fadeSpeed)
    {
      ampFade = 0;
    }
    else
    {
      ampFade = max(ampFade - fadeSpeed, checkMaxAmp / 2);
    }
  }

  // Assign LED values
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i].setHSV(ledHue, 255, ampFade);
  }

  FastLED.show();

  // Serial.print("\n\n");
  // Serial.print(checkMaxAmp);
  // Serial.print(" ");
  // Serial.println(ledHue); //Print out the most dominant frequency.
}