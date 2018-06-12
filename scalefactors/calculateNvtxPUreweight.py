#!/usr/bin/env python
import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
import math

# Run NVtxDistribution selector on some DY and DoubleMuon to derive scale factor
f = ROOT.TFile.Open('NVtxDistribution-DYandData.root')
hmc = f.Get('DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/nvtxSelection')
hdata = f.Get('DoubleMuon/nvtxSelection')

nBins = 75

stackData = [hdata.GetBinContent(i) for i in range(1, nBins+1)]
stackMC = [hmc.GetBinContent(i) for i in range(1, nBins+1)]
stackMCerr = [hmc.GetBinError(i) for i in range(1, nBins+1)]

stackDataNormed = map(lambda i: i/sum(stackData), stackData)
stackMCNormed = map(lambda i: i/sum(stackMC), stackMC)
stackNvtx = ROOT.TH1D('nvtx', 'nvtx reweighting for 2016 ReReco vs. RunIISummer16 (Tranche4) MC', nBins, 0, nBins)
for b in range(nBins):
    d = stackDataNormed[b]
    de = math.sqrt(stackData[b]) / sum(stackData)
    m = stackMCNormed[b]
    me = stackMCerr[b] / sum(stackMC)
    if m>0 and d>0:
        r = d/m
        re = math.sqrt( (de/d)**2 + (me/m)**2 )
    else:
        r = 1.
        re = 1.
    print "Bin %d: %f +- %f" % (b, r, re)
    stackNvtx.SetBinContent(b+1, r)
    stackNvtx.SetBinError(b+1, re)

fOut = ROOT.TFile('data/nvtxReRecoReweight.root', 'recreate')
fOut.cd()
stackNvtx.Write()
