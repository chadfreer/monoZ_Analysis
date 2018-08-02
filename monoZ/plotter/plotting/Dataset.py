import crossSections


class Dataset:
    def __init__(self, resultDirectory, luminosity):
        self._dir = resultDirectory
        self._name = resultDirectory.GetName()

        self._plots = []
        for key in self._dir.GetListOfKeys():
            if "TH" in key.GetClassName() and key.GetName() != "counters":
                self._plots.append(key.GetName())

        self._lumi = luminosity

        # If exists -> True, if None -> False
        self._isMC = (resultDirectory.Get("isMC/Yes") != None)

        self._counters = resultDirectory.Get("counters")

        if self._name in crossSections.crossSections:
            self._crossSection = crossSections.crossSections[self._name]
        elif self._isMC:
            print "Warning: Missing cross section for %s, using 1 pb" % repr(self)
            self._crossSection = 1.

    def __repr__(self):
        return '<Dataset instance ("%s")>' % self._name

    def isValid(self):
        if self._counters == None:
            return False
        selectorCounter = self._dir.Get("selectorCounter")
        if selectorCounter == None:
            return False
        if selectorCounter.GetBinContent(1) == 0:
            print "Info: %r has zero entries in selectorCounter, excluding" % self
            return False
        return True

    def plots(self):
        return self._plots

    def setCrossSection(self, newXs):
        self._crossSection = newXs

    def getNormalization(self):
        norm = 1.
        if self._isMC:
            sumWeights = self._counters.GetBinContent(3) - self._counters.GetBinContent(2)
            norm = self._crossSection * self._lumi / sumWeights
        return norm

    def getPlot(self, plotName):
        hist = self._dir.Get(plotName)
        if not hist:
            raise Exception("No plot named %s in %s" % (plotName, repr(self)))

        hist.SetName("%s__%s" % (plotName, self._name))
        hist.Scale(self.getNormalization())
        hist.UseCurrentStyle()
        return hist
