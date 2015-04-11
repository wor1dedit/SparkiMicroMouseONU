#include <Sparki.h>
#include <DistanceGP2Y0A41SK.h>

DistanceGP2Y0A41SK irRight;
DistanceGP2Y0A41SK irLeft;

const int RIGHT_IR_PIN = A8;
const int LEFT_IR_PIN = A1;

int rightDistance = 0;
int leftDistance = 0;
int centerDistance = 0;

void setup()
{
  irRight.begin(RIGHT_IR_PIN);
  irLeft.begin(LEFT_IR_PIN);
}

void loop()
{
    leftHandSolver();
}

//Solves the Maze with the left hand rule
void leftHandSolver()
{
  while(true)
  {
    if(centerDistance < 18 && centerDistance > 8)
    {
      sparki.moveBackward(centerDistance - 2);
    }
    else if(centerDistance > 18)
    {
      sparki.moveBackward(18)
    }
    centerDistance = sparki.ping();
    rightDistance = irRight.getDistanceCentimeter();
    leftDistance = irLeft.getDistanceCentimeter();
    delay(100); // wait 0.1 seconds (100 milliseconds)
    if(centerDistance < 8 || leftDistance > 8)
    {
      if(leftDistance > 8)
      {
        sparki.moveRight(90);
      }
      else if(rightDistance > 8)
      {
        sparki.moveLeft(90);
      }
      else if(rightDistance > leftDistance)
      {
        sparki.moveLeft(180);
      }
      else
      {
        sparki.moveRight(180);
      }
    }
  }
}

void rightHandSolver()
{
  while(true)
  {
    if(centerDistance < 18 && centerDistance > 8)
    {
      sparki.moveBackward(centerDistance - 2);
    }
    else if(centerDistance > 18)
    {
      sparki.moveBackward(18)
    }
    centerDistance = sparki.ping();
    rightDistance = irRight.getDistanceCentimeter();
    leftDistance = irLeft.getDistanceCentimeter();
    delay(100); // wait 0.1 seconds (100 milliseconds)
    if(centerDistance < 8 || rightDistance > 8)
    {
      if(rightDistance > 8)
      {
        sparki.moveLeft(90);
      }
      else if(leftDistance > 8)
      {
        sparki.moveRight(90);
      }
      else if(rightDistance > leftDistance)
      {
        sparki.moveLeft(180);
      }
      else
      {
        sparki.moveRight(180);
      }
    }
  }
}

void randomSolver()
{
  while(true)
  {
    centerDistance = sparki.ping();
    rightDistance = irRight.getDistanceCentimeter();
    leftDistance = irLeft.getDistanceCentimeter();
    delay(100); // wait 0.1 seconds (100 milliseconds)
    
    bool skip = false;
    int dist = 0;
    int turnDist = 12; 
    int lastWallLeft = 0;
    int lastWallRight = 0;
    
    while(centerDistance > 11)
    {
      sparki.moveBackward(6);
      dist = dist + 6;
      
      rightDistance = irRight.getDistanceCentimeter();
      leftDistance = irLeft.getDistanceCentimeter();
      
      int rand = random(0,6);
      
      if(leftDistance >= 10)
      {
        lastWallLeft += 6;
        if(rand == 5)
        {
          sparki.moveBackward(turnDist - lastWallLeft);
          sparki.moveRight(90); 
          skip = true;
          break;
        }
      }

      if(rightDistance >= 10)
      {
        lastWallRight += 6;
        if(rand == 4)
        {
          sparki.moveBackward(turnDist - lastWallRight);
          sparki.moveLeft(90);
          skip = true;
          break;
        }
      }

      if(leftDistance < 10)
      {
        lastWallLeft = 0;
      }

      if(rightDistance < 10)
      {
        lastWallRight = 0;
      }
      
      lastWallRight = (lastWallRight >= 18) ? 0 : lastWallRight;
      lastWallLeft = (lastWallLeft >= 18) ? 0 : lastWallLeft;
      
      delay(125);
      
      centerDistance = sparki.ping();
    }
  
    sparki.moveStop();

    if(!skip)
    {
      sparki.moveBackward(centerDistance - 3);
      
      rightDistance = irRight.getDistanceCentimeter();
      leftDistance = irLeft.getDistanceCentimeter();

      while(true)
      {
        if(leftDistance == -1)
        {
          leftDistance = irLeft.getDistanceCentimeter();
        }
        else if(rightDistance == -1)
        {
          rightDistance = irRight.getDistanceCentimeter();
        }
        else if(leftDistance == rightDistance) 
        {
          rightDistance = irRight.getDistanceCentimeter();
          leftDistance = irLeft.getDistanceCentimeter();
        }
        if(leftDistance < 5 && rightDistance < 5)
        {
          rightDistance = irRight.getDistanceCentimeter();
          leftDistance = irLeft.getDistanceCentimeter();
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
  }
}