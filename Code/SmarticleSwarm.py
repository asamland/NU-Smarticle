# smarticle_swarm.py
# Alex Samland
# August 1, 2019
# Module for communicating with smarticle swarm over Xbee3s

import time
import threading
import math

from digi.xbee.models.status import NetworkDiscoveryStatus
from digi.xbee.devices import Raw802Device

class SmarticleSwarm:
    ''''Class for simplified XBee communication with remote Smarticles
        Built on top of Digi XBee python library:
            https://github.com/digidotcom/xbee-python'''


    def __init__(self, port='/dev/tty.usbserial-14120', baud_rate = 9600, debug = 1, cycle_period_s =  0.033):
        '''adds initalizes local base xbee with given port and baud rate'''

        self.base = Raw802Device(port, baud_rate)
        self.debug = debug
        self.open_base()
        self.base.add_data_received_callback(self.data_receive_callback)
        self.servo_period_s = cycle_period_s
        self.lock = threading.Lock()


    def open_base(self):
        '''Opens xbee defined by attribute 'base' '''
        success = 0
        self.base.open()
        if self.base is not None and self.base.is_open():
            sucess = 1
        elif self.debug:
            print("Failed to open device")
        self.devices = [self.base.get_node_id()]
        return sucess


    def add_remote(self,remote_device):
        '''takes in remote xbee object and adds it as attribute named after its node ID'''

        setattr(self,remote_device.get_node_id(),remote_device)
        self.devices.append(remote_device.get_node_id())


    def discover(self):
        '''Finds smarticles on network and adds them as attributes
        modified from Digi XBee example DiscoverDevicesSample.py'''

        xbee_network = self.base.get_network()

        xbee_network.set_discovery_timeout(15)  # 15 seconds.

        xbee_network.clear()

        # Callback for discovered devices.
        def callback_device_discovered(remote):
            if self.debug:
                print("Device discovered: %s" % remote)
            self.add_remote(remote)

        # Callback for discovery finished.
        def callback_discovery_finished(status):
            if self.debug:
                if status == NetworkDiscoveryStatus.SUCCESS:
                    print("Discovery process finished successfully.\nDevices: {}".format(self.devices))
                else:
                    print("There was an error discovering devices: %s" % status.description)

        xbee_network.add_device_discovered_callback(callback_device_discovered)

        xbee_network.add_discovery_process_finished_callback(callback_discovery_finished)

        xbee_network.start_discovery_process()

        if self.debug:
            print("Discovering remote XBee devices...")

        while xbee_network.is_discovery_running():
            time.sleep(0.1)


    def send(self, remote_device, msg):
        '''sends string to dest xbee, modified from digi's example SendDataSample.py'''

        if remote_device is None:
            if self.debug():
                print("Could not find the remote device")
            exit(1)
        if self.debug:
            print("Sending data to {} >> {}...".format(remote_device.get_node_id(), msg))

        self.base.send_data(remote_device, msg)

        if self.debug:
            print("Success")

    def broadcast(self, msg):
        '''broadcasts to all xbees on network'''

        self.base.send_data_broadcast(msg)

    def data_receive_callback(self,xbee_message):
        '''DOC'''

        print("From {} >> {}".format(xbee_message.remote_device.get_node_id(),
                                 xbee_message.data.decode()))


    def go(self, remote_device = None):
        '''DOC'''

        msg = 'GO!\n'
        if remote_device == None:
            self.broadcast(msg)
        else:
            self.send(remote_device,msg)


    def stop(self, remote_device = None):
        '''DOC'''

        msg = 'STOP!\n'
        if remote_device == None:
            self.broadcast(msg)
        else:
            self.send(remote_device,msg)


    @staticmethod
    def gait(t):
        '''Doc'''

        return 90+int(90*math.sin(1.5*t))


    def servo_thread_target(self, gaitf):
        '''DOC'''

        t = 0
        self.lock.acquire()
        while self.run_servos:
            self.lock.release()
            t0 = time.time()
            self.broadcast('{}\n'.format(int(gaitf(t))))
            t += self.servo_period_s
            while ((time.time()-t0)<self.servo_period_s):
                time.sleep(0.001)
            self.lock.acquire()
        self.lock.release()

    def init_servo_thread(self, gait_fun):
        '''DOC: initializes servo thread'''

        self.servo_thread = threading.Thread(target=self.servo_thread_target, args= (gait_fun,), daemon = True)


    def start_servos(self):
        '''DOC'''
        with self.lock:
            self.run_servos = 1
        swarm.servo_thread.start()

    def stop_servos(self):
        '''DOC'''
        with self.lock:
            self.run_servos = 0








swarm = SmarticleSwarm()
swarm.discover()
# swarm.send(swarm.smart1,"message");
time.sleep(5)
