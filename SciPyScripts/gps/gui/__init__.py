
from main  import MainDisplay
from model import ActionModel
from model import ConfigurationModel
from model import ViewModel

import wx

EVT_NEW_DATA_TYPE = wx.NewEventType()
EVT_NEW_DATA = wx.PyEventBinder (EVT_NEW_DATA_TYPE)

class NewDataEvent(wx.PyEvent):
    def __init__ (self, winid=0, et=EVT_NEW_DATA_TYPE, data={}):
        wx.PyEvent.__init__ (self, winid, et)
        self.data = data
        pass
    pass
