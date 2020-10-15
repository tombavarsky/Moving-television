#include <Arduino.h>
#include <Servo.h>

// 01234567

// 01001011
// 10110100
// 11111111
// 00000000

// 01001011
// 10110101
//100000000
// 00000000

//          byte range: -128 - 127
// unsigned byte range:    0 - 255

const int CLAP_PIN = A7;
const int B_PIN = 10;

bool clap;  //false if detected clap
bool b_val; //true if pressed
bool last_b_val;
bool direction = true;
unsigned long press_time;
unsigned long mill;

Servo motor;

enum class State
{
  FIRST_STOP,
  SECOND_STOP,
  THIRD_STOP
};

State state = State::FIRST_STOP;

bool doubleClap(bool clapVal, unsigned long mill);

void setup()
{
  // put your setup code here, to run once:

  pinMode(CLAP_PIN, INPUT);
  pinMode(B_PIN, OUTPUT);

  motor.attach(3);
}

void loop()
{
  mill = millis();
  clap = !digitalRead(CLAP_PIN);
  b_val = digitalRead(B_PIN);

  if (doubleClap(clap, mill))
  {
    switch (state)
    {
    case State::FIRST_STOP:
      motor.write(155);
      state = State::SECOND_STOP;
      break;

    case State::SECOND_STOP:
      motor.write(155);
      state = State::THIRD_STOP;
      break;

    case State::THIRD_STOP:
      motor.write(30);
      state = State::FIRST_STOP;
      direction = false;
      break;
    }
  }
  if (b_val && !last_b_val)
  {
    if (direction && mill - press_time >= 100)
    {
      motor.write(90);
      press_time = mill;
    }
    else
    {
      direction = true;
    }
  }

  last_b_val = b_val;
}

bool doubleClap(bool clapVal, unsigned long mill)
{
  // return true if double taped
  static bool lastClapVal;             // for rising and falling edge
  static const int betweenClaps = 400; //the maximum time between the two claps that would count ad double tap
  static long firstClapTime;           //captures the time of the first clap
  static bool hasSucceeded;            //true if finished false if didn't

  if (!clapVal && lastClapVal)
  {
    firstClapTime = mill;
    hasSucceeded = false;
  }

  if ((mill - firstClapTime <= betweenClaps) && !hasSucceeded)
  {
    if (clapVal && !lastClapVal)
    {
      hasSucceeded = true;
      return true;
    }
  }

  lastClapVal = clapVal;
  return false;
}