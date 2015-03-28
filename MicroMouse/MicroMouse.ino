//Libraries
#include <Sparki.h> // include the sparki library
#include <DistanceGP2Y0A41SK.h>

//Distance Sensors
DistanceGP2Y0A41SK DistRight;
DistanceGP2Y0A41SK DistLeft;

int ping_single(int trig, int echo);
int ping(int trig, int echo);

const int RIGHT_IR_PIN = A8;
const int LEFT_IR_PIN = A1;

int rightDistance = 0;
int leftDistance = 0;
int centerDistance = 0;

void setup()
{
  sparki.clearLCD();
  DistRight.begin(RIGHT_IR_PIN);
  DistLeft.begin(LEFT_IR_PIN);
  Serial1.begin(9600);
}

void loop()
{
    centerDistance = sparki.ping();
    rightDistance = DistRight.getDistanceCentimeter();
    leftDistance = DistLeft.getDistanceCentimeter();
    delay(100); // wait 0.1 seconds (100 milliseconds)
    
    while(centerDistance > 4)
    {
      sparki.moveBackward();
      centerDistance = sparki.ping();
      delay(50);
      Serial1.print("Center: ");
      Serial1.println(centerDistance);
    }
  
    sparki.moveStop();
    sparki.moveBackward(2);
    
    rightDistance = DistRight.getDistanceCentimeter();
    leftDistance = DistLeft.getDistanceCentimeter();

    
    Serial1.print("Right: ");
    Serial1.println(rightDistance);
    Serial1.print("Left: ");
    Serial1.println(leftDistance);
        
    while(true)
    {
      if(leftDistance == -1)
      {
        leftDistance = DistLeft.getDistanceCentimeter();
      }
      else if(rightDistance == -1)
      {
        rightDistance = DistRight.getDistanceCentimeter();
      }
      else if(leftDistance == rightDistance) 
      {
        rightDistance = DistRight.getDistanceCentimeter();
        leftDistance = DistLeft.getDistanceCentimeter();
      }
      else if(leftDistance > rightDistance)
      {
        sparki.moveLeft(90);
        break;
      }
      else
      {
        sparki.moveRight(90);
        break;
      }
    }
    sparki.moveStop();
}
