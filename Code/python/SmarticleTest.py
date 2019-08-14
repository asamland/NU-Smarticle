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
L = [randint(32,212) for x in range(15)]
R = [randint(32,212) for x in range(15)]
g = [L,R]
pts = 15
period = 250
header = ":GI:{:02},{:04},".format(pts,period)
bH = bytearray(header,'utf-8')
bL = bytearray(L)
bR = bytearray(R)
bMsg = bH+bL+bR + bytearray('\n','utf-8')
