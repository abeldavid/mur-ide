import zmq
import time
from ctypes import *

DEVICES = c_uint8 * 6


class INFO(Structure):
    _fields_ = [
                ("devicesTypes", c_uint8 * 6),
                ("yaw", c_float),
                ("pitch", c_float),
                ("roll", c_float),
                ("depth", c_float),
                ("leak", c_uint8),
                ("version", c_uint8),
                ("cameras", c_uint8)
                ]


context = zmq.Context()
socket = context.socket(zmq.REP)
socket.bind("tcp://*:5556")

while True:
    message = socket.recv()
    print "Received request: ", ord(message)
    time.sleep(1)
    if ord(message) == 30 or ord(message) == 40:
        socket.send("")
    elif ord(message) == 100:
        devices = DEVICES(c_uint8(1), c_uint8(255), c_uint8(1), c_uint8(255), c_uint8(1), c_uint8(1))
        i = INFO(devices, c_float(1), c_float(2), c_float(3),
                 c_float(40), c_uint8(0), c_uint8(0), c_uint8(1))
        socket.send(i)
