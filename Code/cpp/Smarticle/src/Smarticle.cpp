/*
  Smarticle.cpp - Arduino Library for NU-Smarticle
  header file for
  Alex Samland, created Aug 7, 2019
  Last Updated: Aug 7, 2019
  v1.0

  NU-Smarticle v1 Pins:
  A0    --    Photoresistor Back
  A1    --    Servo Current Sense
  A2    --    Photoresistor Front
  A3    --    Microphone (will be changed to resolve pin conflict)
  A3    --    Photoresistor Left
  D2    --    XBee Dout (Arduino serial RX)
  D3    --    XBee Din (Arduino serial TX)
  D7    --    External pth LED
  D9    --    Right Servo Signal (bridged through D4)
  D10   --    Left Servo Signal
  D13   --    SCK Red Led


  Smarticle Modes:
  0   --    Inactive
  1   --    Plank
  2   --    Streamed Gait
  3   --    Square Gait
  4   --    Triangle Gait
  5   --    Sine Gait
  6   --    S-wave Gait

*/

#include <Smarticle.h>

Smarticle:: Smarticle(int debug, int run_servos, int transmit, double sample_time_s, double cycle_time_s):Xbee(_ssRX,_ssTX)
{
  _debug = debug;
  _mode = IDLE;
  _run_servos = run_servos;
  _transmit = transmit;
  _sample_time_s = sample_time_s;
  _cycle_period_s = cycle_time_s;
  Xbee.begin(9600);
  Serial.println("GO!");
}

void Smarticle::inactive(void)
{

  return;
}

void Smarticle:: plank(void)
{
  return;
}

void Smarticle:: stream_servo(void)
{
  return;
}

void Smarticle:: square_gait(void)
{
  return;
}

void Smarticle:: triangle_gait(void)
{
  return;
}

void Smarticle:: sin_gait(int amp, double freq)
{
  int ang;
  ang = 90+amp*sin(freq*_t);
  _t =(_t+_cycle_period_s);
  if (_t*freq >= 6.283185){
    _t = 0;
  }
  Serial.println(ang);
  if (_run_servos==1){
    // ServoR.write(ang);
    ServoL.write(ang);
  }
}

int Smarticle::run_servos(void)
{
  _run_servos = 1;
  _t = 0;
  return 1;
}
