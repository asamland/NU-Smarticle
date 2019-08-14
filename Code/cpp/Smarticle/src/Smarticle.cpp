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
  1   --    Streamed Gait
  2   --    Interpolated Gait

*/

#include <Smarticle.h>

Smarticle:: Smarticle(int debug, int run_servos, int transmit, int sample_time_ms, int cycle_period_ms):Xbee(RX_PIN,TX_PIN)
{
  pinMode(LED,OUTPUT);
  _debug = debug;
  _mode = IDLE;
  _run_servos = run_servos;
  _transmit = transmit;
  // _input_string.reserve(200);
  _sample_time_ms = sample_time_ms;
  cycle_time_ms = cycle_period_ms;
  //set prescaler to 32 >> overflow period = 2.048ms
  TCCR2B = (0<<CS20)|(0<<CS21)|(1<<CS22);
  /* Clear overflow flag */
  TIFR2  = 0<<TOV2;
}

void Smarticle::led_on(int stat)
{
  if (stat==1){
    digitalWrite(LED,HIGH);
  }else{
    digitalWrite(LED,LOW);
  }
  return;
}

void Smarticle::inactive(void)
{
  return;
}
void Smarticle::_plankf(void)
{
  ServoL.write(90);
  ServoR.write(90);
  disable_t2_interrupts();
}

void Smarticle::timer_interrupt(void)
{
  if (_run_servos&_mode!=IDLE){
    if (_plank==1){
      _plankf();
    }else{
      switch (_mode) {
        case INTERP: gait_interpolate(_gait_period, _gait_pts, _gaitL, _gaitR);break;
        case STREAM: stream_servo();break;
        default: return;
      }
    }
  }
  return;
}

void Smarticle::gait_interpolate(int delay, int len, int *servoL_arr, int *servoR_arr)
{
  static int tt, index;
  if (tt>=(2*delay)){
    tt = 0;
    if (index>= len){
      index = 0;
    }
    ServoL.write(servoL_arr[index]);
    ServoR.write(servoR_arr[index]);
    index++;
  }
  tt++;
  return;
}

void Smarticle:: stream_servo(void)
{
  return;
}

int Smarticle::set_mode(int mode)
{
  Xbee.printf("Mode input: %d\n", mode);
  switch(mode){
    case 0: _mode = IDLE;break;
    case 1: _mode = STREAM;break;
    case 2: _mode = INTERP;break;
    default: _mode = IDLE;
  }
  init_mode();
  _plank = 0;
  Xbee.printf("DEBUG: Mode Set to: %d!\n",_mode);
  return 1;
}

int Smarticle::init_mode()
{
  if (_mode==STREAM ){
    attach_servos();
  }else if (_mode==INTERP){
      attach_servos();
      _gait_pts=1;
      _gait_period = 500;
      _gaitL[0]=90;
      _gaitR[0]=90;
  }else{
    detach_servos();
  }
}


enum STATES Smarticle::get_mode(void)
{
  return _mode;
}

int Smarticle::interp_msg(void)
{
  Xbee.printf("DEBUG: MESSAGE RECEIVED!!\n");
  String InputString=String(_input_string);
  InputString = String(_input_string);

  Xbee.println(InputString);
  _input_string="";
  disable_t2_interrupts();
  msg_flag = 0;
  Xbee.println(InputString.substring(0,3));
  if (_mode==INTERP&InputString.substring(0,3)=="GI:"){
    char str[100];
    InputString.toCharArray(str,100);
    if(sscanf(str,"GI:%d,%d,[%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d],[%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d]",
    &_gait_period,&_gait_pts,&_gaitL[0],&_gaitL[1],&_gaitL[2],&_gaitL[3],&_gaitL[4],&_gaitL[5],&_gaitL[6],&_gaitL[7],&_gaitL[8],&_gaitL[9],&_gaitL[10],&_gaitL[11],&_gaitL[12],&_gaitL[13],&_gaitL[14],
    &_gaitR[0],&_gaitR[1],&_gaitR[2],&_gaitR[3],&_gaitR[4],&_gaitR[5],&_gaitR[6],&_gaitR[7],&_gaitR[8],&_gaitR[9],&_gaitR[10],&_gaitR[11],&_gaitR[12],&_gaitR[13],&_gaitR[14])){
      // _plankf();
      _plank=0;
      Xbee.printf("DEBUG: GI %dms period, %d points\n",_gait_period,_gait_pts);
      delay(500);
    }
  }else if (_mode==STREAM){
  }else if (InputString.substring(0,5)=="MODE:"){
    Xbee.printf("DEBUG: MODE CHANGE!\n");
    _run_servos = 0;
    char buff[30];
    InputString.toCharArray(buff,30);
    int m=0;
    if (InputString.length()==6){
      Xbee.printf("matches length\n");
      sscanf(buff,"MODE:%d,",&m);
    }
    Xbee.printf("m = %d\n",m);
    set_mode(m);
    enable_t2_interrupts();
    return 1;
  } else if(InputString=="STOP SERVO"){ run_servos(0);
} else if(InputString=="START SERVO"){ run_servos(1);
} else if(InputString=="START TRANSMIT"){ transmit(1);
} else if(InputString=="STOP TRANSMIT"){ transmit(0);
} else if(InputString=="PLANK"){ _plank=1; Xbee.printf("DEBUG: START PLANK!\n");
} else if(InputString=="DEPLANK"){ _plank=0; Xbee.printf("DEBUG: STOP PLANK!\n");
} else if (InputString.substring(0,4)=="POS:"){
  _run_servos=0;
  char buff[30];
  InputString.toCharArray(buff,30);
  int angL=90,angR=90;
  sscanf(buff,"POS:%d,%d",&angL, &angR);
  ServoL.write(angL);
  ServoR.write(angR);

  } else{
    Xbee.printf("DEBUG: no match :(\n");
    // enable_t2_interrupts();
    return 0;
  }
  enable_t2_interrupts();
  return 1;
}


int * Smarticle::read_sensors(void)
{
  unsigned long startTime= millis();  // Start of sample window
  int dat[3]={0,0,0};
  while(millis() - startTime < 30) {
      dat[0] = max(dat[0],analogRead(PRF));
      dat[1] = max(dat[1],analogRead(PRB));
      dat[2] = max(dat[2],analogRead(MIC));
    }
  sensor_dat[0]=dat[0];
  sensor_dat[1]=dat[1];
  sensor_dat[2]=dat[2];
  return sensor_dat;
}


int Smarticle::transmit_data(void)
{
  if (_transmit){
    Xbee.printf("%d, %d, %d\n",sensor_dat[0], sensor_dat[1], sensor_dat[2]);
  }
  return 1;
}


int Smarticle::attach_servos(void)
{
  ServoL.attach(SERVO_L,MIN_US,MAX_US);
  ServoR.attach(SERVO_R,MIN_US,MAX_US);
  ServoL.write(90);
  ServoR.write(90);
  Xbee.printf("DEBUG: Servos Attached!\n");
}

int Smarticle::detach_servos(void)
{
  ServoL.detach();
  ServoR.detach();
  Xbee.printf("DEBUG: Servos Detached!\n");
}


int Smarticle::run_servos(int run)
{
   Xbee.printf("DEBUG: TOGGLE SERVOS!\n");
  _run_servos = run;
  return 1;
}

int Smarticle::transmit(int run)
{
  Xbee.printf("DEBUG: TOGGLE TRANSMIT!\n");
  _transmit = run;
  return 1;
}

int Smarticle::enable_t2_interrupts(void)
{
  TIMSK2 = 1<<TOIE2;
  Xbee.printf("DEBUG: T2 interrupt enabled!\n");
}

int Smarticle::disable_t2_interrupts(void)
{
  TIMSK2 = 0<<TOIE2;
  Xbee.printf("DEBUG: T2 interrupt disabled!\n");
}

void Smarticle::rx_interrupt(uint8_t c)
{
  if (c=='\n'){
    msg_flag=1;
  }else{
    _input_string+= (char) c;
  }

}
