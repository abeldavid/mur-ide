#!/usr/bin/env python
import bluetooth
import os, struct, sys
from PyOBEX import client, headers, responses
#import zmq

def find_edison_services(edison_name):
    nearby_devices = bluetooth.discover_devices(lookup_names=True)
    print("found %d devices" % len(nearby_devices))

    edison_address = None
    for addr, name in nearby_devices:
        print name, addr
        if name == edison_name:
            services = bluetooth.find_service(address=addr)
            edison_address = addr
            if len(services) > 0:
                print("found %d services on %s" % (len(services), name))
                print("")
            else:
                print("no services found")

            for svc in services:
                print("Service Name: %s" % svc["name"])
                print("    Provided By: %s" % svc["provider"])
                print("    Protocol:    %s" % svc["protocol"])
                print("    channel/PSM: %s" % svc["port"])
                print("")
    return edison_address

def push_file(address, port, file_name):
    c = client.Client(address, port)
    r = c.connect(header_list=(headers.Target("OBEXObjectPush"),))

    if not isinstance(r, responses.ConnectSuccess):
        sys.stderr.write("Failed to connect.\n")
        return

    c.put(file_name, open(file_name, "rb").read())
    c.disconnect()

if __name__ == "__main__":
    edison_address = find_edison_services("EDISON-D4-40")
    # push_file(edison_address, "666", "Hello.bin")
    print "SUCCESS"