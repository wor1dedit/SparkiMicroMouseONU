//Libraries
#include <Sparki.h> // include the sparki library
#include <DistanceGP2Y0A41SK.h>

//Distance Sensors
DistanceGP2Y0A41SK DistRight;
DistanceGP2Y0A41SK DistLeft;

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
  Serial.begin(9600);
}

void loop()
{
    centerDistance = sparki.ping();
    rightDistance = DistRight.getDistanceCentimeter();
    leftDistance = DistLeft.getDistanceCentimeter();
    delay(100); // wait 0.1 seconds (100 milliseconds)
    /*
    while(true)
    {
          centerDistance = sparki.ping();
          rightDistance = DistRight.getDistanceCentimeter();
          leftDistance = DistLeft.getDistanceCentimeter();
          Serial.print("Left: ");
          Serial.println(leftDistance);
          Serial.print("Center: ");
          Serial.println(centerDistance);
          Serial.print("Right: ");
          Serial.println(rightDistance);
          Serial.println("- - - - - - - - - - -");
          delay(500);
    }*/

    sparki.moveBackward();
    {
      rightDistance = DistRight.getDistanceCentimeter();
      leftDistance = DistLeft.getDistanceCentimeter();
      
      if(leftDistance < 4)
      {
        sparki.moveRight(1);
      }
      if(rightDistance < 4)
      {
        sparki.moveLeft(1);
      }
      sparki.moveBackward();
      delay(125);
      centerDistance = sparki.ping();
    }

    
    /*while(centerDistance > 4)
    {
      sparki.moveBackward();
      centerDistance = sparki.ping();
      delay(50);
    }*/
  
    sparki.moveStop();
    
    rightDistance = DistRight.getDistanceCentimeter();
    leftDistance = DistLeft.getDistanceCentimeter();

        
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
      if(leftDistance < 5 && rightDistance < 5)
      {
        rightDistance = DistRight.getDistanceCentimeter();
        leftDistance = DistLeft.getDistanceCentimeter();
        if(leftDistance > 5 || rightDistance > 5)
        {
          continue;
        }
        else
        {
          sparki.moveLeft(180);
          break;
        }
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
