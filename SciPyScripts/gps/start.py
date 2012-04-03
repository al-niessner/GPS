#! /usr/bin/env python

from device import Serial
from device import Simulator
from gui import ConfigurationModel
from gui import MainDisplay
from gui import UnitModel
from gui import ViewModel
from util import Bus

import argparse
import gps.device
import gps.fileio.config
import logging
import os
import sys
import threading
import wx

def __makeThread (target, name):
    t = threading.Thread (target=target, name=name)
    t.daemon = True
    t.start()
    pass

def _getserial (config, busses, logger):
    return Serial(busses, logger,
                  config['serial']['devname'],
                  config['serial']['baudrate'])

def _getsimulator (config, busses, logger):
    return Simulator(busses, logger, config['simulation'])

def adjustConfig (args, config):
    if args.log_file is not None:
        config['device']['files']['log'] = args.log_file
        pass
    
    if args.output_dir is not None:
        config['device']['files']['outdir'] = args.output_dir
        pass
    
    if args.use_transport is not None:
        config['device']['transport'] = args.use_transport
        pass
    
    if args.serial_port is not None:
        config['device']['serial']['devname'] = args.serial_port
        pass
    
    if args.serial_rate is not None:
        config['device']['serial']['baudrate'] = args.serial_rate
        pass
    
    if args.simulation_file is not None:
        config['device']['simulation'] = args.simulation_file
        pass

    if config['device']['transport'] == 'simulator' and \
           config['device']['simulation'] is None:
        print "Hey! You asked for a simulation and you did not tell me where to get the data from."
        sys.exit()
        pass

    if args.window_compass is None:
        b = config.get ('window', {}).get ('auto', {}).get ('compass', True)
        args.window_compass = b
        pass
    
    if args.window_map is None:
        b = config.get ('window', {}).get ('auto', {}).get ('map', True)
        args.window_map = b
        pass
    
    if args.window_status is None:
        b = config.get ('window', {}).get ('auto', {}).get ('status', True)
        args.window_status = b
        pass
    
    return

def execute (config, compass, map, status):
    import gps.start

    # Get a device
    busses = { 'fromConfig':Bus(),
               'fromHardware':Bus(), 'toHardware':Bus() }
    device = "_get" + config['device']['transport']
    device = gps.start.__getattribute__ (device)\
             (config['device'], busses, logging.getLogger ("gps.device"))
    __makeThread (device.read, "Device Reader")
    __makeThread (device.write, "Device Writer")
    
    # Make the models
    ml = logging.getLogger ("gps.gui.model")
    models = {
        'config':ConfigurationModel(busses['fromConfig'], config, ml),
        'unit':UnitModel(),
        'view':ViewModel(busses['fromHardware'], ml),
        }
    __makeThread (models['view'].run, "GUI View Model")
    
    if status:
        app = wx.PySimpleApp(0)
        wx.InitAllImageHandlers()
        mainFrame = MainDisplay(models, logging.getLogger ("gps.gui.display"),
                                None, -1, "")
        app.SetTopWindow(mainFrame)
        mainFrame.Show()
        
        # Start all of the subsystems and then the main display
        app.MainLoop()
        pass
    
    return

def boolean (v):
    result = None

    if v is not None:
        result = False
        v = v.lower()
        for b in ['t', 'tr', 'tru', 'true', '1', 'y', 'ye', 'yes' ]:
            if b == v: result = True
            pass
        pass
    
    return result

if __name__ == "__main__":
    home = os.environ['HOME']
    parser = argparse.ArgumentParser(description="Run the GPS application.")

    # general output file options
    parser.add_argument ("--config-file", "-c",
                         default=os.path.join (home, ".gps.config"),
                         help="Where to get the configuration parameters from. [%(default)s]")
    parser.add_argument ("--log-file", "-l",
                         help="The file to put log messages into.")
    parser.add_argument ("--output-dir", "-O",
                         help="The directory to put all of the data file into.")

    # General selections
    parser.add_argument ("--use-transport", "-t",
                         choices={'serial', 'simulator', 'usb'},
                         help="Which of the possible transports to use.")
    # serial port options
    parser.add_argument ("--serial-port", "-S",
                         help="Serial port that the GPS unit is attached to.")
    parser.add_argument ("--serial-rate", "-B",
                         help="Serial port data rate.")
    # simulator options
    parser.add_argument ("--simulation-file", "-i",
                         help="Simulation file to read the data from.")
    # window options
    parser.add_argument ("--window-compass", "-C", type=boolean, default=None,
                         help="Force the compass window being present or not")
    parser.add_argument ("--window-map", "-M", type=boolean, default=None,
                         help="Force the map window being present or not")
    parser.add_argument ("--window-status", "-W", type=boolean, default=None,
                         help="Force the status window being present or not")
    
    # get the arguments and then override the config file parameters
    args = parser.parse_args()
    config = gps.fileio.config.load (args.config_file)
    adjustConfig (args, config)
    logging.basicConfig (filename=config['device']['file']['log'],
                         level=logging.INFO)
    execute (config, args.window_compass, args.window_map, args.window_status)
    pass
