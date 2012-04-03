"""Using the XuDL hardware at the end of a USB connection.
"""

import gps.nmea
import threading
import time
import usb.core
import usb.util

class XuDL(object):
    def __init__ (self, buses, logger, vendor=0x4d8, product=0xff8c):
        object.__init__(self)
        self.__in  = buses['toHardware']
        self.__out = buses['fromHardware']
        self.__logger = logger
        self.__stop = threading.Event()
        self.__stop.clear()
        self.__dev = usb.core.find (idVendor=vendor, idProduct=product)

        if self.__dev is not None:
            pass
        else:
            logger.error ("The Vendor ID " + hex (vendor) + " with product ID "+
                          hex (product) + " cannot be found.")
            raise new ValueError("XuDL device could not be found with given " +
                                 "vendor and product IDs.")
        return

    def read (self):
        return

    def stop(self):
        self.__logger.info ("The XuDL task is now stopping.")
        self.__stop.set()
        return
    
    def write (self, l):
        q = self.__in.register()
        while not self.__stop.is_set():
            l = q.get (True)
            
            if l is not None: self.__port.write (gps.nmea.makeSentence (l))
            else: self.stop()
                
            pass
        self.__in.unregister (q)
        return
    pass

    
