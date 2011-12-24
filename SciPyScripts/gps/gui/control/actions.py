#! /usr/bin/env python

import wx

class ActionsControl(wx.Panel):
    def __init__(self, *args, **kwds):
        # Initial connection to the rest of the world
        kwds["style"] = wx.TAB_TRAVERSAL
        wx.Panel.__init__(self, *args, **kwds)

        # Instantiate the elements contained in the GUI
        self.bs1_staticbox = wx.StaticBox(self, -1, "Goto State")
        self.newTrack = wx.Button(self, -1, "New Track")
        self.markWaypoint = wx.Button(self, -1, "Mark Waypoint")
        self.resetTrip = wx.Button(self, -1, "Reset Trip")
        self.connect = wx.Button(self, -1, "Connect")
        self.goto = wx.Button(self, -1, "Go To")
        self.backTrack = wx.Button(self, -1, "Back Track")
        self.s1 = wx.Button(self, -1, "S1")
        self.s2 = wx.Button(self, -1, "S2")
        self.s3 = wx.Button(self, -1, "S3")
        self.s4 = wx.Button(self, -1, "S4")
        self.s5 = wx.Button(self, -1, "S5")
        self.s6 = wx.Button(self, -1, "S6")
        self.s7 = wx.Button(self, -1, "S7")
        self.s8 = wx.Button(self, -1, "S8")
        self.s9 = wx.Button(self, -1, "S9")
        self.s0 = wx.Button(self, -1, "S0")
        self.__unused1 = wx.Button(self, -1, "")
        self.__unused2 = wx.Button(self, -1, "")

        self.__unused1.Enable (False)
        self.__unused2.Enable (False)
        
        self.__do_layout()
        return

    def __do_layout(self):
        gs1 = wx.FlexGridSizer(7, 1, 5, 5)
        bs1 = wx.StaticBoxSizer(self.bs1_staticbox, wx.VERTICAL)
        gs2 = wx.GridSizer(4, 3, 2, 2)
        gs1.Add (self.newTrack, 0, wx.ALIGN_CENTER_HORIZONTAL, 0)
        gs1.Add (self.markWaypoint, 0, wx.ALIGN_CENTER_HORIZONTAL, 0)
        gs1.Add (self.resetTrip, 0, wx.ALIGN_CENTER_HORIZONTAL, 0)
        gs1.Add (self.connect, 0, wx.ALIGN_CENTER_HORIZONTAL, 0)
        gs1.Add (self.goto, 0, wx.ALIGN_CENTER_HORIZONTAL, 0)
        gs1.Add (self.backTrack, 0, wx.ALIGN_CENTER_HORIZONTAL, 0)
        gs2.Add (self.s1, 0, wx.EXPAND, 0)
        gs2.Add (self.s2, 0, wx.EXPAND, 0)
        gs2.Add (self.s3, 0, wx.EXPAND, 0)
        gs2.Add (self.s4, 0, wx.EXPAND, 0)
        gs2.Add (self.s5, 0, wx.EXPAND, 0)
        gs2.Add (self.s6, 0, wx.EXPAND, 0)
        gs2.Add (self.s7, 0, wx.EXPAND, 0)
        gs2.Add (self.s8, 0, wx.EXPAND, 0)
        gs2.Add (self.s9, 0, wx.EXPAND, 0)
        gs2.Add (self.__unused1, 0, wx.EXPAND, 0)
        gs2.Add (self.s0, 0, wx.EXPAND, 0)
        gs2.Add (self.__unused2, 0, wx.EXPAND, 0)
        bs1.Add (gs2, 1, wx.EXPAND, 0)
        gs1.Add (bs1, 1, wx.EXPAND, 0)
        self.SetSizer (gs1)
        gs1.Fit (self)
        gs1.AddGrowableRow (6)
        gs1.AddGrowableCol (0)
        gs1.AddGrowableCol (1)
        gs1.AddGrowableCol (2)
        gs1.AddGrowableCol (3)
        gs1.AddGrowableCol (4)
        gs1.AddGrowableCol (5)
        gs1.AddGrowableCol (6)
        return

    pass

if __name__ == "__main__":
    app = wx.PySimpleApp()
    frame = wx.Frame(None, -1, "*** Test View of Status Panel ***")
    panel = ActionsControl (parent=frame)
    frame.Fit()
    frame.Show(True)
    app.MainLoop()
    pass
