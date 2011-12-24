"""The NMEA package allows easy access to NMEA protocol.
"""

# Possible errors generated when using or manipulating NMEA objects
from util import SentenceError

# Functions to help manipulate NMEA sentences
from util import load
from util import makeSentence
from util import readSentence
from util import save

# Table to define all of the available NMEA objects and how to create them
import common
import diy
import mtk
_nmeaTable = {
    "GPGGA":common.GGA,
    "GPGLL":common.GLL,
    "GPGSA":common.GSA,
    "GPGSV":common.GSV,
    "GPRMC":common.RMC,
    "GPVTG":common.VTG,
    "PDIYNT":diy.Track,
    "PDIYWP":diy.Waypoint,
    "PMTK001":mtk.Ack,
    "PMTK010":mtk.SysMsg,
    "PMTK011":mtk.Unknown,
    "PMTK514":mtk.Output,
    }
