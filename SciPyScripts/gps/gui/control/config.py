#!/usr/bin/env python

import wx

class ConfigurationControl(wx.Panel):
    def __init__(self, *args, **kwds):
        kwds["style"] = wx.TAB_TRAVERSAL
        wx.Panel.__init__(self, *args, **kwds)
        
        self.__notebook = wx.Notebook(self, -1, style=0)
        self.__device = wx.Panel(self.__notebook, -1)
        self.sb2 = wx.StaticBox(self.__device, -1, "USB")
        self.sb1 = wx.StaticBox(self.__device, -1, "Serial")
        self.units = wx.RadioBox(self.__device, -1, "Units",
                                 choices=["Metric", "Royal"],
                                 majorDimension=0, style=wx.RA_SPECIFY_ROWS)
        self.connection = wx.RadioBox(self.__device, -1, "Connection Type",
                                      choices=["Serial", "Simulation", "USB"],
                                      majorDimension=0, style=wx.RA_SPECIFY_ROWS)
        self.timezone = wx.RadioBox(self.__device, -1, "Time Zone",
                                    choices=["Local", "UTC"],
                                    majorDimension=0, style=wx.RA_SPECIFY_ROWS)
        self.__lblSerialDev = wx.StaticText(self.__device, -1, "device name")
        self.devName = wx.TextCtrl(self.__device, -1, "/dev/ttyUSB0")
        self.__lblSerialRate = wx.StaticText(self.__device, -1, "rate")
        self.serialRate = wx.ComboBox(self.__device, -1,
                                      choices=["1200", "2400", "4800", "9600",
                                               "19200", "38400", "57600",
                                               "115200"],
                                      style=wx.CB_DROPDOWN|wx.CB_DROPDOWN|wx.CB_SORT)
        self.heading = wx.RadioBox(self.__device, -1, "Heading",
                                   choices=["0..360", "-180..180"],
                                   majorDimension=0, style=wx.RA_SPECIFY_ROWS)
        self.__lblusbMan = wx.StaticText(self.__device, -1, "Manufacture ID")
        self.usbMan = wx.SpinCtrl(self.__device, -1, "0", min=0, max=65535)
        self.__lblusbProd = wx.StaticText(self.__device, -1, "Product ID")
        self.usbProd = wx.SpinCtrl(self.__device, -1, "", min=0, max=65535)
        self.__lblusbPoll = wx.StaticText(self.__device, -1, "Poll Rate (ms)")
        self.usbPoll = wx.SpinCtrl(self.__device, -1, "1000",
                                   min=100, max=10000)
        self.__window = wx.Panel(self.__notebook, -1)
        self.lock = wx.Button(self, -1, "Unlock")

        self.__set_properties()
        self.__do_layout()
        return

    def __set_properties(self):
        self.units.SetSelection(0)
        self.connection.SetSelection(0)
        self.timezone.SetSelection(1)
        self.serialRate.SetSelection(6)
        self.heading.SetSelection(0)
        return

    def __do_layout(self):
        gs1 = wx.FlexGridSizer(2, 1, 5, 5)
        gs2 = wx.GridSizer(3, 2, 5, 5)
        sbs2 = wx.StaticBoxSizer(self.sb2, wx.HORIZONTAL)
        gs4 = wx.GridSizer(3, 2, 0, 0)
        sbs1 = wx.StaticBoxSizer(self.sb1, wx.HORIZONTAL)
        gs3 = wx.FlexGridSizer(2, 2, 3, 3)
        gs2.Add(self.units, 0, 0, 0)
        gs2.Add(self.connection, 0, 0, 0)
        gs2.Add(self.timezone, 0, 0, 0)
        gs3.Add(self.__lblSerialDev, 0, wx.ALIGN_CENTER_VERTICAL, 0)
        gs3.Add(self.devName, 0, wx.EXPAND, 0)
        gs3.Add(self.__lblSerialRate, 0, wx.ALIGN_CENTER_VERTICAL, 0)
        gs3.Add(self.serialRate, 0, wx.EXPAND, 0)
        sbs1.Add(gs3, 1, wx.EXPAND, 0)
        gs2.Add(sbs1, 1, wx.EXPAND, 0)
        gs2.Add(self.heading, 0, 0, 0)
        gs4.Add(self.__lblusbMan, 0, wx.ALIGN_CENTER_VERTICAL, 0)
        gs4.Add(self.usbMan, 0, wx.EXPAND, 0)
        gs4.Add(self.__lblusbProd, 0, wx.ALIGN_CENTER_VERTICAL, 0)
        gs4.Add(self.usbProd, 0, wx.EXPAND, 0)
        gs4.Add(self.__lblusbPoll, 0, wx.ALIGN_CENTER_VERTICAL, 0)
        gs4.Add(self.usbPoll, 0, wx.EXPAND, 0)
        sbs2.Add(gs4, 1, wx.EXPAND, 0)
        gs2.Add(sbs2, 1, wx.EXPAND, 0)
        self.__device.SetSizer(gs2)
        self.__notebook.AddPage(self.__device, "Device")
        self.__notebook.AddPage(self.__window, "Window")
        gs1.Add(self.__notebook, 1, wx.EXPAND, 0)
        gs1.Add(self.lock, 0, wx.ALIGN_RIGHT, 0)
        self.SetSizer(gs1)
        gs1.Fit(self)
        gs1.AddGrowableRow(0)
        gs1.AddGrowableCol(0)
        return

    pass

if __name__ == "__main__":
    app = wx.PySimpleApp()
    frame = wx.Frame(None, -1, "*** Test View of Status Panel ***")
    panel = ConfigurationControl (parent=frame)
    frame.Fit()
    frame.Show(True)
    app.MainLoop()
    pass
