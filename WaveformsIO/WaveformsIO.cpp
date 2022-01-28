#include "WaveformsIO.h"
#include <Arduino.h>

WaveformsIO::WaveformsIO(byte EP, byte DP, byte CP, byte S1P, byte S2P, byte BP) { 
  m_EP = EP;
  m_DP = DP;
  m_CP = CP; 
  
//  byte S1P = 8;
//  byte S2P = 9;
//  byte BP = 10;
//  // S1P, S2P, and BP are pins 8, 9, and 10, which are bits 0, 1, and 2 respectively in PORTB
//  // set these pins as inputs
//  
  m_S1P = S1P;
  m_S2P = S2P;
  m_BP = BP;
  
  pinMode(m_S1P, INPUT);
  pinMode(m_S2P, INPUT);
  pinMode(m_BP, INPUT);
  
  pinMode(m_EP, OUTPUT);
  digitalWrite(m_EP, HIGH);
  pinMode(m_DP, OUTPUT);
  digitalWrite(m_CP, LOW);
  pinMode(m_CP, OUTPUT);
  digitalWrite(m_CP, LOW);
  
  updateCR = true;
  
  mode = 0;
  GetMode();
  
  CR = B11000000;
  DR = 0x00000000; // only needs to be 24-bits so two most significant hex digits are unused

}
	
void WaveformsIO::ConfigureDisplay(boolean blank, boolean dim) {
  m_blank = blank;
  m_dim = dim;
  
  CR_last = CR;
  
  if (m_blank) {
    CR = CR & B11111110;
  }
  else {
    CR = CR | B00000001;
  }
  
  if (CR != CR_last) { 
    updateCR = true;
  }
  
  
  if (m_dim) {
    DR = DR & 0b11111111011111111111111111111111;
  }
  else {
    DR = DR | 0b00000000100000000000000000000000;
  }
  
}
  
void WaveformsIO::WriteToDisplay(float num) {
  if (num > 9999) {
    NumberOverflow();
  }
  else {
    CR_last = CR;
    CR = CR | B00100000; // needed to disable bank 5 (semi-colon and high dot)
    if (CR != CR_last) {
      updateCR = true;
    }
    	
    DR = DR & 0b11111111100000000000000000000000; // reset bits that set digits in banks 1-4 (with bank 5 blank, set to show 0) and decimal place location
    
    // determine location of decimal place
    if (num >= 1000) {
      digits = round(num);
      DR += 0x00400000; // 0b00000000010000000000000000000000, ones (bank 4)
    }
    else if (num >= 100) {
      digits = round(num*10);
      DR += 0x00300000; // 0b00000000001100000000000000000000, tenths (bank 3)
    }
    else if (num >= 10) {
      digits = round(num*100);
      DR += 0x00200000; // 0b00000000001000000000000000000000, hundredths (bank 2)
    }
    else {
      digits = round(num*1000);
      DR += 0x00100000; // 0b00000000000100000000000000000000, thousandths (bank 1)
    }
    
    // find digits to display
    d1 = digits/1000;
    d2 = (digits/100) % 10;
    d3 = (digits/10) % 10;
    d4 = digits % 10;
    
    DR = DR + d1 + 0x00000010*d2 + 0x00000100*d3 + 0x00001000*d4;
    // digit place numbering is left to right; e.g, d1 is the thousands place but is the least significant element of the hex code; might change on hardware revision
  }
  
  if (updateCR) {
    digitalWrite(m_EP, LOW);
    for (byte ii=0; ii<8; ii++) {
      if (CR & (B10000000 >> ii)) {
        digitalWrite(m_DP, HIGH);
      }
      else {
        digitalWrite(m_DP, LOW);
      }
      digitalWrite(m_CP, HIGH);
      digitalWrite(m_CP, LOW);
    }
    digitalWrite(m_EP, HIGH);
    updateCR = false;
  }
  digitalWrite(m_EP, LOW);
  for (byte ii=0; ii<24; ii++) {
    if (DR & (0x00800000 >> ii)) {
      digitalWrite(m_DP, HIGH);
    }
    else {
      digitalWrite(m_DP, LOW);
    }
    digitalWrite(m_CP, HIGH);
    digitalWrite(m_CP, LOW);
  }
  digitalWrite(m_EP, HIGH);

}

void WaveformsIO::NumberOverflow() {
  CR_last = CR;
  CR = CR | B00110010;
  if (CR != CR_last) { 
    updateCR = true;
  }
  DR = DR & 0b11111111100000000000000000000000;
  DR += 0x00005F00; // 0b00000000000000000101111100000000;
}	

boolean WaveformsIO::GetMode() {
  byte currentMode = mode;
  boolean modeChange = false;
  
  if (digitalRead(m_S1P)==LOW) {
    mode = 1;
  }
  else if (digitalRead(m_S2P)==LOW) {
    mode = 2;
  }
  else {
    mode = 3;
  }
 
  
  if (currentMode != mode) {
    modeChange = true;
  }
  return modeChange;
}

    
