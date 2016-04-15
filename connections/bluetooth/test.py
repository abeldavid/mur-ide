#-*-coding-*-:UTF8

import bluetooth as bt
import time


def connect_to_service(address, uuid):
    result = False
    spp_service_matches = bt.find_service(address=address, uuid=uuid)
    if len(spp_service_matches) > 0:
        spp_service = spp_service_matches[0]
        port = spp_service["port"]
        service_name = spp_service["name"]
        host = spp_service["host"]
        print("connecting to \"%s\" on %s" % (service_name, host))
        try:
            sock = bt.BluetoothSocket(bt.RFCOMM)
            print(bt.RFCOMM, host, port)
            sock.connect((host, port))
            print("connected")
            sock.send("HELLOWEE BLUETOOTHY WORLDY!")
            print("data sent")
            result = True
        except IOError as e:
            print(e)
            print("data not sent")
    else:
        print("spp service not found")
    return result

if __name__=="__main__":
    edison_name = "EDISON"
    service_uuid = "1101"
# edison_adress = "98:4f:ee:04:57:b2"
    edison_adress = "98:4f:ee:04:57:9b"
#    edison_address = None
    if edison_adress is None:
        devices = bt.discover_devices(lookup_names=True)
        print ("found %d devices" % len(devices))
        for addr, name in devices:
            if edison_name == name:
                edison_address = addr

    if edison_adress is not None:
        print "Edison %s found with MAC %s" % (edison_name, edison_adress)
        connected = False
        while not connected:
            connected = connect_to_service(edison_adress, service_uuid)
            time.sleep(1)

        # services = bt.find_service(address=edison_address)
        # print "Found %d services on %s" % (len(services), edison_name)
        # for service in services:
        #     print "SERVICE: %s" % (service['name'])
        #     print "protocol: %s, port %s" % (service['protocol'], service['port'])

    else:
        print "Edison not found"