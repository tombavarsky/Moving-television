#include <Arduino.h>
#include <Servo.h>

static const int CLAP_PIN = A7;
static const int B_PIN = 10;

Servo motor;

enum class State
{
  FIRST_STOP,
  SECOND_STOP,
  THIRD_STOP
};

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

  if ((mill - firstClapTime <= betweenClaps) && (mill - firstClapTime >= 200) && !hasSucceeded)
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

void setup()
{
  pinMode(CLAP_PIN, INPUT);
  pinMode(B_PIN, OUTPUT);

  Serial.begin(9600);

  motor.attach(3);
}

void loop()
{
  static State state = State::FIRST_STOP;

  bool clap = !digitalRead(CLAP_PIN); //false if detected clap
  bool b_val = digitalRead(B_PIN);    //true if pressed
  static bool last_b_val;
  static bool moving_up = true;
  static unsigned long press_time;
  unsigned long mill = millis();
  static bool finished_moving = true; //if the TV stoped true, if moving between stops false

  b_val = digitalRead(B_PIN);

  if (doubleClap(clap, mill) && finished_moving)
  {
    Serial.println("a");
    finished_moving = false;
    switch (state)
    {
    case State::FIRST_STOP:
      motor.write(130);
      state = State::SECOND_STOP;
      Serial.println("1");
      break;

    case State::SECOND_STOP:
      motor.write(130);
      state = State::THIRD_STOP;
      Serial.println("2");
      break;

    case State::THIRD_STOP:
      motor.write(50);
      state = State::FIRST_STOP;
      moving_up = false;
      Serial.println("3");
      break;
    }
  }
  if (b_val && !last_b_val)
  {
    if (moving_up)
    {
      if(mill - press_time >= 100){
      motor.write(90);
      press_time = mill;
      Serial.println("b");
      finished_moving = true;
      }
    }
    else
    {
      moving_up = true;
      motor.write(68);
    }
  }

  last_b_val = b_val;
}