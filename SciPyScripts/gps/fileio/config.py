
def readGPS (fn):
    """Read the GPS options from a file.

    The GPS configuration options are a dictionary. The configuration file is
    read and its dictionary contents are returned.

    fn - the filename to read from
    """
    g = {}
    execfile (fn, g)
    return g['config']

def readWindow():
    """Read the Window options from a file.

    The window configuration options are a dictionary. The configuration file is
    read and its dictionary contents are returned.
    """
    return

def saveGPS (fn, opts):
    """Write the GPS options into a file.

    The GPS configuration options are a dictionary. The dictionary is written
    to disk in a form that can be simply executed by a python interpretter.

    fn   - the file name to write the option to.
    opts - the current options to be saved.
    """
    f = file(fn, "w")
    f.write ("config=" + str(opts) + "\n")
    f.close()
    return

def saveWindow (opts):
    """Write the Window options into a file.

    The window configuration options are a dictionary. The dictionary is written
    to disk in a form that can be simply executed by a python interpretter.

    opts - the current options to be saved.
    """
    return
