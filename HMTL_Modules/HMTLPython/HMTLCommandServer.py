#!/usr/bin/python
#
# Launch a client/server HMTL command system

import sys, os
sys.path.append(os.path.join(os.path.dirname(__file__), "lib"))

from optparse import OptionParser
from server import *
from client import *
import portscan

def handle_args():
    global options

    parser = OptionParser()

    # Server options
    parser.add_option("-s", "--server", dest="servermode", action="store_true",
                      help="Run as a server", default=False)
    parser.add_option("-d", "--device", dest="device",
                      help="Arduino USB device")
    parser.add_option("-n", "--dryrun", dest="dryrun", action="store_true",
                      help="Perform dryrun only", default=False)

    # Client options
    parser.add_option("-r", "--rgb", dest="setrgb", action="store_true",
                      help="Set RGB value", default=False)
    parser.add_option("-P", "--period", dest="period", type="float",
                      help="Sleep period between changes")
    parser.add_option("-A", "--hmtladdress", dest="hmtladdress", type="int",
                      help="Address to which messages are sent")

    # General options
    parser.add_option("-v", "--verbose", dest="verbose", action="store_true",
                      help="Verbose output", default=False)
    parser.add_option("-p", "--port", dest="port", type="int",
                      help="Port to bind to", default=6000)
    parser.add_option("-a", "--address", dest="address",
                      help="Address to bind to", default="localhost")


    (options, args) = parser.parse_args()
    print("options:" + str(options) + " args:" + str(args))



    if (options.servermode and (options.device == None) and (not options.dryrun)):
        options.device = portscan.choose_port()

    # Required args
    if (options.servermode):
        pass
    else:
        pass
    
    return (options, args)
 
def main():

    handle_args()

    if (options.servermode):
        server = HMTLServer(options)
        server.listen()
        server.close()
    else:
        client = HMTLClient(options)
        client.test()
        client.close()

    print("Done.")
    exit(0)

main()

