//Libraries
//#include <Sparki.h> // include the sparki library
#include "Sparki.h"
#include "DistanceGP2Y0A41SK.h"
#include "Cell.h"
#include "CellStack.h"

//Distance Sensors
DistanceGP2Y0A41SK irRight;
DistanceGP2Y0A41SK irLeft;
byte leftIR;
byte rightIR;
int forwardDistance;

const int RIGHT_IR_PIN = A8;
const int LEFT_IR_PIN = A1;

int rightDistance = 0;
int leftDistance = 0;
int centerDistance = 0;

#define WALL_THRESHOLD_FORWARD 5 //cm from the the front sensor to up to
#define WALL_THRESHOLD_LEFT 7
#define WALL_THRESHOLD_RIGHT 7
#define TURN_NUMBER_LEFT 14 //number the encoder should check when turning left
#define TURN_NUMBER_RIGHT -17 //number the encoder should check when turning right
#define TURN_SPEED  30 //speed for turning
#define TURN_CORRECTION 30 //speed to turn one wheel on correction
#define TURN_CORRECTION_DELAY 50 //delay to turn for error in milliseconds
#define LEFT 0
#define RIGHT 1
#define BEHIND 2
int LEFT_ENCODER_TICKS_FORWARD;
int RIGHT_ENCODER_TICKS_FORWARD;
int leftEncoder;
int rightEncoder;

//check distance is to know if the wall is in the cell or not
#define CHECK_DISTANCE_FORWARD 12 //centimeters forward
#define CHECK_DISTANCE_LEFT 15
#define CHECK_DISTANCE_RIGHT 15

#define ISNORTH 1   //0b00000001
#define ISEAST 2    //0b00000010
#define ISSOUTH 4   //0b00000100
#define ISWEST 8    //0b00001000
#define ISEXPLORED 16 //0b00010000
#define NOTEXPLORED 239 //0b11101111
#define Y_ONLY 15   //0b00001111
#define SHIFT 4 //shift 4 bits

#define MAZE_LENGTH 12
Cell maze[MAZE_LENGTH][MAZE_LENGTH];
Cell* currentCell;
CellStack stack;
byte currentDirection;
#define USELESS 255


void setup()
{
  sparki.clearLCD();
  irRight.begin(RIGHT_IR_PIN);
  irLeft.begin(LEFT_IR_PIN);
  Serial.begin (9600);
  
  for (byte y = 0; y < MAZE_LENGTH; y++)
  {
    for(byte x = 0; x < MAZE_LENGTH; x++)
    {
    
      maze[x][y].position = (x << SHIFT); //x position
      maze[x][y].position |= (y);//y position
      maze[x][y].data = 0x00;
      
    }
  }
  //fill the distances
  byte half = MAZE_LENGTH / 2;
  for (byte y = 0; y < half; y++)
  {
    for (byte x = 0; x < half; x++)
    {
      byte m = MAZE_LENGTH - 2 - x - y;
      maze[x][y].distance = m;
      maze[MAZE_LENGTH - 1 - x][y].distance = m;
      maze[MAZE_LENGTH - 1 - x][MAZE_LENGTH - 1 - y].distance = m;
      maze[x][MAZE_LENGTH - 1 - y].distance = m;
    }
  }
  printMap();
  currentDirection = ISNORTH;
  currentCell = &maze[0][0];
  currentCell->data |= ISEAST;
  currentCell->data |= ISWEST;
  currentCell->data |= ISSOUTH;
  currentCell->data |= ISEXPLORED;
  stack.push(currentCell);
  randomSeed(analogRead(0));
}

void loop()
{
  while (currentCell->distance == 0);
  updateWalls();
  byte moveDirection = getBestDirection();
  move(moveDirection); 
}

void turn(int direction)
{
  if(direction == LEFT)
  {
    sparki.moveRight(90);
  }
  else
  {
    sparki.moveLeft(90);
  }

  delay(500);
  currentDirection = getDirection(direction);
}


void move(byte direction)
{
  if (currentDirection != direction)
  {
    if (getDirection(LEFT) == direction)
    {
      turn(LEFT);
    }
    else if (getDirection(RIGHT) == direction)
    {
      turn(RIGHT);
    }
    else
    {
      //turn around
      turn(LEFT);
      turn(LEFT);
    }
  }
  forwardDistance = getSonarDistance();
  sparki.moveBackward(18);
  
  sparki.moveStop();

  byte x = (currentCell->position >> SHIFT);
  byte y = (currentCell->position & Y_ONLY);
  switch (currentDirection)
  {
    case ISNORTH:
      y++;
      break;
    case ISEAST:
      x++;      
      break;
    case ISSOUTH:
      y--;
      break;
    case ISWEST:
      x--;
      break;
  }
  currentCell = &maze[x][y];
  if (stack.contains(currentCell))
  {
    Cell* temp;
    //keep popping cell off stack untill it finds the cell and put it back on
    //this gets rid of the unneccessary cells like in a dead end
    //I am pretty sure I spelled ^ wrong
    while (stack.pop() != currentCell);
  }
  currentCell->data |= ISEXPLORED;
  stack.push(currentCell);
}

int getSonarDistance()
{
  return sparki.ping();
}

void printMap()
{
  for (int i = 0; i < MAZE_LENGTH; i++)
  {
    for (int j = 0; j < MAZE_LENGTH; j++)
    {
      Serial.print(maze[i][j].distance);
      Serial.print(" ");
    }
    Serial.println("");
  }
}

void updateWalls()
{
  if (getSonarDistance() <= WALL_THRESHOLD_FORWARD)
  {
    updateWall(currentDirection);
  }
  if (irLeft.getDistanceCentimeter() <= WALL_THRESHOLD_LEFT)
  {
    updateWall(getDirection(LEFT));
  }
  if (irRight.getDistanceCentimeter() <= WALL_THRESHOLD_RIGHT)
  {
    updateWall(getDirection(RIGHT));
  }
}

void updateWall(byte direction)
{
  currentCell->data |= direction;
}

byte getDirection(byte direction)
{
  if (direction == LEFT)
  {
    switch (currentDirection)
    {
      case ISNORTH:
        return ISWEST;
      case ISEAST:
        return ISNORTH;
      case ISSOUTH:
        return ISEAST;
      case ISWEST:
        return ISSOUTH;
    }
  }
  else if (direction == RIGHT)
  {
    switch (currentDirection)
    {
      case ISNORTH:
        return ISEAST;
      case ISEAST:
        return ISSOUTH;
      case ISSOUTH:
        return ISWEST;
      case ISWEST:
        return ISNORTH;
    }
  } 
  else//turn around
  {
    switch (currentDirection)
    {
    case ISNORTH:
      return ISSOUTH;
    case ISEAST:
      return ISWEST;
    case ISSOUTH:
      return ISNORTH;
    case ISWEST:
      return ISEAST;
    }
  }
}

byte getBestDirection()
{
  //if the cell ahead of the robot is open and the distance is shorter than
  //the current distance, and hasn't been explored go there
  if (!(currentCell->data & currentDirection) && currentCell->distance > getCellDistance(currentDirection) && !getIsCellExplored(currentDirection))
  {
    return currentDirection;
  }

  //if the cell to the right is open and the distance is shorter than the current distance, and hasn't been explored go there
  if (!(currentCell->data & getDirection(RIGHT)) && currentCell->distance > getCellDistance(getDirection(RIGHT)) && !getIsCellExplored(currentDirection))
  {
    return getDirection(RIGHT);
  }

  //if the cell to the left is open and the distance is shorter than the current distance, and hasn't been explored go there
  if (!(currentCell->data & getDirection(LEFT)) && currentCell->distance > getCellDistance(getDirection(LEFT)) && !getIsCellExplored(currentDirection))
  {
    return getDirection(LEFT);
  }

  //if the cell ahead of the robot is open and the distance is shorter than
  //the current distance, go there
  if (!(currentCell->data & currentDirection) && currentCell->distance > getCellDistance(currentDirection))
  {
    return currentDirection;
  }

  //if the cell to the right is open and the distance is shorter than the current distance, go there
  if (!(currentCell->data & getDirection(RIGHT)) && currentCell->distance > getCellDistance(getDirection(RIGHT)))
  {
    return getDirection(RIGHT);
  }

  //if the cell to the left is open and the distance is shorter than the current distance, go there
  if (!(currentCell->data & getDirection(LEFT)) && currentCell->distance > getCellDistance(getDirection(LEFT)))
  {
    return getDirection(LEFT);
  }

  //if the cell ahead of the robot is open go there
  if (!(currentCell->data & currentDirection) && currentCell->distance > getCellDistance(currentDirection))
  {
    return currentDirection;
  }

  //if the cell to the right is open go there
  if (!(currentCell->data & getDirection(RIGHT)) && currentCell->distance > getCellDistance(getDirection(RIGHT)))
  {
    return getDirection(RIGHT);
  }

  //if the cell to the left is open go there
  if (!(currentCell->data & getDirection(LEFT)) && currentCell->distance > getCellDistance(getDirection(LEFT)))
  {
    return getDirection(LEFT);
  }
  
  return getDirection(BEHIND);
}

bool getIsCellExplored(byte direction)
{
  byte x = (currentCell->position >> SHIFT);
  byte y = (currentCell->position & Y_ONLY);
  switch (direction)
  {
  case ISNORTH:
    y++;
    break;
  case ISEAST:
    x++;
    break;
  case ISSOUTH:
    y--;
    break;
  case ISWEST:
    x--;
    break;
  }
  return maze[x][y].data & ISEXPLORED;
}

byte getCellDistance(byte direction)
{
  byte x = (currentCell->position >> SHIFT);
  byte y = (currentCell->position & Y_ONLY);
  switch (direction)
  {
    case ISNORTH:
      y++;
      if (y == MAZE_LENGTH)
      {
        return USELESS;
      }
      break;
    case ISEAST:
      x++;
      if (x == MAZE_LENGTH)
      {
        return USELESS;
      }
      break;
    case ISSOUTH:
      if (y == 0)
      {
        return USELESS;
      }
      y--;
      break;
    case ISWEST:
      if (x == 0)
      {
        return USELESS;
      }
      x--;
      break;
  }
  return maze[x][y].distance;
}
