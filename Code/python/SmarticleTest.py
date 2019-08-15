#SmarticleTest.py

from SmarticleSwarm import SmarticleSwarm
from random import randint


def rx_callback(xbee_message):
    '''DOC'''

    print("From {} >> {}".format(xbee_message.remote_device.get_node_id(),
                             xbee_message.data.decode()))


swarm = SmarticleSwarm()
swarm.xb.add_rx_callback(rx_callback)
swarm.build_network(2)
L = [randint(0,180) for x in range(15)]
R = [randint(0,180) for x in range(15)]
gait=[L,R]
