// Zach Bayler
// Embedded Systems Programming
// 04/13/2023
// Developed with help from Dr. Martins' provided resources

/*
Kitchen Timer
- start/stop button
- increment seconds button (add to time left)
- display seconds left
- both LEDs on = stopped
- green LED on = running
- red only = alarm (reached 0)
*/

// Pin assignments
#define BUTTON_1  2
#define BUTTON_2  3
#define GREEN_LED 4
#define RED_LED   5
#define BUZZER 6

#define DATA      9   //74HC595  pin 8 DS
#define LATCH     8   //74HC595  pin 9 STCP
#define CLOCK     7  //74HC595  pin 10 SHCP

#define DIGIT_4   10
#define DIGIT_3   11
#define DIGIT_2   12
#define DIGIT_1   13

// time between displaying digits on the 7 segment display in ms 
// (have to rotate quickly between them)
#define digitDelay 3

// segment assignments to display correct digit on 7-segment (0-9,a-f)
unsigned char gTable[]=
{0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c
,0x39,0x5e,0x79,0x71,0x00};

//volatile variables
volatile unsigned int gTimeLeftSeconds = 30;
volatile int gButtonFlag2 = 0;

//governing state variable
// 0 is paused, 1 is counting down, 2 = alarm (hit 0s)
uint8_t gState = 0; 


unsigned int gReloadTimer1 = 62500; // corresponds to 1 second


void setup() {
  // LEDs Pins
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // Button Pins
  pinMode(BUTTON_1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_1), Button_1_ISR, RISING);
  pinMode(BUTTON_2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_2), Button_2_ISR, RISING);

  // Buzzer Pin
  pinMode(BUZZER, OUTPUT);

  // 7-Seg Display
  pinMode(DIGIT_1, OUTPUT);
  pinMode(DIGIT_2, OUTPUT);
  pinMode(DIGIT_3, OUTPUT);
  pinMode(DIGIT_4, OUTPUT);

  // Shift Register Pins
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);

  //setup timer 1
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = gReloadTimer1; // compare match register 16MHz/256
  TCCR1B |= (1<<WGM12);   // CTC mode 
  TIMSK1 |= (1<<OCIE1A);  // enable timer compare interrupt
  interrupts();

}


// increment time left 
void Button_1_ISR()
{
  for(int i = 0; i<30000; i++);    //software debounce; needs bigger but couldn't use bigger because that messes with the behavior
  gTimeLeftSeconds++;
}

// start/stop button
void Button_2_ISR()
{ 
  for(int i = 0; i<30000; i++);     //software debounce
  gButtonFlag2++;
  if((gButtonFlag2 % 2) == 0){      //alternates between states on press
    gState = 0;
  }
  else{
    gState = 1;
  }
  
}


// Write multiple digits to the 4x7-segment display at the "same time"
// digit1 is the left most place (MSB)
void write_display(int digit1, int digit2, int digit3, int digit4){

  //turn all digits off
  digitalWrite(DIGIT_1, HIGH);
  digitalWrite(DIGIT_2, HIGH);
  digitalWrite(DIGIT_3, HIGH);
  digitalWrite(DIGIT_4, HIGH);

  //write first digit with given parameter
  digitalWrite(LATCH, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, gTable[digit1]);
  digitalWrite(LATCH, HIGH);
  digitalWrite(DIGIT_1, LOW);
  delay(digitDelay);              //hold on for a brief period
  digitalWrite(DIGIT_1, HIGH);    //turn off

  digitalWrite(LATCH, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, gTable[digit2] | (1<<7));   //include decimal point after 2nd MSB
  digitalWrite(LATCH, HIGH);
  digitalWrite(DIGIT_2, LOW);
  delay(digitDelay);
  digitalWrite(DIGIT_2, HIGH);

  digitalWrite(LATCH, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, gTable[digit3]);
  digitalWrite(LATCH, HIGH);
  digitalWrite(DIGIT_3, LOW);
  delay(digitDelay);
  digitalWrite(DIGIT_3, HIGH);


  digitalWrite(LATCH, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, gTable[digit4]);
  digitalWrite(LATCH, HIGH);
  digitalWrite(DIGIT_4, LOW);
  delay(digitDelay);
  digitalWrite(DIGIT_4, HIGH);
}

//converts time in seconds to minutes and seconds then calls write function
void display(int timeSeconds){

  int digit_4 = (int) timeSeconds % 10;            //seconds; ones place xx:x0
  int digit_3 = (int) (timeSeconds / 10) % 6;      //seconds; tens place xx:0x
  int digit_2 = (int) (timeSeconds / 60) % 10;     //minutes; ones place x0:xx
  int digit_1 = (int) (timeSeconds / 600) % 6;     //minutes; tens place 0x:xx

  write_display(digit_1, digit_2, digit_3, digit_4);
}

// Timer1 interrupt service routine (ISR)
ISR(TIMER1_COMPA_vect)  
{
  //decrement count once per second
  gTimeLeftSeconds--;

  // timer end condition
  if(gTimeLeftSeconds == 0)
  {
    gState = 2;
  }
}

void startTimer1()
{
  // Start Timer
  TCCR1B |= (1<<CS12);    // 256 prescaler 
  TIMSK1 |= (1<<OCIE1A);  // enable timer compare interrupt
}

void stopTimer1()
{
  // Stop Timer
  TCCR1B &= 0b11111000; // stop clock
  TIMSK1 = 0; // cancel clock timer interrupt
}

//turn on buzzer in a pattern
void activeBuzzer()
{
  unsigned char i;
  unsigned char sleepTime = 1; // ms
  
  for(i=0;i<100;i++)
  {
    digitalWrite(BUZZER,HIGH);
    delay(sleepTime);//wait for 1ms
    digitalWrite(BUZZER,LOW);
    delay(sleepTime);//wait for 1ms
  }
}

void loop() {

  //control LEDs and counting based on desired state
  switch(gState){
    
    //paused; both LEDs on
    case 0:
      stopTimer1();
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, HIGH);
      break;

    //running; red off, green on
    case 1:
      startTimer1();
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
      break;

    //end alarm; red on, green off
    case 2:
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
      activeBuzzer();
      break;
  }

  //show count down
  display(gTimeLeftSeconds);

}
