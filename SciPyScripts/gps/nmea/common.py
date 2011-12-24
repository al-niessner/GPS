
from datetime import datetime
from pytz import utc

def toDeg (a, pos):
    """Convert a ddmm.mmmm string to just degrees.

    a   - the angle in ddmm.mmmm
    neg - True when north or east
    """
    if a is not None and 0 < len (a):
        result = int(float (a) / 100) + float(a[a.find ('.')-2:]) / 60.0

        if not pos: result = -result

        return result
    return

def toNumber (s):
    if s is not None and 0 < len (s):
        if 0 < s.find ("."): return float(s)
        else: return int(s)
        pass
    return

class nmeaSentence(object):
    _mutable = True
    def __init__ (self, payload):
        self._payload = payload
        return
    def __setattr__(self, name,value):
        if self._mutable: super(nmeaSentence,self).__setattr__(name,value)
        else: raise TypeError("The NMEA object " + self.id + " is immutable.")
        return
    def __delattr__(self,name):
        if self._mutable: super(nmeaSentence,self).__delattr__(name)
        else: raise TypeError("The NMEA object " + self.id + " is immutable.")
        return
    def __str__ (self):
        return self._payload
    pass

class GGA(nmeaSentence):
    __fix_table = (
        'not available or invalid',
        'GPS SPS Mode, fix valid',
        'Diff GPS, SPS Mode, fix valid',
        'Not Supported',
        'Not Supported',
        'Not Supported',
        'Dead Reckoning Mode, fix valid',
        )
    def __init__ (self, payload):
        nmeaSentence.__init__ (self, payload)
        p = payload.split (',')
        self.id = p[0]
        self.time = datetime.strptime (p[1], "%H%M%S.%f").time().replace (tzinfo=utc)
        self.lat  = toDeg (p[2], p[3] == 'N')
        self.long = toDeg (p[4], p[5] == 'E')
        self.fix = self.__fix_table[toNumber(p[6])]
        self.fixid = toNumber(p[6])
        self.sats = toNumber(p[7])
        self.hdop = toNumber(p[8])
        self.alt = toNumber(p[9])
        self.alt_units = p[10]
        self.geoid_sep = toNumber(p[11])
        self.gs_units = p[12]
        self.age = p[13]
        self.diff_id = p[14]
        self._mutable = False
        return
    
class GLL(nmeaSentence):
    __modes = {'A':'Autonomous',
               'D':'DGPS',
               'E':'DR',
               'N':'Unknown'}
    def __init__ (self, payload):
        nmeaSentence.__init__ (self, payload)
        p = payload.split (',')
        self.id = p[0]
        self.lat = toDeg (p[1], p[2] == 'N')
        self.long = toDeg (p[3], p[4] == 'E')
        self.time = datetime.strptime (p[5], "%H%M%S.%f").time().replace (tzinfo=utc)
        self.isValid = p[6] == 'A'
        self.mode = self.__modes[p[7]]
        self._mutable = False
        return
    
class GSA(nmeaSentence):
    __modes = ("Fix not available", "2D", "3D", "Unknown")
    def __init__ (self, payload):
        nmeaSentence.__init__ (self, payload)
        p = payload.split (',')
        self.id = p[0]
        self.isManual = p[1] == "M"
        self.mode = self.__modes[toNumber(p[2])]
        sids = []
        for id in p[3:15]:
            if id is not None and 0 < len (id): sids.append (toNumber (id))
            pass
        self.satIDs = tuple(sids)
        self.pdop = toNumber(p[15])
        self.hdop = toNumber(p[16])
        self.vdop = toNumber(p[17])
        self._mutable = False
        return
    
class GSV(nmeaSentence):
    def __init__ (self, payload):
        nmeaSentence.__init__ (self, payload)
        p = payload.split (',')
        self.id = p[0]
        self.messageCount = toNumber(p[1])
        self.messageNumber = toNumber(p[2])
        self.satCount = toNumber(p[3])

        if self.messageCount == self.messageNumber:
            cnt = self.satCount - (self.messageCount - 1) * 4
            pass
        else: cnt = 4
        
        ids = []
        el =[]
        az = []
        snr = []
        for i in xrange (cnt):
            ids.append (p[4+4*i])
            el.append (p[5+4*i])
            az.append (p[6+4*i])
            snr.append (p[7+4*i])
            pass
        self.satID = tuple(ids)
        self.elevation = tuple(el)
        self.azimuth = tuple(az)
        self.snr = tuple(snr)
        self._mutable = False
        return
    
class RMC(nmeaSentence):
    __modes = {'A':'Autonomous',
               'D':'DGPS',
               'E':'DR',
               'N':'Unknown'}
    def __init__ (self, payload):
        nmeaSentence.__init__ (self, payload)
        p = payload.split (',')
        self.id = p[0]
        self.isValid = p[2] == 'A'
        self.lat = toDeg (p[3], p[4] == 'N')
        self.long = toDeg (p[5], p[6] == 'E')
        self.speed = toNumber (p[7])
        self.speedUnit = "Knots"
        self.trueHeading = toNumber(p[8])
        self.ts = datetime.strptime (p[9] + " " + p[1],
                                     "%d%m%y %H%M%S.%f").replace (tzinfo=utc)
        self.ts.replace (tzinfo=utc)
        self.mode = self.__modes[p[12]]
        self._mutable = False
        return
    
class VTG(nmeaSentence):
    __modes = {'A':'Autonomous',
               'D':'DGPS',
               'E':'DR',
               'N':'Unknown'}
    __units = { 'T':'True',
                'M':'Magnetic',
                'N':'Knots',
                'K':'Km/hr' }
    def __init__ (self, payload):
        nmeaSentence.__init__ (self, payload)
        p = payload.split (',')
        self.id = p[0]
        self.heading = toNumber(p[1])
        self.headingRef = self.__units[p[2]]
        self.speed = (toNumber(p[5]), toNumber(p[7]))
        self.speedUnits = (self.__units[p[6]], self.__units[p[8]])
        self.mode = self.__modes[p[9]]
        self._mutable = False
        return
    
