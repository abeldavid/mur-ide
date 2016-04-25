# -*-coding=UTF8-*-

import zmq
import time, random, json
import bluetooth as bt
from ctypes import *

DEVICES = c_uint8 * 6

bluetooth_connection = {
    "connected": False,
    "MAC": "",
    "sock": None
}


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


def connect(host):
    service_uuid = "1101"
    sock = bt.BluetoothSocket(bt.RFCOMM)
    stat = False
    spp_service_matches = bt.find_service(address=host, uuid=service_uuid)
    if len(spp_service_matches) > 0:
        port = spp_service_matches[0]["port"]
        try:
            sock.connect((host, port))
            stat = True
        except IOError as e:
            pass
    return stat, sock


if __name__ == "__main__":
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    socket.bind("tcp://*:5556")

    devices = bt.discover_devices(lookup_names=True)

    while True:
        message = socket.recv()
        # print "Received request: ", ord(message)
        if ord(message) == 30 or ord(message) == 40:
            socket.send("")
        elif ord(message) == 200:  # get device list command
            if devices is not None:
                socket.send(json.dumps(devices))
            else:
                socket.send(json.dumps(["Fail", "no devices"]))  # temporary
        elif ord(message) == 210:  # connect command
            socket.send("Ready to receive mac")
            deviceMAC = socket.recv()
            if not bluetooth_connection["connected"] or deviceMAC != bluetooth_connection["MAC"]:
                bluetooth_connection["connected"], bluetooth_connection["sock"] = connect(deviceMAC)

            if bluetooth_connection["connected"]:
                bluetooth_connection["MAC"] = deviceMAC
                socket.send("Success")
                bluetooth_connection["sock"].send("HELLOW BLUETOOTH WORLD!")
            else:
                socket.send("Fail")
        elif ord(message) == 100:
            device_info = DEVICES(c_uint8(random.choice((1, 255))),
                                  c_uint8(random.choice((1, 255))),
                                  c_uint8(random.choice((1, 255))),
                                  c_uint8(random.choice((1, 255))),
                                  c_uint8(1), c_uint8(1))
            i = INFO(device_info,
                     c_float(random.random() * 100.0),
                     c_float(random.random() * 100.0),
                     c_float(random.random() * 100.0),
                     c_float(random.random() * 100.0),
                     c_uint8(0), c_uint8(0),
                     c_uint8(random.choice((0, 1, 2))))
            socket.send(i)
        else:
            socket.send("")
