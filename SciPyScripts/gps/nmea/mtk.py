from common import nmeaSentence

class Ack(nmeaSentence):
    __message = ("Invalid", "Unsupported", "Failed", "Success")
    def __init__ (self, payload):
        nmeaSentence.__init__(self, payload)
        p = payload.split (",")
        self.id = p[0]
        self.request = p[1]
        self.message = self.__messages[int(p[2])]
        self._mutable = False
        return
    pass

class Output(nmeaSentence):
    def __init__ (self,
                  payload=None,
                  gll=0, rmc=0, vtg=0, gga=0, gsa=0, gsv=0, grs=0, gst=0,
                  malm=0, meph=0, mdgp=0, mdbg=0, zda=0, mchn=0):
        if payload is not None:
            nmeaSentence.__init__(self, payload)
            p = payload.split (",")
            self.gll  = int (p[1])
            self.rmc  = int (p[2])
            self.vtg  = int (p[3])
            self.gga  = int (p[4])
            self.gsa  = int (p[5])
            self.gsv  = int (p[6])
            self.grs  = int (p[7])
            self.gst  = int (p[8])
            
            self.malm = int (p[14])
            self.meph = int (p[15])
            self.mdgp = int (p[16])
            self.mdbg = int (p[17])
            self.zda  = int (p[18])
            self.mchn = int (p[19])
            pass
        else:
            self.gll = gll
            self.rmc = rmc
            self.vtg = vtg
            self.gga = gga
            self.gsa = gsa
            self.gsv = gsv
            self.grs = grs
            self.gst = gst
            self.malm = malm
            self.meph = meph
            self.mdgp = mdgp
            self.mdbg = mdbg
            self.zda = zda
            self.mchn = mchn
            payload = "PMTK314,%d,%d,%d,%d,%d,%d,%d,%d,0,0,0,0,0,%d,%d,%d,%d,%d,%d"
            payload = payload % (gll, rmc, vtg, gga, gsa, gsv, grs, gst,
                                 malm, meph, mdgp, mdbg, zda, mchn)
            nmeaSentence.__init__(self, payload)
            pass
        
        self._mutable = False
        return

    def set (self,
             gll=None, rmc=None, vtg=None, gga=None, gsa=None, gsv=None,
             grs=None, gst=None, malm=None, meph=None, mdgp=None, mdbg=None,
             zda=None, mchn=None):
        if gll is None: gll = self.gll
        if rmc is None: rmc = self.rmc
        if vtg is None: vtg = self.vtg
        if gga is None: gga = self.gga
        if gsa is None: gsa = self.gsa
        if gsv is None: gsv = self.gsv
        if grs is None: grs = self.grs
        if gst is None: gst = self.gst
        if malm is None: malm = self.malm
        if meph is None: meph = self.meph
        if mdgp is None: mdgp = self.mdgp
        if mdbg is None: mdbg = self.mdbg
        if zda is None: zda = self.zda
        if mchn is None: mchn = self.mchn

        payload = "PMTK314,%d,%d,%d,%d,%d,%d,%d,%d,0,0,0,0,0,%d,%d,%d,%d,%d,%d"
        payload = payload % (gll, rmc, vtg, gga, gsa, gsv, grs, gst,
                             malm, meph, mdgp, mdbg, zda, mchn)
        return Output(payload)
    pass

class SysMsg(nmeaSentence):
    __messages = ("Unknown", "Start")
    def __init__ (self, payload):
        nmeaSentence.__init__(self, payload)
        p = payload.split (",")
        self.id = p[0]
        self.message = self.__messages[int(p[1])]
        self._mutable = False
        return
    pass

class Unknown(nmeaSentence):
    def __init__ (self, payload):
        nmeaSentence.__init__(self, payload)
        self._mutable = False
        return
    pass
