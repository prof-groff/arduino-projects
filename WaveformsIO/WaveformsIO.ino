#include "WaveformsIO.h"

// Enable Pin, Data Pin, Clock Pin, Switch Pin 1, Switch Pin 2, and Button Pin are 
// 4, 5, 2, 8, 9, and 10, respectively
WaveformsIO myWave(4, 5, 2, 8, 9, 10);

// float myNum = 0;
// byte cur_waveform[64];
// byte wave_shape = 1;

// byte PBREG;

// C4 through C5
const float scale[8] = {261.626, 293.665, 329.628, 349.228, 391.995, 440, 493.883, 523.251};

// define waveforms
// sawtooth wave
const byte sawtooth[64] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63};
// triangle wave
const byte triangle[64] = {0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,33,35,37,39,41,43,45,47,49,51,53,55,57,59,61,63,61,59,57,55,53,51,49,47,45,43,41,39,37,35,33,32,30,28,26,24,22,20,18,16,14,12,10,8,6,4,2};
// sine wave
const byte sine[64] = {32,35,38,41,44,46,49,51,54,56,58,59,61,62,62,63,63,63,62,62,61,59,58,56,54,51,49,46,44,41,38,35,32,28,25,22,19,17,14,12,9,7,5,4,2,1,1,0,0,0,1,1,2,4,5,7,9,12,14,17,19,22,25,28};
// square wave
const byte squarewave[64] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63};

byte waveform[64];

volatile byte iSample = 0;
const byte nSamples = 64; // selected to balance sampling resolution with range of attanable frequencies

byte shape = 2;
float frequency;



// stuff for frequency measuring
// This code measure the frequency of an incoming periodic signal using Timer1 and the
// onboard comparator. For squrewaves the code should work regardless of the duty-cycle.
volatile unsigned int overflows = 0;
volatile unsigned long edges = 0;
volatile unsigned long tstart = 0;
volatile unsigned long tstop = 0;
volatile unsigned long tnow = 0;
const unsigned long cycles = 100; // how many cycles to average over

float frequencyIn; 

int potCur, potLast;

int maxFrequency = 1000;

void setup() {
  myWave.ConfigureDisplay(false, true);
  Serial.begin(9600);
  potCur = analogRead(0);
  potLast = potCur;
  frequency = ((float)potCur/1023)*maxFrequency;

  pinMode(3, OUTPUT);  


  //  myWave.GetMode();
  //  SelectWaveform(myWave.mode);
  //  GenerateWaveform(frequency);
  //  myWave.WriteToDisplay(frequency);
  SelectWaveform(shape);
  GenerateWaveform(frequency);
  myWave.WriteToDisplay(frequency);
  delay(500);
  
}

void loop() {
//  // three modes controlled by switch: function generator, tone generator, and frequency measurer
//  // as function generator: button changes waveform shape while two potentiometers change frequency and volume
//  // as tone generator: button changes note (sine wave) while one potentiometers changes volume
//  // as frequency measurer: button and potentiometers are disabled. 


while(1) {
  if(digitalRead(10)==LOW) {
    // Serial.println("hi");
    shape++;
    if (shape > 4) {
      shape = 1;
    }
    SelectWaveform(shape);
    GenerateWaveform(frequency);
    myWave.WriteToDisplay(frequency);
    delay(500);
  }
  
  potCur = analogRead(0);
  if (abs(potCur - potLast)>5) {
    frequency = ((float)potCur/1023)*maxFrequency;
    GenerateWaveform(frequency); 
    myWave.WriteToDisplay(frequency);
    potLast = potCur;
    delay(100);
  }
}

while(0) {
  frequency = 0;
  for(byte i = 0; i < 100; i++) {
    frequency = frequency + analogRead(0);
  }
  frequency = ((frequency/1023)/100)*1000;
  GenerateWaveform(frequency);
  myWave.WriteToDisplay(frequency);
  delay(100);
}

//if(myWave.GetMode()) {
//  InitializeWaveform(myWave.mode);
//  GenerateWaveform(frequency);
//  myWave.WriteToDisplay(frequency);
//}
   
//  while (myWave.mode == 1) { // mode 1
//    // do function generator
//    myWave.WriteToDisplay(1);
//    
//    
//    if(myWave.GetMode()) {
//      break;
//    }
//  }
//  
//  while (myWave.mode == 2) { // mode 2
//    // do tone generator
//    myWave.WriteToDisplay(2);
//    
//    if(myWave.GetMode()) {
//      break;
//    }
//  }
//  
//  while (myWave.mode == 3) { // mode 3
//    // do frequency measurer
//    myWave.WriteToDisplay(3);
//    
//    if(myWave.GetMode()) {
//      break;
//    }
//  }

}



void SelectWaveform(byte shape) {


  if (shape == 1) {
    for (int i = 0; i < nSamples; i++) {
      waveform[i] = squarewave[i];
    }
    
  }
  else if (shape == 2) {
    for (int i = 0; i < nSamples; i++) {
      waveform[i] = sine[i];
    }
  }
  else if (shape == 3) {
    for (int i = 0; i < nSamples; i++) {
      waveform[i] = triangle[i];
    }
  }
  else if (shape == 4) {
    for (int i = 0; i < nSamples; i++) {
      waveform[i] = sawtooth[i];
    }
  }
  else {
    // error
  }
}

void GenerateWaveform(float frequency) {

    // set up timer/counter 2, fast PWM with OCR2A as TOP, OC2B (pin 5, DP3) set high at BOTTOM and set LOW on output on compare match with OCR2B (non-inverting mode)
    TCCR2A = B00100011;
    TCCR2B = B00001001; // no prescaler (bit 0)
    OCR2A = 63; // OCR2A is one byte, smaller values increase frequency of PWM output on OC2B
    OCR2B = waveform[iSample];
    TIMSK2 = B00000000;
    
    // set up timer/counter 1, normal mode, no output, fast PWM, OCR1A is TOP, no prescaler
    TCCR1A = B00000011; 
    TCCR1B = B00011001;
  
    TIMSK1 = B00000010; // enable interrupt when OCR1A is reached
    
    // timer/counter 1 sets the rate at which the sample value is changed and, thus, the frequency of the output waveform.
    OCR1A = round(16e6/(nSamples*frequency)) - 1;

}

ISR(TIMER1_COMPA_vect) {
  iSample++;
  if (iSample == nSamples) {
    iSample = 0;
  }
  OCR2B = waveform[iSample];
}



void measureFreq() {
  edges = 0;
  // ACSR = ACSR | B01000100; // comparator input capture enabled using internal bandgap voltage on positive input 
 
  ACSR = ACSR | B00000010; // enable analog comparator interrupt (bit 3) on failing edge (bit 1) 
  // using external reference voltage on digital pin 6 (AIN0) (bit 6 = 0)  
  
  overflows = 0;
  
  TCCR1A = B00000000; // set timer/counter 1 in normal mode where it will count to 0xFFFF then repeat
  TCCR1B = B10000000; // input capture enabled with capture on falling edge (bit 6 = 0) which would
  // correspond to a rising edge of the actual signal being detected. bit 7 = 1 turns on the noise canceler
  TIMSK1 = TIMSK1 | B00000001; // enables Timer1 input capture interrupt (bit 5) and/or overflow interrupt (bit 0)
  
  // turn on the counter - no prescaler
  TCCR1B = TCCR1B | B00000001;
  ACSR = ACSR | B00001000; // enable analog comparator interrupt (bit 3)
  
  while (edges < (cycles+1)) {
    // do nothing
  }
  
  // calculate the frequency 
  frequencyIn = (float)16000000*(float)cycles/(float)(tstop - tstart);
  
  
  
}


ISR(TIMER1_OVF_vect) {
  overflows += 1;
  }
 
 
  ISR(ANALOG_COMP_vect)
  {
    tnow = TCNT1;
    edges += 1;
    if (edges == 1) {
     tstart = overflows*65536 + tnow;
    }
   else if (edges == cycles + 1) {
     tstop = overflows*65536 + tnow;
     // turn off the clock and comparator
     ACSR = 0;
     TCCR1B = 0;
   }
  }

