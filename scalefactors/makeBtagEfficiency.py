#!/usr/bin/env python
import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
import math

# Run MonoZSelector on some TTbar (and DY for more light jets) to derive scale factor
# Use flags: skipSystematics doBtagEfficiency
f = ROOT.TFile.Open('MonoZSelector-ttbar_dy.root')

dy_eff = f.Get('DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/btagEffNumerator')
dy_denom = f.Get('DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/btagEffDenominator')

ttbar_eff = f.Get('TTTo2L2Nu_TuneCUETP8M2_ttHtranche3_13TeV-powheg-pythia8/btagEffNumerator')
ttbar_denom = f.Get('TTTo2L2Nu_TuneCUETP8M2_ttHtranche3_13TeV-powheg-pythia8/btagEffDenominator')

combined_eff = dy_eff.Clone("combinedEff")
combined_eff.Add(ttbar_eff)
combined_denom = dy_denom.Clone("combinedDenom")
combined_denom.Add(ttbar_denom)

dy_eff.Divide(dy_denom)
ttbar_eff.Divide(ttbar_denom)
combined_eff.Divide(combined_denom)

btagEff = ROOT.ScaleFactor("btagEfficiency", "Moriond 17 MC B-tag efficiency for CSVv2 WP medium = 0.8484, x=flavor mod 3, y=pt, z=eta")
btagEff.Set3DHist(combined_eff, None, None, ROOT.ScaleFactor.AsInHist)
fout = ROOT.TFile("btagEfficiency.root", "recreate")
btagEff.Write()

def drawFlavor(hist, flavor, name):
    hist.GetXaxis().SetRange(flavor+1,flavor+1)
    hproj = hist.Project3D("yz")
    hproj.SetMarkerSize(3)
    hproj.Draw("colztext")
    hproj.GetZaxis().SetRangeUser(0,1)
    hproj.GetYaxis().SetRangeUser(20,120)
    ROOT.gPad.Print("~/www/monoZ/btag/%s.png" % name)

drawFlavor(dy_eff, 0, "dy_light")
drawFlavor(dy_eff, 1, "dy_charm")
drawFlavor(dy_eff, 2, "dy_bottom")
drawFlavor(ttbar_eff, 0, "ttbar_light")
drawFlavor(ttbar_eff, 1, "ttbar_charm")
drawFlavor(ttbar_eff, 2, "ttbar_bottom")
