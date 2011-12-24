"""Read and write directly to the LS20030-1.

During the build up of the GPS device, it was necessary to talk directly to the
LS20030-1 through a bread board to convert the 3V signal from the LS20030-1 to
the expected RS-232 values expected by the computer. It was a trivial circuit
allowing the serial bus to do most of the prototyping communications. This
module is residue from that work.

"""
import gps.nmea
import serial
import threading

class Serial(object):
    def __init__ (self, buses, logger, devname, devrate):
        object.__init__(self)
        self.__in  = buses['toHardware']
        self.__out = buses['fromHardware']
        self.__logger = logger
        self.__stop = threading.Event()
        self.__stop.clear()
        self.__port = serial.Serial(devname, baudrate=devrate)
        return

    def read (self):
        while not self.__stop.is_set():
            l = self.__port.readline()
            
            if l is not None and 0 < len (l):
                try:
                    o = gps.nmea.readSentence (l)
                    self.__out.broadcast (o)
                    pass
                except gps.nmea.SentenceError as se:
                    self.__logger.warning ("Could not convert the sentence '"+
                                           l + "' to a useful object. " +
                                           "The actual error: " + se)
                    pass
                pass
            else: self.stop()
        
            pass
        self.__logger.info ("Serial.read() is now exiting.")
        return

    def stop(self):
        self.__logger.info ("The RS232 task is now stopping.")
        self.__stop.set()
        pass

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




