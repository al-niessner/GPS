"""The model handles transforms infomation between the GUI and the other subsystems.

There are two parts to the model. The first part receives data from the bus
that connects it to the GPS data stream. It interprets the data received,
transforms it to the GUI representation, and then ships it off to the
appropriate component. The second part takes actions from the GUI and sends
them off to other subsystems that make up the GPS.

The first part that receives data from the rest of the GPS system is
encapsulated in the ViewModel. Once the data is received and transformed,
it is dispatched out to the view elements that are listening to it.
"""
import threading

class ActionModel(object):
    pass

class ViewModel(object):
    def __init__ (self, bus, logger):
        """Instantiate a ViewModel.
        """
        object.__init__ (self)
        self.__bus = bus
        self.__listeners = []
        self.__lock = threading.RLock()
        self.__logger = logger
        self.__state = {}
        self.__stop = threading.Event()
        self.__stop.clear()
        return

    def __toString (self, deg, isLat):
        if isLat:
            if deg < 0.0: sign = 'S '
            else: sign = 'N '
            pass
        else:
            if deg < 0.0: sign = 'W '
            else: sign = 'E '
            pass
        deg = abs (deg)
        d = int (deg)
        m = (deg - d) * 60
        return sign + ("%03d %2.4f" % (d,m))
    
    def __update (self, newVals):
        self.__lock.acquire()
        self.__state.update (newVals)
        for l in self.__listeners: l.update (newVals.copy())
        self.__lock.release()
        return

    def _doGPGGA (self, m):
        result = {}
        if m.alt is not None:
            result['alt'] = m.alt
            result['altUnit'] = m.alt_units.lower()
            pass
        if m.lat is not None: result['lat'] = self.__toString (m.lat, True)
        if m.long is not None: result['long'] = self.__toString (m.long, False)
        if m.hdop is not None: result['hdop'] = m.hdop
        if m.time is not None:
            result['now'] = m.time.strftime ("%H:%M:%S")
            result['zone'] = m.time.tzname()
            pass
        return result
    
    def _doGPGLL (self, m):
        result = {}
        if m.lat is not None: result['lat'] = self.__toString (m.lat, True)
        if m.long is not None: result['long'] = self.__toString (m.long, False)
        if m.time is not None:
            result['now'] = m.time.strftime ("%H:%M:%S")
            result['zone'] = m.time.tzname()
            pass
        return result
    
    def _doGPGSA (self, m):
        result = {}
        if m.hdop is not None: result['hdop'] = m.hdop
        if m.pdop is not None: result['pdop'] = m.pdop
        if m.vdop is not None: result['vdop'] = m.vdop

        result['sats'] = m.satIDs 
        return result

    __temp = { 'ids':[], 'elev':[], 'az':[], 'snr':[], }
    def _doGPGSV (self, m):
        self.__temp['ids'].extend (m.satID)
        self.__temp['elev'].extend (m.elevation)
        self.__temp['az'].extend (m.azimuth)
        self.__temp['snr'].extend (m.snr)

        if m.messageNumber == m.messageCount:
            result = { 'ids':tuple (self.__temp['ids']),
                       'elev':tuple (self.__temp['elev']),
                       'az':tuple (self.__temp['az']),
                       'snr':tuple (self.__temp['snr']) }
            self.__temp['ids'] = []
            self.__temp['elev'] = []
            self.__temp['az'] = []
            self.__temp['snr'] = []
            pass
        else: result = {}
        
        return result

    def _doGPRMC (self, m):
        result = {}
        if m.trueHeading is not None: result['heading'] = m.trueHeading
        if m.lat is not None: result['lat'] = self.__toString (m.lat, True)
        if m.long is not None: result['long'] = self.__toString (m.long, False)
        if m.speed is not None:
            result['velInst'] = m.speed
            result['velInstUnit'] = "knots"
            pass
        if m.ts is not None:
            result['today'] = m.ts.strftime ("%Y-%m-%d")
            result['now'] = m.ts.strftime ("%H:%M:%S")
            result['zone'] = m.ts.tzname()
            pass
        return result

    def _doGPVTG (self, m):
        result = {}
        if m.heading is not None: result['heading'] = m.heading
        if m.speed is not None:
            result['velInst'] = m.speed[0]
            result['velInstUnit'] = m.speedUnits[0].lower()
            pass
        return result
    
    def register (self, listener):
        self.__lock.acquire()
        self.__listeners.append (listener)
        listener.update (self.__state.copy())
        self.__lock.release()
        return

    def run (self):
        q = self.__bus.register()
        while not self.__stop.is_set():
            m = q.get (True)
            id = "_do" + m.id
            try:
                newVals = self.__getattribute__ (id)(m)
                self.__update (newVals)
                pass
            except AttributeError as ae:
                self.__logger.warning ("Could not find a method for message " +
                                       "id '" + m.id +"'. Actual error was: " +
                                       str (ae))
                pass
            pass
        self.__bus.unregister (q)
        self.__logger.info ("ViewModel.run() is now exiting.")
        return
    
    def stop (self):
        self.__logger.info ("View Model task is now stopping.")
        self.__stop.set()
        return
    
    def unregister (self, listener):
        self.__lock.acquire()
        if listener in self.__listeners: __listeners.remove (listener)
        else: self.__logger.warning ("Listener tried to unregister that was never registered in the first place.")
        self.__lock.release()
        return
    
    pass

