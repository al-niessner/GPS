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

import gps.fileio.config
import threading

class ActionModel(object):
    pass

class ConfigurationModel(object):
    def __init__ (self, bus, config, logger):
        object.__init__ (self)
        self.__bus = bus
        self.__gps = config['device'] if 'device' in config else {}
        self.__listeners = []
        self.__lock = threading.RLock()
        self.__logger = logger
        self.__stop = threading.Event()
        self.__stop.clear()
        self.__window = config['window'] if 'window' in config else {}
        return

    def __convertCompass (self, v):
        if v is not None:
            if type (v) is str:
                if v == 'raw':       result = 0
                if v == '0..360':    result = 1
                if v == '-180..180': result = 2
                pass

            if type (v) is int:
                if v == 0: result = 'raw'
                if v == 1: result = '0..360'
                if v == 2: result = '-180..180'
                pass
            pass
        else: result = 0
        return result
    
    def __convertConnection (self, v):
        if v is not None:
            if type (v) is str:
                if v == 'disconnect': result = 0
                if v == 'serial':     result = 1
                if v == 'simulator':  result = 2
                if v == 'usb':        result = 3
                pass

            if type (v) is int:
                if v == 0: result = "disconnect"
                if v == 1: result = "serial"
                if v == 2: result = "simulator"
                if v == 3: result = "usb"
                pass
            
            pass
        else: result = 0
        return result
    
    def __convertTimezone (self, v):
        if v is not None:
            if type (v) is str:
                if v == 'raw':   result = 0
                if v == 'local': result = 1
                if v == 'utc':   result = 2
                pass

            if type (v) is int:
                if v == 0: result = 'raw'
                if v == 1: result = 'local'
                if v == 2: result = 'utc'
                pass
            pass
        else: result = 0
        return result
    
    def __convertUnits (self, v):
        if v is not None:
            if type (v) is str:
                if v == 'raw':    result = 0
                if v == 'metric': result = 1
                if v == 'royal':  result = 2
                pass

            if type (v) is int:
                if v == 0: result = 'raw'
                if v == 1: result = 'metric'
                if v == 2: result = 'royal'
                pass
            pass
        else: result = 0
        return result
    
    def __getState(self):
        state = {
            'autoCompass':self.__window.get ('auto',{}).get ('compass', True),
            'autoMap':self.__window.get ('auto',{}).get ('map', True),
            'autoStatus':self.__window.get ('auto',{}).get ('status', True),
            'compass':self.__gps.get ('compass', None),
            'connection':self.__gps.get ('transport', None),
            'devName':self.__gps.get ('serial', {}).get ('devname', None),
            'serialRate':self.__gps.get ('serial', {}).get ('baudrate', None),
            'sim':self.__gps.get ('simulation', None),
            'timezone':self.__gps.get ('timezone', None),
            'units':self.__gps.get ('units', None),
            'usbMan':self.__gps.get ('usb', {}).get ('manufacture', None),
            'usbPoll':self.__gps.get ('usb', {}).get ('product', None),
            'usbProd':self.__gps.get ('usb', {}).get ('pollrate', None),
            }
        state['compass'] = self.__convertCompass (state['compass'])
        state['connection'] = self.__convertConnection (state['connection'])
        state['timezone'] = self.__convertTimezone (state['timezone'])
        state['units'] = self.__convertUnits (state['units'])
        return state

    def get_pos (self, name=None):
        result = None

        if 'pos' in self.__window:
            if name is None: result = self.__window['pos']
            else: result = self.__window['pos'].get (name, None)
            pass
        
        return result
    
    def get_size (self, name=None):
        result = None

        if 'size' in self.__window:
            if name is None: result = self.__window['size']
            else: result = self.__window['size'].get (name, None)
            pass
                
        return result

    def get_units (self):
        return self.__convertUnits (self.__gps.get ('units', None))
    
    def register (self, listener):
        self.__lock.acquire()
        self.__listeners.append (listener)
        listener.update (self.__getState())
        self.__lock.release()
        return

    def set_auto (self, compass=None, map=None, status=None):
        if 'auto' not in self.__window: self.__window['auto'] = {}
        if compass is not None: self.__window['auto']['compass'] = compass
        if map is not None:     self.__window['auto']['map'] = map
        if status is not None:  self.__window['auto']['status'] = status

        gps.fileio.config.saveWindow (self.__window)
        return
    
    def set_con (self, value):
        self.__gps['transport'] = self.__convertConnection (value)
        gps.fileio.config.saveGPS (self.__gps)
        return

    def set_display (self, compass, tz, units):
        self.__gps['compass'] = self.__convertCompass (compass)
        self.__gps['timezone'] = self.__convertTimezone (tz)
        self.__gps['units'] = self.__convertUnits (units)
        gps.fileio.config.saveGPS (self.__gps)
        return

    def set_position (self, name, pos):
        if 'pos' not in self.__window: self.__window['pos'] = {}
        
        self.__window['pos'][name] = pos
        gps.fileio.config.saveWindow (self.__window)
        return
    
    def set_serial (self, dev, rate):
        if not 'serial' in self.__gps: self.__gps['serial'] = {}
        
        self.__gps['serial']['devname'] = dev
        self.__gps['serial']['baudrate'] = rate
        gps.fileio.config.saveGPS (self.__gps)
        return
    
    def set_sim (self, fn):
        self.__gps['simulation'] = str (fn)
        gps.fileio.config.saveGPS (self.__gps)
        return

    def set_size (self, name, size):
        if 'size' not in self.__window: self.__window['size'] = {}
        
        self.__window['size'][name] = size
        gps.fileio.config.saveWindow (self.__window)
        return

    def set_usb (self, man, prod, poll):
        if not 'usb' in self.__gps: self.__gps['usb'] = {}
        
        self.__gps['usb']['manufacture'] = man
        self.__gps['usb']['product'] = prod
        self.__gps['usb']['pollrate'] = poll
        gps.fileio.config.saveGPS (self.__gps)
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


class UnitModel(object):
    def __init__ (self):
        object.__init__ (self)
        return
    
    pass

