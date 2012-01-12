#!/usr/bin/env python

from control import ActionsControl
from control import ConfigurationControl
from model import ActionModel
from model import ConfigurationModel
from model import ViewModel
from view import StatusView

import gps.gui
import wx

class MainDisplay(wx.Frame):
    def __init__(self, models, logger, *args, **kwds):
        kwds["style"] = wx.DEFAULT_FRAME_STYLE
        wx.Frame.__init__(self, *args, **kwds)

        self.__horizSplit = wx.SplitterWindow(self, -1,
                                              style=wx.SP_3D|wx.SP_BORDER)
        self.__horizSplit.SetMinimumPaneSize (10)
        self.__configPane = wx.Panel(self.__horizSplit, -1)
        self.__splitPane = wx.Panel(self.__horizSplit, -1)
        self.__vertSplit = wx.SplitterWindow(self.__splitPane, -1,
                                             style=wx.SP_3D|wx.SP_BORDER)
        self.__vertSplit.SetMinimumPaneSize (10)
        self.__actionPane = wx.Panel(self.__vertSplit, -1)
        self.__statusPane = wx.Panel(self.__vertSplit, -1)
        self.sbs1 = wx.StaticBox(self.__statusPane, -1, "Status")
        self.sbs2 = wx.StaticBox(self.__actionPane, -1, "Actions")
        self.sbs3 = wx.StaticBox(self.__configPane, -1, "Configuration")
        self.__action = ActionsControl(self.__actionPane, -1)
        self.__config = ConfigurationControl(logger, models['config'],
                                             self.__configPane, -1)
        self.__status = StatusView(logger, self.__statusPane, -1)
        self.__model = models['config']
        self.__units = models['unit']
        
        models['config'].register (self)
        models['view'].register (self)
        self.SetTitle("Main Display")

        self.__do_layout()
        self.__do_config (models)
        self.__do_events()
        return

    def __do_config (self, models):
        if models['config'].get_pos ('main') is not None:
            self.SetPosition (wx.Point (**models['config'].get_pos ('main')))
            pass

        if models['config'].get_pos ('main_horz') is not None:
            pos = models['config'].get_pos ('main_horz')
            self.__horizSplit.SetSashPosition (pos, True)
            pass

        if models['config'].get_pos ('main_vert') is not None:
            pos = models['config'].get_pos ('main_vert')
            self.__vertSplit.SetSashPosition (pos, True)
            pass

        if models['config'].get_size ('main') is not None:
            self.SetSize (wx.Size (**models['config'].get_size ('main')))
            pass

        self.Layout()
        self.Refresh()
        return

    def __do_events (self):
        self.__horizSplit.Bind (wx.EVT_SPLITTER_SASH_POS_CHANGED, self.__horz)
        self.__vertSplit.Bind (wx.EVT_SPLITTER_SASH_POS_CHANGED, self.__vert)
        self.Bind (wx.EVT_SIZE, self.__resize)
        self.Bind (wx.EVT_MOVE, self.__move)
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

    def __horz (self, e):
        self.__model.set_position ("main_horz", e.GetSashPosition())
        return

    def __move (self, e):
        pos = e.GetPosition()
        self.__model.set_position ("main", {'x':pos.x, 'y':pos.y})
        return

    def __resize (self, e):
        size = e.GetSize()
        self.__model.set_size ("main", {'h':size.y, 'w':size.x})
        self.Layout()
        self.Refresh()
        return

    def __vert (self, e):
        self.__model.set_position ("main_vert", e.GetSashPosition())
        return
    
    def update (self, data):
        data = self.__unit.convert (data, self.__model.get_units())
        e = gps.gui.NewDataEvent (winid=self.__status.Id, data=data)
        wx.PostEvent (self.__config.GetEventHandler(), e)
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
