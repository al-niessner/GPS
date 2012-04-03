"""Simulate the hardware.

The simulator takes a file of raw data and feeds it through the system as
though it was from the hardware itself. It does not emulate timing, but it
does allow for full functionality testing.
"""
import gps.nmea
import threading
import time

class Simulator(object):
    def __init__ (self, buses, logger, filename):
        object.__init__(self)
        self.__in  = buses['toHardware']
        self.__out = buses['fromHardware']
        self.__logger = logger
        self.__stop = threading.Event()
        self.__stop.clear()
        self.__port = open (filename, "r")
        return

    def read (self):
        while not self.__stop.is_set():
            l = self.__port.readline()
            time.sleep (0.1)
            
            if l is not None and 0 < len (l):
                try:
                    o = gps.nmea.readSentence (l)
                    self.__out.broadcast (o)
                    pass
                except gps.nmea.SentenceError as se:
                    self.__logger.warning ("Could not convert the sentence '"+
                                           l + "' to a useful object. " +
                                           "The actual error: " + str (se))
                    pass
                pass
            else:
                self.__logger.warning ("Simulator file is empty.")
                self.stop()
                pass
        
            pass
        self.__logger.info ("Simulator.read() is now exiting.")
        return

    def stop(self):
        self.__logger.info ("The simulation task is now stopping.")
        self.__stop.set()
        pass

    def write (self):
        q = self.__in.register()
        while not self.__stop.is_set():
            l = q.get (True)
            
            if l is not None: self.__logger.info (gps.nmea.makeSentence (l))
            else: self.stop()
                
            pass
        self.__in.unregister (q)
        return
    pass
