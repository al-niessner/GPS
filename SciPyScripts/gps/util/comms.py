"""The communication mechanism between all of the modules.

Various modules must communicate between and amongst themselves. This module
contains the Bus class that allows the various modules to communicate and to
execute in their own threads.

A Bus supports a single set of messages and is broadcast only. The Bus was
made a class to allow for various instantiations allowing for a richer set of
relationships amongst the various subsystems.
"""

import Queue
import threading

class Bus(object):
    """The class that offers bus like communications among subsystems.
    """
    def __init__ (self):
        object.__init__ (self)
        self.__lock = threading.RLock()
        self.__queues = []
        return

    def broadcast (self, message):
        """Send a message to all bus listeners.
        """
        self.__lock.acquire()
        for q in self.__queues: q.put (message, False)
        self.__lock.release()
        return

    def register (self):
        """Register with the bus to get messages.
        """
        q = Queue.Queue()
        self.__lock.acquire()
        self.__queues.append (q)
        self.__lock.release()
        return q

    def unregister (self, q):
        """Unregister with the bus to quit receiving messages.
        """
        self.__lock.acquire()
        if q in self.__queues: self.__queues.remove (q)
        else: self.__logger.warning ("Bus listener tried to unregister a queue that was never registered in the first place.")
        self.__lock.release()
        return

    pass
