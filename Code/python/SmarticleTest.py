#SmarticleTest.py

from SmarticleSwarm import SmarticleSwarm
from random import randint
import time


def rx_callback(xbee_message):
    '''DOC'''

    print("From {} >> {}".format(xbee_message.remote_device.get_node_id(),
                             xbee_message.data.decode()))


swarm = SmarticleSwarm()
swarm.xb.add_rx_callback(rx_callback)
swarm.build_network(4)



def random_gaits(x):
    for dev in swarm.xb.devices.values():
        L = [randint(0,180) for x in range(x)]
        R = [randint(0,180) for x in range(x)]
        gait=[L,R]
        swarm.gi(gait,remote_device=dev)
        time.sleep(0.2)
        swarm.set_pose(L[0],R[0],remote_device=dev)
        time.sleep(0.2)
