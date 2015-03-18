#include <Sparki.h> // include the sparki library

int ping_single(int trig, int echo);
int ping(int trig, int echo);

const int RIGHT_TRIG = 16;
const int RIGHT_ECHO = 19;
const int CENTER_TRIG = 10;
const int CENTER_ECHO = 5;
const int LEFT_TRIG  = 15;
const int LEFT_ECHO  = 14;

void setup()
{
  pinMode(RIGHT_TRIG, OUTPUT);
  pinMode(RIGHT_ECHO, INPUT);
  pinMode(CENTER_TRIG,OUTPUT);
  pinMode(CENTER_ECHO, INPUT);
  pinMode(LEFT_TRIG,OUTPUT);
  pinMode(LEFT_ECHO, INPUT);
  Serial1.begin(9600);
}

void loop()
{
    int centerSonic = ping(CENTER_TRIG, CENTER_ECHO);
    int rightSonic = ping(RIGHT_TRIG, RIGHT_ECHO);
    int leftSonic = ping(LEFT_TRIG, LEFT_ECHO);
    sparki.clearLCD();
    sparki.print("Mid: "); 
    sparki.print(centerSonic); // tells the distance to the computer
    sparki.println(" cm");
    sparki.updateLCD();
    delay(100); // wait 0.1 seconds (100 milliseconds)
    
    while(centerSonic > 4)
    {
      sparki.moveBackward();
      
      centerSonic = ping(CENTER_TRIG, CENTER_ECHO);
      delay(50);
      sparki.clearLCD();     
      sparki.print("Mid: "); 
      sparki.print(centerSonic); // tells the distance to the computer
      sparki.println(" cm");
      sparki.updateLCD();
      Serial1.print("Mid: ");
      Serial1.println(centerSonic);
    }
  
    sparki.moveStop();
    sparki.moveBackward(2);
    
    rightSonic = ping(RIGHT_TRIG, RIGHT_ECHO);
    delay(50);
    leftSonic = ping(LEFT_TRIG, LEFT_ECHO);
    delay(50);
    Serial1.print("Left: ");
    Serial1.println(leftSonic);
    Serial1.print("Right: ");
    Serial1.println(rightSonic);
        
    while(true)
    {
      if(leftSonic == -1)
      {
        leftSonic = ping(LEFT_TRIG, LEFT_ECHO);
        delay(30);
      }
      else if(rightSonic == -1)
      {
        rightSonic = ping(RIGHT_TRIG, RIGHT_ECHO);
        delay(50);
      }
      else if(leftSonic == rightSonic) 
      {
        rightSonic = ping(RIGHT_TRIG, RIGHT_ECHO);
        delay(50);
        leftSonic = ping(LEFT_TRIG, LEFT_ECHO);
        delay(50);
      }
      else if(leftSonic > rightSonic)
      {
        sparki.moveLeft(90);
        sparki.clearLCD();     
        sparki.print("Left"); 
        sparki.updateLCD();
        break;
      }
      else
      {
        sparki.moveRight(90);
        sparki.clearLCD(); 
        sparki.print("Right"); 
        sparki.updateLCD();
        break;
      }
    }
    sparki.moveStop();
}

int ping(int trig, int echo){
  int attempts = 7;
  float distances [attempts];
  for(int i=0; i<attempts; i++){
    distances[i] = ping_single(trig, echo);
    delay(20);
  }
  
  // sort them in order
  int i, j;
  float temp;
 
  for (i = (attempts - 1); i > 0; i--)
  {
    for (j = 1; j <= i; j++)
    {
      if (distances[j-1] > distances[j])
      {
        temp = distances[j-1];
        distances[j-1] = distances[j];
        distances[j] = temp;
      }
    }
  }
  
  // return the middle entry
  return int(distances[(int)ceil((float)attempts/2.0)]); 
}

int ping_single(int trig, int echo){
  long duration; 
  float cm;
  digitalWrite(trig, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trig, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trig, LOW); 
  

  uint8_t bit = digitalPinToBitMask(echo);
  uint8_t port = digitalPinToPort(echo);
  uint8_t stateMask = (HIGH ? bit : 0);
  
  unsigned long startCount = 0;
  unsigned long endCount = 0;
  unsigned long width = 0; // keep initialization out of time critical area
  
  // convert the timeout from microseconds to a number of times through
  // the initial loop; it takes 16 clock cycles per iteration.
  unsigned long numloops = 0;
  unsigned long maxloops = 5000;
	
  // wait for any previous pulse to end
  while ((*portInputRegister(port) & bit) == stateMask)
    if (numloops++ == maxloops)
      return -1;
	
  // wait for the pulse to start
  while ((*portInputRegister(port) & bit) != stateMask)
    if (numloops++ == maxloops)
      return -1;
  
  startCount = micros();
  // wait for the pulse to stop
  while ((*portInputRegister(port) & bit) == stateMask) {
    if (numloops++ == maxloops)
      return -1;
    delayMicroseconds(10); //loop 'jams' without this
    if((micros() - startCount) > 58000 ){ // 58000 = 1000CM
      return -1;
      break;
    }
  }
  duration = micros() - startCount;
  //--------- end pulsein
  cm = (float)duration / 29.0 / 2.0; 
  return int(cm);
}
