#!/usr/bin/env python

from control import ActionsControl
from control import ConfigurationControl
from model import ActionModel
from model import ViewModel
from view import NewDataEvent
from view import StatusView

import wx

class MainDisplay(wx.Frame):
    def __init__(self, models, logger, *args, **kwds):
        kwds["style"] = wx.DEFAULT_FRAME_STYLE
        wx.Frame.__init__(self, *args, **kwds)

        self.__horizSplit = wx.SplitterWindow(self, -1, style=wx.SP_3D|wx.SP_BORDER)
        self.__configPane = wx.Panel(self.__horizSplit, -1)
        self.__splitPane = wx.Panel(self.__horizSplit, -1)
        self.__vertSplit = wx.SplitterWindow(self.__splitPane, -1, style=wx.SP_3D|wx.SP_BORDER)
        self.__actionPane = wx.Panel(self.__vertSplit, -1)
        self.__statusPane = wx.Panel(self.__vertSplit, -1)
        self.sbs1 = wx.StaticBox(self.__statusPane, -1, "Status")
        self.sbs2 = wx.StaticBox(self.__actionPane, -1, "Actions")
        self.sbs3 = wx.StaticBox(self.__configPane, -1, "Configuration")
        self.__action = ActionsControl(self.__actionPane, -1)
        self.__config = ConfigurationControl(self.__configPane, -1)
        self.__status = StatusView(logger, self.__statusPane, -1)

        models['view'].register (self)
        self.SetTitle("Main Display")

        self.__do_layout()
        return
    
    def __do_layout(self):
        bs1 = wx.BoxSizer(wx.VERTICAL)
        sbs3 = wx.StaticBoxSizer(self.sbs3, wx.HORIZONTAL)
        bs2 = wx.BoxSizer(wx.HORIZONTAL)
        sbs2 = wx.StaticBoxSizer(self.sbs2, wx.HORIZONTAL)
        sbs1 = wx.StaticBoxSizer(self.sbs1, wx.HORIZONTAL)
        sbs1.Add(self.__status, 1, wx.EXPAND, 0)
        self.__statusPane.SetSizer(sbs1)
        sbs2.Add(self.__action, 1, wx.EXPAND, 0)
        self.__actionPane.SetSizer(sbs2)
        self.__vertSplit.SplitVertically(self.__statusPane, self.__actionPane)
        bs2.Add(self.__vertSplit, 1, wx.EXPAND, 0)
        self.__splitPane.SetSizer(bs2)
        sbs3.Add(self.__config, 1, wx.EXPAND, 0)
        self.__configPane.SetSizer(sbs3)
        self.__horizSplit.SplitHorizontally(self.__splitPane, self.__configPane)
        bs1.Add(self.__horizSplit, 1, wx.EXPAND, 0)
        self.SetSizer(bs1)
        bs1.Fit(self)
        self.Layout()
        return

    def update (self, data):
        e = NewDataEvent (winid=self.__status.Id, data=data)
        wx.PostEvent (self.__status.GetEventHandler(), e)
        return
    
    pass

if __name__ == "__main__":
    app = wx.PySimpleApp(0)
    wx.InitAllImageHandlers()
    models = { 'actions':ActionModel(),
               'view':ViewModel(None, None) }
    mainFrame = MainDisplay(models, None, -1, "")
    app.SetTopWindow(mainFrame)
    mainFrame.Show()
    app.MainLoop()
    pass
