from threading import Thread
from Queue import Queue, Empty

from time import sleep

class NBSR:

    def __init__(self, stream):
        '''
        stream: the stream to read from.
                Usually a process' stdout or stderr.
        '''

        self._s = stream
        self._q = Queue()
        self.bad = False;

        def _populateQueue(stream, queue):
            '''
            Collect lines from 'stream' and put them in 'quque'.
            '''

            while not self.bad:
                line = stream.readline().replace('\r', '').strip()
                if line:
                    queue.put(line)
                else:
                    self.bad = True
                    print 'CHILD PROCESS BAD'

        self._t = Thread(target = _populateQueue,
                args = (self._s, self._q))
        self._t.daemon = True
        self._t.start() #start collecting lines from the stream

    def readline(self, timeout = None):
        try:
            return self._q.get(block = timeout is not None,
                    timeout = timeout)
        except Empty:
            return None