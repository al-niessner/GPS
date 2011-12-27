
def load (fn):
    """Load the config file.
    """
    global _current, _fn
    g = {}
    execfile (fn, g)
    _current =  g['config']
    _fn = fn
    return

def readGPS ():
    """Read the GPS options from the loaded file.

    The GPS configuration options are a dictionary. The configuration file is
    read and its dictionary contents are returned.
    """
    return _current['device']

def readWindow():
    """Read the Window options from a file.

    The window configuration options are a dictionary. The configuration file is
    read and its dictionary contents are returned.
    """
    return _current['window']

def saveGPS (opts, fn=None):
    """Write the GPS options into a file.

    The GPS configuration options are a dictionary. The dictionary is written
    to disk in a form that can be simply executed by a python interpretter.

    opts - the current options to be saved.
    fn   - the file name to write the option to.
    """
    _current['device'] = opts.copy()
    write (fn)
    return

def saveWindow (opts, fn=None):
    """Write the Window options into a file.

    The window configuration options are a dictionary. The dictionary is written
    to disk in a form that can be simply executed by a python interpretter.

    opts - the current options to be saved.
    fn   - the file name to write the option to.
    """
    _current['window'] = opts.copy()
    write (fn)
    return

def write (fn=None):
    """Write the configuation to the file.
    """
    if fn is None: fn = _fn
    f = open (fn, 'w')
    f.write ("config=" + str (_current) + "\n")
    f.close()
    return
