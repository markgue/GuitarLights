// #include <Arduino.h>
// #include <FastLED.h>
// #include <math.h>
// #define NUM_LEDS 400
// #define DATA_PIN 7


// CRGB leds[NUM_LEDS];
// int ledHue = 0;
// int ledIndex = 0;
// byte ampFade = 0;
// byte fadeSpeed = 10;

// //clipping indicator variables
// boolean clipping = 0;

// //data storage variables
// byte newData = 0;
// byte prevData = 0;
// unsigned int time = 0;//keeps time and sends vales to store in timer[] occasionally
// int timer[10];//sstorage for timing of events
// int slope[10];//storage for slope of events
// unsigned int totalTimer;//used to calculate period
// unsigned int period;//storage for period of wave
// byte index = 0;//current storage index
// float frequency;//storage for frequency calculations
// int maxSlope = 0;//used to calculate max slope as trigger point
// int newSlope;//storage for incoming slope data

// //variables for decided whether you have a match
// byte noMatch = 0;//counts how many non-matches you've received to reset variables if it's been too long
// byte slopeTol = 20;//slope tolerance- adjust this if you need
// int timerTol = 15;//timer tolerance- adjust this if you need
// float maxFrequency = (1300);//cutoff for maximum frequency detection (a guitar can't play higher than this)

// //variables for amp detection
// unsigned int ampTimer = 0;
// byte maxAmp = 0;
// byte checkMaxAmp;
// byte ampThreshold = 10;//raise if you have a very noisy signal

// void setup() {
//   //Serial.begin(9600);

//   FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);

//   // for (int i = 0; i < NUM_LEDS ; i++) {
//   //   leds[i] = CRGB(0, 255, 0);
//   // }
//   // FastLED.show();

//   pinMode(13, OUTPUT); //led indicator pin
//   pinMode(12, OUTPUT); //output pin

//   cli();//diable interrupts

//   //set up continuous sampling of analog pin 0 at 38.5kHz

//   //clear ADCSRA and ADCSRB registers
//   ADCSRA = 0;
//   ADCSRB = 0;

//   ADMUX |= (5 & 0x07);    // set A5 analog input pin
//   ADMUX |= (1 << REFS0); //set reference voltage
//   ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only

//   ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
//   ADCSRA |= (1 << ADATE); //enabble auto trigger
//   ADCSRA |= (1 << ADIE); //enable interrupts when measurement complete
//   ADCSRA |= (1 << ADEN); //enable ADC
//   ADCSRA |= (1 << ADSC); //start ADC measurements

//   sei();//enable interrupts
// }

// void reset() { //clea out some variables
//   index = 0;//reset index
//   noMatch = 0;//reset match couner
//   maxSlope = 0;//reset slope
// }

// ISR(ADC_vect) {//when new ADC value ready

//   PORTB &= B11101111;//set pin 12 low
//   prevData = newData;//store previous value
//   newData = ADCH;//get value from A0
//   //Serial.println(newData);
//   if (prevData < 127 && newData >= 127) { //if increasing and crossing midpoint
//     newSlope = newData - prevData;//calculate slope
//     if (abs(newSlope - maxSlope) < slopeTol) { //if slopes are ==
//       //record new data and reset time
//       slope[index] = newSlope;
//       timer[index] = time;
//       time = 0;
//       if (index == 0) { //new max slope just reset
//         PORTB |= B00010000;//set pin 12 high
//         noMatch = 0;
//         index++;//increment index
//       }
//       else if (abs(timer[0] - timer[index]) < timerTol && abs(slope[0] - newSlope) < slopeTol) { //if timer duration and slopes match
//         //sum timer values
//         totalTimer = 0;
//         for (byte i = 0; i < index; i++) {
//           totalTimer += timer[i];
//         }
//         period = totalTimer;//set period
//         //reset new zero index values to compare with
//         timer[0] = timer[index];
//         slope[0] = slope[index];
//         index = 1;//set index to 1
//         PORTB |= B00010000;//set pin 12 high
//         noMatch = 0;
//       }
//       else { //crossing midpoint but not match
//         index++;//increment index
//         if (index > 9) {
//           reset();
//         }
//       }
//     }
//     else if (newSlope > maxSlope) { //if new slope is much larger than max slope
//       maxSlope = newSlope;
//       time = 0;//reset clock
//       noMatch = 0;
//       index = 0;//reset index
//     }
//     else { //slope not steep enough
//       noMatch++;//increment no match counter
//       if (noMatch > 9) {
//         reset();
//       }
//     }
//   }

//   if (newData == 0 || newData == 1023) { //if clipping
//     PORTB |= B00100000;//set pin 13 high- turn on clipping indicator led
//     clipping = 1;//currently clipping
//   }

//   time++;//increment timer at rate of 38.5kHz

//   ampTimer++;//increment amplitude timer
//   if (abs(127 - ADCH) > maxAmp) {
//     maxAmp = abs(127 - ADCH);
//   }
//   if (ampTimer == 1000) {
//     ampTimer = 0;
//     checkMaxAmp = maxAmp;
//     maxAmp = 0;
//   }

// }

// void checkClipping() { //manage clipping indicator LED
//   if (clipping) { //if currently clipping
//     PORTB &= B11011111;//turn off clipping indicator led
//     clipping = 0;
//   }
// }

// void loop() {

//   checkClipping();

//     // for (int i = 0; i < NUM_LEDS; i++) {    
//     //     leds[i].maximizeBrightness();
//     // }

//   if (checkMaxAmp > ampThreshold) {


//     frequency = 38462 / float(period); //calculate frequency timer rate/period
//     if (frequency < maxFrequency && checkMaxAmp > ampFade) {
//       //print results

//       ledHue = (frequency / maxFrequency) * 255;
//       // Serial.print(ledIndex);
//       // Serial.print(" ");
//       // Serial.print(frequency);
//       // Serial.println(" hz");
//     }
//   }
//   // Fade intesity 
//   if (checkMaxAmp > ampFade && checkMaxAmp > ampThreshold){
//     ampFade = checkMaxAmp;
//   } else {
//     if (ampFade <= fadeSpeed){
//       ampFade = 0;
//     } else {
//       ampFade -= fadeSpeed;
//     }
//   }

//   // Assign LED values
//   for (int i = 0; i < NUM_LEDS; i++) {    
//     leds[i].setHSV(ledHue, 255, ampFade);
//   } 

//   FastLED.show();
//   //delay(10);
// }