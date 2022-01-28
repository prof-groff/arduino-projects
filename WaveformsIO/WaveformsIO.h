#ifndef WAVEFORMSIO_H
#define WAVEFORMSIO_H

#include <Arduino.h>

class WaveformsIO
{
private:
  byte m_EP, m_DP, m_CP, m_S1P, m_S2P, m_BP, CR, CR_last;
  boolean m_blank, m_dim, updateCR;
  unsigned int digits, d1, d2, d3, d4; // works but may be less than ideal
  unsigned long DR;
  
  void NumberOverflow();

public:
  WaveformsIO(byte EP, byte DP, byte CP, byte S1P, byte S2P, byte BP); // constructor
  
  byte mode;
  
  void WriteToDisplay(float num);
  void ConfigureDisplay(boolean blank, boolean dim);
  boolean GetMode(); 
  
      
};

#endif

