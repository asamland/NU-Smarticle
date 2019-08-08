/*
  Smarticle.h - Arduino Library for NU-Smarticle
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

#ifndef __SMARTICLE_H__
#define __SMARTICLE_H__

#include "Arduino.h"
#include "PWMServo.h"
#include "NeoSWSerial.h"

#define MAX_US 2500
#define MIN_US 600




enum STATES{IDLE, PLANK, STREAM, SQUARE, TRI, SIN, S_WAVE};

class Smarticle
{
  public:
    Smarticle(int debug=0, int run_servos=0, int transmit=0, double sample_time_s = 0.01, double cycle_time_s = 0.033);
    void inactive(void);
    void square_gait(void);
    void triangle_gait(void);
    void sin_gait(int amp, double freq);
    void stream_servo(void);
    void plank(void);
    int set_mode(int mode);
    int get_mode(void);
    int read_sensors(void);
    int transmit_data(void);
    int init_xbee(void);
    int run_servos(void);
    PWMServo ServoL;
    PWMServo ServoR;
    NeoSWSerial Xbee;
    double cycle_time_s = 0.04;
  private:
    const int _prb = A0; //photoresistor back
    const int _prf = A2; //photoresistor front
    const int _mic = A3;
    const int _ssRX = 2; //software serial RX
    const int _ssTX = 3; //software serial TX
    const int _servo_right = 9;
    const int _servo_left = 10;
    const int _LED = 13;
    enum STATES _mode;
    float _t = 0;
    int _debug;
    int _run_servos;
    int _transmit;
    char  _input_string[50];
    double _sample_time_s= 0.010;
    float _cycle_period_s = 0.033;
    float sin_freq;
    int sin_amp;

};

#endif
