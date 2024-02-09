// Libraries
#include "arduinoFFT.h"

// Constants
#define SAMPLES 256            // Sampling constant should be a power of 2
#define SAMPLING_FREQUENCY 8000  // Hz , should be double desired frequency
#define SAMPLES_SUMATION 10
#define MIC_PIN 34
#define ena 13   //5
#define in1 12   //6
#define in2 14   //7
#define in3 27   //8
#define in4 26   //9
#define enb 25   //10

// Motor A is Left
// Motor B is Right

// Classes
arduinoFFT FFT = arduinoFFT();

// Global Variables
unsigned int sampling_period_us;
unsigned int forwardSamples;
unsigned int rightSamples;
unsigned int leftSamples;
unsigned long microseconds;

double vReal[SAMPLES];
double vImag[SAMPLES];

// Setup
void setup() {
    Serial.begin(115200);
    // FFT
    sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY));

    // Pin Modes
    pinMode(ena, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(enb, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);

    // Variables
    forwardSamples = 0;
    leftSamples = 0;
}

// Loop
void loop() {

    // Sampling
    for(int i=0; i<SAMPLES; i++)
    {
        microseconds = micros();    //Overflows after around 70 minutes!

        vReal[i] = analogRead(MIC_PIN);
        vImag[i] = 0;

        while(micros() < (microseconds + sampling_period_us)){
        }
    }

    // FFT
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
    double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);

    // Print the FFT Results
    Serial.println(peak);     // Prints out the most dominant frequency

    // Move Forward Sample
    if ((peak >= 950) and (peak <= 1100)) {
      forwardSamples++;
    // Move Right Sample
    } else if ((peak >= 1275) and (peak <= 1375))
    {
      rightSamples++;
    // Move Left Sample
    } else if ((peak >= 1475) and (peak <= 1575))
    {
      leftSamples++;
    }

    // Move Forward (H-Bridge)
    if (forwardSamples == SAMPLES_SUMATION){
      // Motor A Clockwise Max Speed
      digitalWrite(in1,HIGH);
      digitalWrite(in2,LOW);
      analogWrite(ena, 255);
    
      // Motor B Clockwise Max Speed
      digitalWrite(in3,HIGH);
      digitalWrite(in4,LOW);
      analogWrite(enb, 255);

      forwardSamples = 0;
    }

    // Move Right (H-Bridge)
    if (rightSamples == SAMPLES_SUMATION){
      // Motor A Reverse Low Speed
      digitalWrite(in1,LOW);
      digitalWrite(in2,HIGH);
      analogWrite(ena, 100);
  
      // Motor B Clockwise Max Speed
      digitalWrite(in3,HIGH);
      digitalWrite(in4,LOW);
      analogWrite(enb, 255);

      rightSamples = 0;
    }

    // Move Left (H-Bridge)
   if (leftSamples == SAMPLES_SUMATION){
      // Motor A Forward Max Speed
      digitalWrite(in1,HIGH);
      digitalWrite(in2,LOW);
      analogWrite(ena, 255);
  
      // Motor B Reverse Low Speed
      digitalWrite(in3,LOW);
      digitalWrite(in4,HIGH);
      analogWrite(enb, 100);

      leftSamples = 0;
   }
    

}
