# SmarticleSwarm.py
# Alex Samland
# August 13, 2019
# Module for communicating with smarticle swarm over Xbee3s

import time
import XbeeNU


class SmarticleSwarm(object):
    '''Class that Utilizies XbeeNU class for performing smarticle operations
    coordinated with Smarticle.h and Smarticle.cpp used on the Smarticles'''

    GI_LENGTH = 15

    def __init__(self, port='/dev/tty.usbserial-DN050I6Q', baud_rate = 9600, debug = 1,cycle_period_ms):
    '''DOC'''
        self.xb = XbeeNU(port,baud_rate,debug)
        self.servo_period_s = cycle_period_ms*0.001
        self.lock = threading.Lock()


    def build_network(self, exp_no_smarticles):
        '''DOC'''
        self.xb.discover()
        if (len(self.xb.devices)-1)<exp_no_smarticles):
            inp= input('Failed to discover all expected Smarticles. Retry discovery (Y/N)\n')
            if inp[0].upper()=='Y':
                self.build_network(exp_no_smarticles)
            else:
                print('Quitting Network Discovery\n')



    def set_servos(self, state, remote_device = None):
        '''DOC'''
        if (state==1):
            msg = 'START SERVO\n'
        else:
            msg = 'STOP SERVO\n'
        self.xb.command(msg, remote_device)


    def set_transmit(self, state, remote_device = None):
        '''DOC'''
        if (state==1):
            msg = 'START TRANSMIT\n'
        else:
            msg = 'STOP TRANSMIT\n'
        self.xb.command(msg, remote_device, ack)

    def set_mode(self, state, remote_device = None):
        assert((state>=0 and state<=2),"Mode must between 0-2")
        msg = 'MODE:{}'.format(int(state))
        self.xb.command(msg, remote_device)

    def set_plank(self, state, remote_device = None):
        if (state==1):
            msg = 'PLANK\n'
        else:
            msg = 'DEPLANK\n'
        self.xb.command(msg, remote_device, ack)

    def set_pose(self, posL, posR, remote_device = None):
        '''DOC'''
        msg='POS:{},{}'.format(int(posL),int(posR))
        self.xb.command(msg, remote_device)


    def gait_interpolate(self, gaitL, gaitR, period_ms=250, remote_device = None):
        assert(len(gaitL)==len(gaitR),'Gait lists must be same length')
        gait_points = len(gaitL)
        if gait_points != self.GI_LENGTH:
            while len(gaitL)!=self.GI_LENGTH:
                gaitL.append(0)
                gaitR.append(0)
        msg = 'GI:{},{},{},{}'.format(period_ms,gait_points,gaitL,gaitR)
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
