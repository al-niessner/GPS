#!/usr/bin/env python

import wx

EVT_NEW_DATA_TYPE = wx.NewEventType()
EVT_NEW_DATA = wx.PyEventBinder (EVT_NEW_DATA_TYPE)

class NewDataEvent(wx.PyEvent):
    def __init__ (self, winid=0, et=EVT_NEW_DATA_TYPE, data={}):
        wx.PyEvent.__init__ (self, winid, et)
        self.data = data
        pass
    pass

class StatusView(wx.Panel):
    def __init__(self, logger, *args, **kwds):
        # Initial connection to the rest of the world
        kwds["style"] = wx.TAB_TRAVERSAL
        wx.Panel.__init__(self, *args, **kwds)

        self.__logger = logger
        self.Bind (EVT_NEW_DATA, self.__update)

        # Instantiate the elements contained in the GUI
        self.__lblToday = wx.StaticText(self, -1, "Today")
        self.__lblNow = wx.StaticText(self, -1, "Now")
        self.__lblLat = wx.StaticText(self, -1, "Lattitude")
        self.__lblLong = wx.StaticText(self, -1, "Longitude")
        self.__lblAlt = wx.StaticText(self, -1, "Altitude")
        self.__lblPdop = wx.StaticText(self, -1, "P and H DOP")
        self.__lblDis = wx.StaticText(self, -1, "Distance")
        self.__lblDisTrack = wx.StaticText(self, -1, "Track")
        self.__lblDisTrip = wx.StaticText(self, -1, "Trip")
        self.__lblDur = wx.StaticText(self, -1, "Duration")
        self.__lblDurTrack = wx.StaticText(self, -1, "Track")
        self.__lblDurTrip = wx.StaticText(self, -1, "Trip")
        self.__lblVel = wx.StaticText(self, -1, "Velocity")
        self.__lblVelInst = wx.StaticText(self, -1, "Instantaneous")
        self.__lblVelTrack = wx.StaticText(self, -1, "Track")
        self.__lblVelTrip = wx.StaticText(self, -1, "Trip")
        self.__lblDev = wx.StaticText(self, -1, "Device")
        self.__lblConn = wx.StaticText(self, -1, "Connection")
        self.__lblState = wx.StaticText(self, -1, "State")

        self._today =  wx.StaticText(self, -1, "-- no date --",
                                     style=wx.ST_NO_AUTORESIZE)
        self._now =  wx.StaticText(self, -1, "-- no time --",
                                   style=wx.ST_NO_AUTORESIZE)
        self._zone = wx.StaticText(self, -1, "-- no zone --",
                                   style=wx.ST_NO_AUTORESIZE)
        self._lat =  wx.StaticText(self, -1, "-- no lattitude --",
                                   style=wx.ST_NO_AUTORESIZE)
        self._long =  wx.StaticText(self, -1, "-- no longitude --",
                                    style=wx.ST_NO_AUTORESIZE)
        self._alt =  wx.StaticText(self, -1, "-- no altitude --",
                                   style=wx.ST_NO_AUTORESIZE)
        self._pdop =  wx.StaticText(self, -1, "-- no accuracy --",
                                      style=wx.ST_NO_AUTORESIZE)
        self._hdop =  wx.StaticText(self, -1, "-- no accur --",
                                      style=wx.ST_NO_AUTORESIZE)
        self._disTrack =  wx.StaticText(self, -1, "-- no distance --",
                                        style=wx.ST_NO_AUTORESIZE)
        self._disTrip =  wx.StaticText(self, -1, "-- no distance --",
                                       style=wx.ST_NO_AUTORESIZE)
        self._durTrack =  wx.StaticText(self, -1, "-- no duration --",
                                        style=wx.ST_NO_AUTORESIZE)
        self._durTrip =  wx.StaticText(self, -1, "-- no duration --",
                                       style=wx.ST_NO_AUTORESIZE)
        self._velInst =  wx.StaticText(self, -1, "-- no velocity --",
                                       style=wx.ST_NO_AUTORESIZE)
        self._velTrack =  wx.StaticText(self, -1, "-- no velocity --",
                                        style=wx.ST_NO_AUTORESIZE)
        self._velTrip =  wx.StaticText(self, -1, "-- no velocity --",
                                       style=wx.ST_NO_AUTORESIZE)
        self._devConn = wx.StaticText(self, -1, "-- no connection --",
                                      style=wx.ST_NO_AUTORESIZE)
        self._devState = wx.StaticText(self, -1, "-- no state --",
                                       style=wx.ST_NO_AUTORESIZE)
        self._altUnit = wx.StaticText(self, -1, "-- no units --",
                                      style=wx.ST_NO_AUTORESIZE)
        self._disTrackUnit = wx.StaticText(self, -1, "-- no units --",
                                           style=wx.ST_NO_AUTORESIZE)
        self._disTripUnit = wx.StaticText(self, -1, "-- no units --",
                                          style=wx.ST_NO_AUTORESIZE)
        self._velInstUnit = wx.StaticText(self, -1, "-- no units --",
                                          style=wx.ST_NO_AUTORESIZE)
        self._velTrackUnit = wx.StaticText(self, -1, "-- no units --",
                                           style=wx.ST_NO_AUTORESIZE)
        self._velTripUnit = wx.StaticText(self, -1, "-- no units --",
                                          style=wx.ST_NO_AUTORESIZE)
        
        # Configure the elements and then do the layout of the panel
        self.__do_layout()
        return

    def __do_layout(self):
        gbs = wx.GridBagSizer(2, 2)
        
        gbs.Add (self.__lblToday,
                 wx.GBPosition(row=0, col=0),
                 wx.GBSpan(rowspan=1, colspan=5))
        gbs.Add (self._today,
                 wx.GBPosition(row=0, col=6),
                 wx.GBSpan(rowspan=1, colspan=4))
        gbs.Add (self.__lblNow,
                 wx.GBPosition(row=1, col=0),
                 wx.GBSpan(rowspan=1, colspan=5))
        gbs.Add (self._now,
                 wx.GBPosition(row=1, col=6),
                 wx.GBSpan(rowspan=1, colspan=4))
        gbs.Add (self._zone,
                 wx.GBPosition(row=0, col=10),
                 wx.GBSpan(rowspan=2, colspan=1),
                 wx.ALIGN_CENTER_VERTICAL)

        gbs.Add (self.__lblLat,
                 wx.GBPosition(row=3, col=0),
                 wx.GBSpan(rowspan=1, colspan=5))
        gbs.Add (self._lat,
                 wx.GBPosition(row=3, col=6),
                 wx.GBSpan(rowspan=1, colspan=5))
        gbs.Add (self.__lblLong,
                 wx.GBPosition(row=4, col=0),
                 wx.GBSpan(rowspan=1, colspan=5))
        gbs.Add (self._long,
                 wx.GBPosition(row=4, col=6),
                 wx.GBSpan(rowspan=1, colspan=5))
        gbs.Add (self.__lblAlt,
                 wx.GBPosition(row=5, col=0),
                 wx.GBSpan(rowspan=1, colspan=5))
        gbs.Add (self._alt,
                 wx.GBPosition(row=5, col=6),
                 wx.GBSpan(rowspan=1, colspan=4))
        gbs.Add (self._altUnit,
                 wx.GBPosition(row=5, col=10),
                 wx.GBSpan(rowspan=1, colspan=1))

        gbs.Add (self.__lblPdop,
                 wx.GBPosition(row=7, col=0),
                 wx.GBSpan(rowspan=1, colspan=5))
        gbs.Add (self._pdop,
                 wx.GBPosition(row=7, col=6),
                 wx.GBSpan(rowspan=1, colspan=4))
        gbs.Add (self._hdop,
                 wx.GBPosition(row=7, col=10),
                 wx.GBSpan(rowspan=1, colspan=1))

        gbs.Add (self.__lblDis,
                 wx.GBPosition(row=9, col=0),
                 wx.GBSpan(rowspan=1, colspan=5))
        gbs.Add (self.__lblDisTrack,
                 wx.GBPosition(row=10, col=1),
                 wx.GBSpan(rowspan=1, colspan=4))
        gbs.Add (self._disTrack,
                 wx.GBPosition(row=10, col=6),
                 wx.GBSpan(rowspan=1, colspan=4))
        gbs.Add (self._disTrackUnit,
                 wx.GBPosition(row=10, col=10),
                 wx.GBSpan(rowspan=1, colspan=1))
        gbs.Add (self.__lblDisTrip,
                 wx.GBPosition(row=11, col=1),
                 wx.GBSpan(rowspan=1, colspan=4))
        gbs.Add (self._disTrip,
                 wx.GBPosition(row=11, col=6),
                 wx.GBSpan(rowspan=1, colspan=4))
        gbs.Add (self._disTripUnit,
                 wx.GBPosition(row=11, col=10),
                 wx.GBSpan(rowspan=1, colspan=1))
        
        gbs.Add (self.__lblDur,
                 wx.GBPosition(row=13, col=0),
                 wx.GBSpan(rowspan=1, colspan=5))
        gbs.Add (self.__lblDurTrack,
                 wx.GBPosition(row=14, col=1),
                 wx.GBSpan(rowspan=1, colspan=4))
        gbs.Add (self._durTrack,
                 wx.GBPosition(row=14, col=6),
                 wx.GBSpan(rowspan=1, colspan=5))
        gbs.Add (self.__lblDurTrip,
                 wx.GBPosition(row=15, col=1),
                 wx.GBSpan(rowspan=1, colspan=4))
        gbs.Add (self._durTrip,
                 wx.GBPosition(row=15, col=6),
                 wx.GBSpan(rowspan=1, colspan=5))

        gbs.Add (self.__lblVel,
                 wx.GBPosition(row=17, col=0),
                 wx.GBSpan(rowspan=1, colspan=5))
        gbs.Add (self.__lblVelInst,
                 wx.GBPosition(row=18, col=1),
                 wx.GBSpan(rowspan=1, colspan=4))
        gbs.Add (self._velInst,
                 wx.GBPosition(row=18, col=6),
                 wx.GBSpan(rowspan=1, colspan=4))
        gbs.Add (self._velInstUnit,
                 wx.GBPosition(row=18, col=10),
                 wx.GBSpan(rowspan=1, colspan=1))
        gbs.Add (self.__lblVelTrack,
                 wx.GBPosition(row=19, col=1),
                 wx.GBSpan(rowspan=1, colspan=4))
        gbs.Add (self._velTrack,
                 wx.GBPosition(row=19, col=6),
                 wx.GBSpan(rowspan=1, colspan=4))
        gbs.Add (self._velTrackUnit,
                 wx.GBPosition(row=19, col=10),
                 wx.GBSpan(rowspan=1, colspan=1))
        gbs.Add (self.__lblVelTrip,
                 wx.GBPosition(row=20, col=1),
                 wx.GBSpan(rowspan=1, colspan=4))
        gbs.Add (self._velTrip,
                 wx.GBPosition(row=20, col=6),
                 wx.GBSpan(rowspan=1, colspan=4))
        gbs.Add (self._velTripUnit,
                 wx.GBPosition(row=20, col=10),
                 wx.GBSpan(rowspan=1, colspan=1))

        gbs.Add (self.__lblDev,
                 wx.GBPosition(row=22, col=0),
                 wx.GBSpan(rowspan=1, colspan=5))
        gbs.Add (self.__lblConn,
                 wx.GBPosition(row=23, col=1),
                 wx.GBSpan(rowspan=1, colspan=4))
        gbs.Add (self._devConn,
                 wx.GBPosition(row=23, col=6),
                 wx.GBSpan(rowspan=1, colspan=5))
        gbs.Add (self.__lblState,
                 wx.GBPosition(row=24, col=1),
                 wx.GBSpan(rowspan=1, colspan=4))
        gbs.Add (self._devState,
                 wx.GBPosition(row=24, col=6),
                 wx.GBSpan(rowspan=1, colspan=5))

        self.SetSizer (gbs)
        gbs.Fit (self)
        gbs.AddGrowableCol (5)
        gbs.AddGrowableRow (2)
        gbs.AddGrowableRow (6)
        gbs.AddGrowableRow (8)
        gbs.AddGrowableRow (12)
        gbs.AddGrowableRow (16)
        gbs.AddGrowableRow (21)
        return

    def __update (self, e):
        refresh = False
        nv = e.data
        for k in nv.keys():
            if nv[k] is None:
                self.__logger.warning ("Received key '" + k +
                                       "' with None as its content")
                continue
            
            try:
                self.__getattribute__ ("_" + k).SetLabel (str(nv[k]))
                refresh = True
                pass
            except AttributeError as ae:
                #self.__logger.warning ("Error converting keys to labels: " + str(ae))
                pass
            pass

        if refresh:
            self.Layout()
            self.Refresh()
            pass
        
        return refresh
    
    pass

if __name__ == "__main__":
    app = wx.PySimpleApp()
    frame = wx.Frame(None, -1, "*** Test View of Status Panel ***")
    panel = StatusView (None,parent=frame)
    frame.Fit()
    frame.Show(True)
    app.MainLoop()
    pass
