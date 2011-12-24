from common import nmeaSentence
from datetime import datetime

def __toDMM (d):
    dd = d.split()

    if len (dd) == 3:
        m = int (dd[1]) + float(dd[2]) / 60.
        result = __toDMM (dd[0] + ' ' + str (m))
        pass
    elif len (dd) == 2:
        if dd[1].find ('.') < 2: dd[1] = '0' + dd[1]
        if 7 < len (dd[1]): dd[1] = dd[1][:7]

        result = dd[0] + dd[1]
        pass
    else:
        d = float(d)
        m = (d - int(d)) * 60.
        result = __toDMM (str (int (d)) + ' ' + str (m))
        pass
    
    return result

def factory_for_sample (when, lat, lon, alt):
    """Convert elements into a sentence and return an instance.

    when : datetime object or None
    lat  : a DMS or DMM or DDD string with the first character being N or S
    a floating point number of degrees with + being N and - being S
    lat  : a DMS or DMM or DDD string with the first character being E or W
    a floating point number of degrees with + being E and - being W
    alt  : a floating point number
    """
    if when is None: date, time = '',''
    else:
        date = "%04d-%02d-%02d" % (when.year, when.month, when.day)
        time = "%02d:%02d:%02d" % (when.hour, when.minute, when.second)
        pass

    if type (lat) is str:
        latdir = lat[0]
        lat = __toDMM (lat[1:])
        pass
    else:
        latdir = 'S' if lat < 0 else 'N'
        lat = __toDMM (str (abs (lat)))
        pass
        
    if type (lon) is str:
        londir = lon[0]
        lon = __toDMM (lon[1:])
        pass
    else:
        londir = 'W' if lon < 0 else 'E'
        lon = __toDMM (str (abs (lon)))
        pass

    if alt is not None:
        if -1 < alt.find ('.'): alt = str (int (round (float (alt))))
        else: alt = str (int (alt))
    else: alt = ''
    
    return Sample("PDIYSAM," + date + ',' + time + ',' +
                  latdir + ',' + lat + ',' +
                  londir + ',' + lon + ',' +
                  alt)

def factory_for_track (name=None, date=None):
    if date is not None:
        date = "%04d-%02d-%02d" % (date.year, date.month, date.day)
        pass
    else: date = ''

    if name is None: name=''
    
    result = Track("PDIYNT," + name + ',' + date)
    return result

def factory_for_waypoint (name):
    return

class Sample(nmeaSentence):
    """Define a sample point that came from some other tool like GPSman.

    Sample point is a GLL sentence with the potential for time and altitude.
    It is defined to be:
       PDIYSAM,{date},{time},{lat dir},{lat},{lon dir},{lon},{alt}
       where {time} is UTC
             {lat dir} is either N or S
             {lat} is  ddmm.mmmm
             {lon dir} is either E or W
             {lon} is dddmm.mmmm
    """
    def __init__ (self, payload):
        nmeaSentence.__init__ (self, payload)
        p = payload.split (',')
        self.id = p[0]

        if 0 < len (p[1]) and 0 < len (p[2]):
            self.when = datetime.strptime (p[1] + 'T' + p[2],
                                           "%Y-%m-%dT%H:%M:%S")
            pass
        else: self.when = None
        
        if 0 < p[4].find ('.'):
            self.lat = float (p[4][:p[4].find ('.')-2])
            self.lat += float (p[4][p[4].find ('.')-2:]) / 60.0
            pass
        else: self.lat = 100

        if p[3] == 'S': self.lat = -self.lat
        
        if 0 < p[6].find ('.'):
            self.lon = float (p[6][:p[6].find ('.')-2])
            self.lon += float (p[6][p[6].find ('.')-2:]) / 60.0
            pass
        else: self.lon = 200

        if p[5] == 'W': self.lon = -self.lon
        
        if 0 < len (p[7]): self.alt = int(p[7])
        else: self.alt = -1

        self._mutable = False
    pass

class Track(nmeaSentence):
    """Define the new track marker.

    The new track marker in the GPS data stream. It is defined to be:
       PDIYNT,{name},{date}
    where the {name} and {date} are user defined.
    """
    def __init__ (self, payload):
        nmeaSentence.__init__ (self, payload)
        p = payload.split (',')
        self.id = p[0]
        self.name = p[1]
        self.__date = p[2]

        if 0 < len (p[2]):
            self.date = datetime.strptime (p[2], "%Y-%m-%d").date()
            pass
        
        self._mutable = False
        return

    def defineDate (self, date):
        """Create a new Track object withthe given date.

        date - should be a datetime.date of when the track started
        
        Returns the newly created track with the desired date.
        """
        if date is not None:
            d = "%04d-%02d-%02d" % (date.year, date.month, date.day)
            payload = self.id + "," + self.name + "," + d
            result = Track(payload)
            pass
        else: result = self
        return result
    
    def defineName (self, name):
        """Create a new Track object with the given name.

        name - should be a string representing the name of the track
        
        Returns the newly created track with the desired name.
        """
        if name is not None:
            payload = self.id + "," + name + "," + self.__date
            result = Track(payload)
            pass
        else: result = self
        return result
    pass

class Waypoint(nmeaSentence):
    """Define a waypoint maker.

    The waypoint marker in the GPS data stream. It is defined to be:
       PDIYWP,{name}
    where the {name} is user defined.
    """
    def __init__ (self, payload):
        nmeaSentence.__init__ (self, payload)
        p = payload.split (',')
        self.id = p[0]
        self.name = p[1]
        self._mutable = False
        return

    def defineName (self, name):
        """Create a new Track object with the given name.

        name - should be a string representing the name of the waypoint

        Returns the newly created track with the desired name.
        """
        if name is not None:
            payload = self.id + "," + name
            result = Waypoint(payload)
            pass
        else: result = self
        return result
    pass
