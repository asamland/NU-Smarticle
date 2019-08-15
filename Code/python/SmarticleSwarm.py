# SmarticleSwarm.py
# Alex Samland
# August 13, 2019
# Module for communicating with smarticle swarm over Xbee3s

import time
from XbeeComm import XbeeComm
import threading


class SmarticleSwarm(object):
    '''Class that Utilizies XbeeNU class for performing smarticle operations
    coordinated with Smarticle.h and Smarticle.cpp used on the Smarticles'''

    GI_LENGTH = 15
    ASCII_OFFSET = 32

    def __init__(self, port='/dev/tty.usbserial-DN050I6Q', baud_rate = 9600, debug = 1,servo_period_ms=0):
        '''DOC'''
        self.xb = XbeeComm(port,baud_rate,debug)
        self.servo_period_s = servo_period_ms*0.001
        self.lock = threading.Lock()


    def build_network(self, exp_no_smarticles=None):
        '''DOC'''
        self.xb.discover()
        if exp_no_smarticles != None:
            if (len(self.xb.devices)-1)<exp_no_smarticles:
                inp= input('Failed to discover all {} expected Smarticles. Retry discovery (Y/N)\n'.format(exp_no_smarticles))
                if inp[0].upper()=='Y':
                    self.build_network(exp_no_smarticles)
                else:
                    print('Network Discovery Ended\n')



    def set_servos(self, state, remote_device = None):
        '''DOC'''
        if (state==1):
            msg = ':S:1\n'
        else:
            msg = ':S:0\n'
        self.xb.command(msg, remote_device)


    def set_transmit(self, state, remote_device = None):
        '''DOC'''
        if (state==1):
            msg = ':T:1\n'
        else:
            msg = ':T:0\n'
        self.xb.command(msg, remote_device, ack)

    def set_mode(self, state, remote_device = None):
        assert (state>=0 and state<=2),"Mode must between 0-2"
        msg = ':M:{}\n'.format(int(state))
        self.xb.command(msg, remote_device)

    def set_plank(self, state, remote_device = None):
        if (state==1):
            msg = ':P:1\n'
        else:
            msg = ':P:0\n'
        self.xb.command(msg, remote_device)

    def set_pose(self, posL, posR, remote_device = None):
        '''DOC'''
        msg=':SP:{},{}\n'.format(int(posL),int(posR))
        self.xb.command(msg, remote_device)


    def gi(self, gait, period_ms=250, remote_device = None):
        gaitL=gait[0]
        gaitR=gait[1]
        gaitL = [x+self.ASCII_OFFSET for x in gaitL]
        gaitR = [x+self.ASCII_OFFSET for x in gaitR]
        assert len(gaitL)==len(gaitR),'Gait lists must be same length'
        gait_points = len(gaitL)
        if gait_points != self.GI_LENGTH:
            while len(gaitL)!=self.GI_LENGTH:
                gaitL.append(self.ASCII_OFFSET)
                gaitR.append(self.ASCII_OFFSET)
        str = ':GI:{:02},{:04};'.format(gait_points,period_ms)
        b_str = bytearray(str,'utf-8')
        msg=  b_str+bytearray(gaitL)+bytearray(gaitR)+bytearray('\n','utf-8')
        self.xb.command(msg, remote_device)


    def servo_thread_target(self, gaitf):
        '''DOC'''

        t = 0
        self.lock.acquire()
        while self.run_servos:
            self.lock.release()
            t0 = time.time()
            self.xb.broadcast('{}\n'.format(int(gaitf(t))))
            t += self.servo_period_s
            while ((time.time()-t0)<self.servo_period_s):
                time.sleep(0.001)
            self.lock.acquire()
        self.lock.release()

    def init_servo_thread(self, gait_fun=None):
        '''DOC: initializes servo thread'''
        self.servo_thread = threading.Thread(target=self.servo_thread_target, args= (gait_fun,), daemon = True)


    def start_servo_thread(self):
        '''DOC'''
        with self.lock:
            self.run_servos = 1
        self.servo_thread.start()

    def stop_servo_thread(self):
        '''DOC'''
        with self.lock:
            self.run_servos = 0
