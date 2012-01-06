#!/usr/bin/env python

import gps.gui
import os
import wx

class ConfigurationControl(wx.Panel):
    def __init__(self, logger, model, *args, **kwds):
        kwds["style"] = wx.TAB_TRAVERSAL
        wx.Panel.__init__(self, *args, **kwds)
        
        self.__logger = logger
        self.__model = model
        
        self.__notebook = wx.Notebook(self, -1, style=0)
        self.__device = wx.Panel(self.__notebook, -1)
        self.sb1 = wx.StaticBox(self.__device, -1, "Serial")
        self.sb2 = wx.StaticBox(self.__device, -1, "Simulator")
        self.sb3 = wx.StaticBox(self.__device, -1, "USB")
        self._units = wx.RadioBox(self.__device, -1, "Units",
                                  choices=["GPS Native", "Metric", "Royal"],
                                  majorDimension=0, style=wx.RA_SPECIFY_ROWS)
        self._connection = wx.RadioBox(self.__device, -1, "Connection Type",
                                       choices=["Disconnect", "Serial",
                                                "Simulation", "USB"],
                                       majorDimension=0,
                                       style=wx.RA_SPECIFY_ROWS)
        self._timezone = wx.RadioBox(self.__device, -1, "Time Zone",
                                     choices=["GPS Native", "Local", "UTC"],
                                     majorDimension=0, style=wx.RA_SPECIFY_ROWS)
        self.__lblSerialDev = wx.Button(self.__device, -1, "device name")
        self._devName = wx.TextCtrl(self.__device, -1, "/dev/ttyUSB0")
        self.__lblSerialRate = wx.StaticText(self.__device, -1, "rate")
        self._serialRate = wx.ComboBox(self.__device, -1,
                                       choices=["1200", "2400", "4800", "9600",
                                                "19200", "38400", "57600",
                                                "115200"],
                                       style=wx.CB_DROPDOWN|wx.CB_DROPDOWN|wx.CB_SORT)
        self.__lblSimulator = wx.Button(self.__device, -1, "file")
        self._sim = wx.TextCtrl(self.__device, -1, "~/sim.dat")
        self._compass = wx.RadioBox(self.__device, -1, "Heading",
                                    choices=["GPS Native",
                                             "0..360", "-180..180"],
                                    majorDimension=0, style=wx.RA_SPECIFY_ROWS)
        self.__lblusbMan = wx.StaticText(self.__device, -1, "Manufacture ID")
        self._usbMan = wx.SpinCtrl(self.__device, -1, "0", min=0, max=65535)
        self.__lblusbProd = wx.StaticText(self.__device, -1, "Product ID")
        self._usbProd = wx.SpinCtrl(self.__device, -1, "", min=0, max=65535)
        self.__lblusbPoll = wx.StaticText(self.__device, -1, "Poll Rate (ms)")
        self._usbPoll = wx.SpinCtrl(self.__device, -1, "1000",
                                    min=100, max=10000)
        self.__window = wx.Panel(self.__notebook, -1)
        self.__assb = wx.StaticBox (self.__window, -1, "Auto Start")
        self._autoCompass = wx.CheckBox(self.__window, -1, "Compass View")
        self._autoMap = wx.CheckBox(self.__window, -1, "Map View")
        self._autoStatus = wx.CheckBox(self.__window, -1, "Main View")
        self._lock = wx.Button(self, -1, "Unlock")

        self.__set_properties()
        self.__do_layout()
        self.__register_events()
        return

    def __do_layout(self):
        # do the device configuration
        fgs1 = wx.FlexGridSizer(2, 1, 5, 5)
        fgs2 = wx.FlexGridSizer(2, 2, 3, 3)
        gbs = wx.GridBagSizer (5,5)
        gs1 = wx.GridSizer(1, 2, 0, 0)
        gs2 = wx.GridSizer(3, 2, 0, 0)
        sbs1 = wx.StaticBoxSizer(self.sb1, wx.HORIZONTAL)
        sbs2 = wx.StaticBoxSizer(self.sb2, wx.HORIZONTAL)
        sbs3 = wx.StaticBoxSizer(self.sb3, wx.HORIZONTAL)
        gbs.Add (self._units, wx.GBPosition (0,0), wx.GBSpan (3,1))
        gbs.Add (self._connection, wx.GBPosition (1,1), wx.GBSpan (4,1))
        gbs.Add (self._timezone, wx.GBPosition (3,0), wx.GBSpan (3,1))
        fgs2.Add (self.__lblSerialDev, 0, wx.ALIGN_CENTER_VERTICAL, 0)
        fgs2.Add (self._devName, 0, wx.EXPAND, 0)
        fgs2.Add (self.__lblSerialRate, 0, wx.ALIGN_CENTER_VERTICAL, 0)
        fgs2.Add (self._serialRate, 0, wx.EXPAND, 0)
        sbs1.Add (fgs2, 1, wx.EXPAND, 0)
        gbs.Add (sbs1, wx.GBPosition (2,2), wx.GBSpan (2,1), wx.EXPAND)
        gbs.Add (self._compass, wx.GBPosition (6,0), wx.GBSpan (3,1))
        gs1.Add (self.__lblSimulator, 0, wx.ALIGN_CENTER_VERTICAL, 0)
        gs1.Add (self._sim, 0, wx.EXPAND, 0)
        sbs2.Add (gs1, 1, wx.EXPAND, 0)
        gbs.Add (sbs2, wx.GBPosition (4,2), wx.GBSpan (1,1), wx.EXPAND)
        gs2.Add (self.__lblusbMan, 0, wx.ALIGN_CENTER_VERTICAL, 0)
        gs2.Add (self._usbMan, 0, wx.EXPAND, 0)
        gs2.Add (self.__lblusbProd, 0, wx.ALIGN_CENTER_VERTICAL, 0)
        gs2.Add (self._usbProd, 0, wx.EXPAND, 0)
        gs2.Add (self.__lblusbPoll, 0, wx.ALIGN_CENTER_VERTICAL, 0)
        gs2.Add (self._usbPoll, 0, wx.EXPAND, 0)
        sbs3.Add (gs2, 1, wx.EXPAND, 0)
        gbs.Add (sbs3, wx.GBPosition (5,2), wx.GBSpan (3,1), wx.EXPAND)
        self.__device.SetSizer (gbs)
        # do the window configuration
        gs3 = wx.GridSizer (1,1,10,10)
        gs4 = wx.GridSizer (3,1,10,10)
        sbs4 = wx.StaticBoxSizer (self.__assb, wx.HORIZONTAL)
        gs4.Add (self._autoCompass, flag=wx.EXPAND)
        gs4.Add (self._autoMap, flag=wx.EXPAND)
        gs4.Add (self._autoStatus, flag=wx.EXPAND)
        sbs4.Add (gs4, 1, wx.EXPAND)
        gs3.Add (sbs4, 0, wx.EXPAND)
        self.__window.SetSizer (gs3)
        # do the full window
        self.__notebook.AddPage (self.__device, "Device")
        self.__notebook.AddPage (self.__window, "Window")
        fgs1.Add (self.__notebook, 1, wx.EXPAND, 0)
        fgs1.Add (self._lock, 0, wx.ALIGN_RIGHT, 0)
        self.SetSizer (fgs1)
        fgs1.Fit (self)
        fgs1.AddGrowableRow (0)
        fgs1.AddGrowableCol (0)
        return

    def __register_events(self):
        self.Bind (gps.gui.EVT_NEW_DATA, self.__update)
        self.__lblSerialDev.Bind (wx.EVT_BUTTON, self.dev_select)
        self.__lblSimulator.Bind (wx.EVT_BUTTON, self.file_select)
        self._autoCompass.Bind (wx.EVT_CHECKBOX, self.auto)
        self._autoMap.Bind (wx.EVT_CHECKBOX, self.auto)
        self._autoStatus.Bind (wx.EVT_CHECKBOX, self.auto)
        self._compass.Bind (wx.EVT_RADIOBOX, self.display)
        self._connection.Bind (wx.EVT_RADIOBOX, self.connection)
        self._devName.Bind (wx.EVT_TEXT, self.serial)
        self._lock.Bind (wx.EVT_BUTTON, self.toggle_lock)
        self._serialRate.Bind (wx.EVT_TEXT, self.serial)
        self._sim.Bind (wx.EVT_TEXT, self.sim)
        self._timezone.Bind (wx.EVT_RADIOBOX, self.display)
        self._units.Bind (wx.EVT_RADIOBOX, self.display)
        self._usbMan.Bind (wx.EVT_TEXT, self.usb)
        self._usbProd.Bind (wx.EVT_TEXT, self.usb)
        self._usbPoll.Bind (wx.EVT_TEXT, self.usb)
        return
    
    def __set_properties(self):
        self._units.SetSelection (0)
        self._connection.SetSelection (0)
        self._timezone.SetSelection (0)
        self._serialRate.SetSelection (6)
        self._compass.SetSelection (0)
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
                if k == 'compass' or \
                   k == 'connection' or \
                   k == 'timezone' or \
                   k == 'units' or \
                   k == 'usb':
                    self.__getattribute__ ("_" + k).SetSelection (int(nv[k]))
                    pass
                elif k.startswith ("auto"):
                    self.__getattribute__ ("_" + k).SetValue (bool(nv[k]))
                    pass
                else: self.__getattribute__ ("_" + k).ChangeValue (str(nv[k]))
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

    def auto (self, event):
        self.__model.set_auto (compass=self._autoCompass.GetValue(),
                               map=self._autoMap.GetValue(),
                               status=self._autoStatus.GetValue())
        return
    
    def connection (self, event):
        self.__model.set_con (self._connection.GetSelection())
        return
    
    def dev_select (self, event):
        dlg = wx.FileDialog(self,
                            "Choose a simulation data file",
                            os.path.dirname (self._devName.GetValue()),
                            os.path.basename (self._devName.GetValue()),
                            "*.*",
                            wx.OPEN)
            
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            self._sim.SetValue (path)
            pass
        
        dlg.Destroy()
        return

    def display (self, event):
        self.__model.set_display (self._compass.GetSelection(),
                                  self._timezone.GetSelection(),
                                  self._units.GetSelection())
        return
    
    def file_select (self, event):
        dlg = wx.FileDialog(self,
                            "Choose a simulation data file",
                            os.path.dirname (self._sim.GetValue()),
                            os.path.basename (self._sim.GetValue()),
                            "*.nmea",
                            wx.OPEN)
            
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            self._sim.SetValue (path)
            pass
        
        dlg.Destroy()
        return

    def serial (self, event):
        self.__model.set_serial (self._devName.GetValue(),
                                 int (self._serialRate.GetValue()))
        return

    def sim (self, event):
        self.__model.set_sim (self._sim.GetValue())
        return

    def usb (self, event):
        self.__model.set_usb (self._usbMan.GetValue(),
                              self._usbProd.GetValue(),
                              self._usbPoll.GetValue())
        return
    
    def toggle_lock (self, event):
        print "Toggle the lock"
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
