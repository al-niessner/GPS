#! /usr/bin/env python

from device import Serial
from device import Simulator
from gui import MainDisplay
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
    if args.log_file is not None: config['files']['log'] = args.log_file
    if args.output_dir is not None: config['files']['outdir'] = args.output_dir
    if args.use_transport is not None: config['transport'] = args.use_transport
    if args.serial_port is not None: config['serial']['devname'] = args.serial_port
    if args.serial_rate is not None: config['serial']['baudrate'] = args.serial_rate
    if args.simulation_file is not None: config['simulation'] = args.simulation_file

    if config['transport'] == 'simulator' and config['simulation'] is None:
        print "Hey! You asked for a simulation and you did not tell me where to get the data from."
        sys.exit()
        pass
    return

def execute (config):
    import gps.start

    # Get a device
    busses = { 'fromHardware':Bus(), 'toHardware':Bus() }
    device = "_get" + config['transport']
    device = gps.start.__getattribute__ (device)\
             (config, busses, logging.getLogger ("gps.device"))
    __makeThread (device.read, "Device Reader")
    __makeThread (device.write, "Device Writer")
    
    # Make the models
    models = {
        'view':ViewModel (busses['fromHardware'],
                          logging.getLogger ("gps.gui.model")),
        }
    __makeThread (models['view'].run, "GUI View Model")
    
    # Get a GUI
    app = wx.PySimpleApp(0)
    wx.InitAllImageHandlers()
    mainFrame = MainDisplay(models, logging.getLogger ("gps.gui.display"),
                            None, -1, "")
    app.SetTopWindow(mainFrame)
    mainFrame.Show()

    # Start all of the subsystems and then the main display
    app.MainLoop()
    
    return

if __name__ == "__main__":
    home = os.environ['HOME']
    parser = argparse.ArgumentParser(description="Run the GPS application.")

    # general output file options
    parser.add_argument ("--config-file", "-c",
                         default=os.path.join (home, ".gps.config"),
                         help="Where to get the configuration parameters from.")
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

    # get the arguments and then override the config file parameters
    args = parser.parse_args()
    config = gps.fileio.config.readGPS (args.config_file)
    adjustConfig (args, config)
    logging.basicConfig (filename=config['file']['log'],
                         level=logging.INFO)
    execute (config)
    pass
