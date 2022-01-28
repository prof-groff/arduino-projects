// Tone Maker by Jeffrey Groff is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.

// int myNotes[] = {30578, 27242, 24270, 22908, 20408, 18182, 16198, 15289}; // C4 (middle C) through C5 - C4, D4, E4, F4, G4, A4, B4 C5
int myNotes[] = {15288, 13620, 12134, 11453, 10203, 9090, 8098, 7644}; // C5 through C6
// int myNotes[] = {61156, 54484, 48540, 45815, 40815, 36363, 32396, 30577}; // C3 through C4
// int myNotes[] = {7644, 6809, 6066, 5726, 5101, 4544, 4049, 3821}; // C6 through C7

boolean buttonPressed = false;
int myPins[] = {2, 3, 4, 5, 6, 7, 8};

// C, D, E, F, G, A, B, C
byte myLetters[] = {B01001110, B00111101, B01001111, B01000111, B01111011, B01110111, B00011111, B00001101};
int maxLetter = 7;
int curLetter = 0;

void setup() {
  
  for (int ii = 2; ii<=9; ii++) {
    pinMode(ii,OUTPUT);
  }
  pinMode(10,INPUT);
  
  setDisplay(B01111111);
}

void loop() {
  // put your main code here, to run repeatedly: 
  
  while(buttonPressed == true) {
    setDisplay(myLetters[curLetter]);
    TCCR1A = B01000011;
    TCCR1B = B00011001;
    OCR1A = myNotes[curLetter];
    curLetter = curLetter + 1;
    if (curLetter > maxLetter) {
     curLetter = 0;
    }
    delay(500); // wait for button release
    if(digitalRead(10)==HIGH) { // button released
      buttonPressed = false;
      break;
    }
  }
  
  if(digitalRead(10)==LOW) {
    buttonPressed = true;
    delay(50); // debounce
  }
}

 void setDisplay(int character) {
   for (int ii=1; ii<=7; ii++) {
     if (character & (B10000000>>ii)) { // this is a bit mask using a bitwise 
     // and operator.
       digitalWrite(myPins[ii-1],HIGH);
     }
     else {
       digitalWrite(myPins[ii-1],LOW);
     }
   }
 }
