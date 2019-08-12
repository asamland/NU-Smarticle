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
  1   --    Streamed Gait
  2   --    Interpolated Gait


*/

#ifndef __SMARTICLE_H__
#define __SMARTICLE_H__

#include "Arduino.h"
#include "PWMServo.h"
#include "NeoSWSerial.h"

#define MAX_US 2500
#define MIN_US 600
#define RX_PIN 2
#define TX_PIN 3
#define LED 13
#define SERVO_L 10
#define SERVO_R 9
#define PRF A2
#define PRB A0
#define MIC A3





enum STATES{IDLE, STREAM, INTERP};

class Smarticle
{
  public:
    Smarticle(int debug=0, int run_servos=0, int transmit=0, int sample_time_ms = 15, int cycle_period_ms = 33);
    void inactive(void);
    void timer_interrupt(void);
    void gait_interpolate(int delay, int len, int* servoL_arr, int* servoR_arr);
    void stream_servo(void);
    int set_mode(int mode);
    int init_mode(void);
    enum STATES get_mode(void);
    int * read_sensors(void);
    int transmit_data(void);
    int interp_msg(void);
    int attach_servos(void);
    int detach_servos(void);
    int run_servos(int);
    int transmit(int);
    int enable_t2_interrupts(void);
    int disable_t2_interrupts(void);
    PWMServo ServoL;
    PWMServo ServoR;
    NeoSWSerial Xbee;
    int cycle_time_ms;
    int sensor_dat[3];
    int msg_flag = 0;
  private:
    void _plankf(void);
    enum STATES _mode;
    float _t = 0;
    int _debug;
    int _run_servos;
    int _transmit;
    String  _input_string;
    int _sample_time_ms;
    int _gaitL[15];
    int _gaitR[15];
    int _servo_period;
    int _servo_pts;
    int _plank;

};

#endif
