
import gps.nmea
import logging
import struct

_logger = logging.getLogger ("gps.nmea.util")

class SentenceError(Exception):
    pass

def load (fn):
    """Load a data file of sentences.

    Take a file and load the sentences that make sense. It will gracefully
    handle just about any error and just give a tally of the results at the
    end of loading the file.

    fn - the file to be loaded

    returns a list of objects in the order that they were read from the file
    """
    result = []
    count = 0
    f = open (fn)
    for l in f.xreadlines():
        try: result.append (readSentence (l))
        except SentenceError:
            count += 1
            pass
        pass
    f.close()
    
    if 0 < count:
        print "Had " + str (count) + " exceptions out of " + \
              str (count + len (result)) + \
              " attempts to make an object from sentences."
        pass
    
    return result

def makeSentence (o):
    """Take a NMEA object and convert it back into a NMEA sentence.

    Used to export saved information in NMEA format.
    """
    payload = str(o)
    p = struct.unpack (str (len (payload)) + "B", payload)
    cksum = p[0]
    for v in p[1:]: cksum ^= v
    result = "$%s*%02X\r\n" % (payload, cksum)
    return result

def readSentence (s):
    """Turn a NMEA sentence into a more readily usable object.

    Returns a NMAE object. These include MTK NMEA packets and packet formats
    specific for these tools. Each sentence has a standard format of:

    ${payload}*{cksum}<CR><LF>

    The $ is the preamble and the * is the payload terminator. The <CR><LF> is
    the sentence terminator.
    
    The payload is what the the objects use to fill their respective fields.
    The cksum is an xor 8 bit pattern that is encoded in a hex number where
    each 4-bit nibble is a character. The <CR><LF> mark the ned of the sentence.

    If the sentence does not follow the given format, or the checksum does not
    match the computed one, it does not end in an <CR>LF>, or it is not a known
    type, then a SentenceError is raised.
    
    For a full list of NMEA sentences handled by the is package, see __nmeaTable
    in this package.
    """
    if s.startswith ('$') and 0 < s.find ('*') and s.endswith ("\r\n"):
        payload = s[s.find ('$') + 1 :s.find ('*')]
        try: cksum = int (s[s.find ('*') + 1:s.find ('*') + 3], 16)
        except ValueError:
            raise SentenceError("Checksum is not a hex number!")
        p = struct.unpack (str (len (payload)) + "B", payload)
        cks = p[0]
        for v in p[1:]: cks ^= v

        if cks != cksum:
            raise SentenceError("Computed checksum " + hex(cks) +
                                " does not match " + hex(cksum))

        if   payload[0:2] == "GP": id = payload[0:5]
        elif payload[0:2] == "PM": id = payload[0:7]
        elif payload[0:2] == "PD": id = payload[0:6]
        else: raise SentenceError("Could not identify the packet ID '" +
                                  payload.split (",")[0] + "'.")

        if id in gps.nmea._nmeaTable: result = gps.nmea._nmeaTable[id](payload)
        else: _logger.warning ("Could not find an entry for id '" + id + "'.")
        
        pass
    else:
        raise SentenceError("Does not conform to preamble, payload, terminator.")
    
    return result

def save (fn, seq):
    """Save a sequence of objects into a file as sentences

    Takes a sequence of nmeaSentences and writes them to a file.

    fn  - the file name to write the data too
    seq - an iterable sequence of objects to write to the given filename
    """
    f = open (fn, "w")
    for o in seq: f.write (makeSentence (str (o)))
    f.close()
    return
