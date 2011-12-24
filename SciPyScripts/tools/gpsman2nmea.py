#! /usr/bin/env python
"""Convert gpsman data files to NMEA message files.
"""

from argparse import ArgumentParser
from collections import namedtuple
from datetime import datetime
from gps.nmea.diy import factory_for_sample
from gps.nmea.diy import factory_for_track
from gps.nmea.diy import factory_for_waypoint
from gps.nmea.util import makeSentence

import os
import sys

def _readGPSMAN (ifn):
    """Read a GPSMAN data file and returns two lists tracks and waypoints.
    """
    Point = namedtuple('Point', 'when lat lon alt')
    Track = namedtuple('Track', 'name points')
    Way   = namedtuple('Way', 'name point')
    tracks = []
    waypoints = []
    
    if not os.path.exists (ifn):
        print "The file '" + ifn + "' does not exist. Doing nothing."
        pass
    else:
        track = None
        wpnt = False
        inf = open (ifn)
        for l in inf:
            l = l.strip()
            
            if l.startswith ('%'): continue # gpsman comment
            if len (l) < 1: continue # white space to make the file readable

            if l[0] == '!':
                if l.startswith ("!W:"):
                    wpnt = True
                    track = None
                    pass
                elif l.startswith ("!T:"):
                    ll = l.split ("\t")
                    wpnt = False
                    track = []
                    tracks.append (Track(ll[1], track))
                    pass
                elif l.startswith ("!Format:"):
                    wpnt = False
                    track = None
                    ll = l.split()

                    if ll[1] != "DMS":
                        inf.close()
                        print "Need to extend converter to handle non DMS file."
                        sys.exit (-2)
                        pass
                    pass
                elif l.startswith ("!Creation"):
                    wpnt = False
                    track = None
                    pass
                elif l.startswith ("!Position"):
                    print "No longer DMS: " + l
                    inf.close()
                    sys.exit (-3)
                    pass
                elif l.startswith ("!NB:"): pass
                elif l.startswith ("!TS:"): pass
                else: print "Command '" + l + "' is not understood."
                pass
            elif wpnt:
                ll = l.split ("\t")
                if len (ll) < 6: waypoints.append (Way(ll[0],
                                                        Point(ll[1],
                                                              ll[2],
                                                              ll[3],
                                                              '')))
                else: waypoints.append (Way(ll[0],
                                            Point(ll[1],
                                                  ll[2],
                                                  ll[3],
                                                  ll[5][4:])))
                pass
            elif track is not None:
                ll = l.split ("\t")
                track.append (Point(ll[0], ll[1], ll[2], ll[3]))
                pass
            else: print l + " is not understood."
            
            pass
        inf.close()
        pass
    
    return tracks, waypoints

def main (context):
    """Process the given context.

    Opens the GPSman file in read mode and reads in all of the waypoints and
    tracks. Unfortunately, the GPSman waypoints and tracks do not have a simple
    method of reintegration. They must be reintergrated by matching lat/lon
    values. Given lots of waypoints and tracks, this could take a while.

    The first thing to do is to make sure that the input file exists. If it
    does, then need to read in all of the waypoints and tracks.

    Once read in, the data converted to NMEA messages and pushed out to the
    output file. If the output file name is not specified (None) then the
    input file name is stripped of its extension and used. If track_per_file
    is False, then the extension 'nmea' is added to the file and the converted
    NMEA stream is appended to the file. If track_per_file is True, then the
    extension 'Trip' is added to the file name. If the file currently exists
    and is not a directory, then the program aborts.

    context : a Namespace that contains three attributes:
              input_file     : GPSman file to convert
              output_file    : output file name (see rules above for details)
              track_per_file : boolean indicating if GPSman tracks should be
                               extracted to separate files (recommended)
    """
    if context.output_file is None:
        bn = context.input_file[:context.input_file.rfind (os.path.extsep)]

        if context.track_per_file:
            bn = bn + os.path.extsep + "Trip"

            if os.path.exists (bn) and not os.path.isdir (bn):
                raise ValueError("The file '" + bn + "' exists but is not a " +
                                 "direcotry.")

            if not os.path.exists (bn): os.makedirs (bn)
            pass
        else: bn = bn + os.path.extsep + "nmea"
        pass

    f = None
    tracks, waypnts = _readGPSMAN (context.input_file)
    for t in tracks:
        if context.track_per_file:
            if f is not None: f.close()
            
            f = open (os.path.join (bn, t[0] + os.path.extsep + "nmea"), 'w')
            pass
        else: f = open (os.path.join (bn), 'a') if f is None else f

        f.write (makeSentence (factory_for_track (name=t[0])))
        for s in t[1]:
            f.write (
                makeSentence (
                factory_for_sample (when=datetime.strptime (s.when,
                                                            "%d-%b-%Y %H:%M:%S"),
                                    lat=s.lat,
                                    lon=s.lon,
                                    alt=s.alt)))
        pass
    f.close()
    print tracks[0][1][0]
    print waypnts[0]
    pass

if __name__ == "__main__":
    ap = ArgumentParser(description="Convert GPSman data files to NMEA message files so that they can be handled natively by the rest of this software.")
    ap.add_argument ("--input-file", "-i", type=str, required=True,
                     help="name of the GPSman file to convert")
    ap.add_argument ("--output-file", "-o", default=None, required=False,
                     help="name of the file to fill with the NMEA messages")
    ap.add_argument ("--track-per-file", "-t", action='store_true',
                     default=False, required=False,
                     help="extract each track into its own file")
    args = ap.parse_args()
    main (args)
    pass
