#!/usr/bin/env python
import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
from DataFormats.FWLite import Handle, Runs, Lumis, Events
import sys
import math

lorentz = ROOT.Math.LorentzVector('ROOT::Math::PxPyPzE4D<double>')

lheHandle = Handle('LHEEventProduct')

hInclusive = ROOT.TH1D("Inclusive", "Inclusive;Z p_{T};Counts / fb", 400, 0, 400)
hInclusive.SetLineColor(ROOT.kBlack)
h50to100 = ROOT.TH1D("50to100", "50 #leq p_{T} #leq 100;Z p_{T};Counts / fb", 400, 0, 400)
h50to100.SetLineColor(ROOT.kRed)
h100to250 = ROOT.TH1D("100to250", "100 #leq p_{T} #leq 250;Z p_{T};Counts / fb", 400, 0, 400)
h100to250.SetLineColor(ROOT.kGreen)
h250to400 = ROOT.TH1D("250to400", "250 #leq p_{T} #leq 400;Z p_{T};Counts / fb", 400, 0, 400)
h250to400.SetLineColor(ROOT.kBlue)

def fillhist(hist, events, xs):
    sumW = 0
    for iev, event in enumerate(events):
        event.getByLabel('externalLHEProducer', lheHandle)
        lhe = lheHandle.product()
        weight = 1 if lhe.originalXWGTUP() > 0 else -1
        sumW += weight

        lheParticles = lhe.hepeup()
        def lhep4(i):
            px = lheParticles.PUP.at(i)[0]
            py = lheParticles.PUP.at(i)[1]
            pz = lheParticles.PUP.at(i)[2]
            pE = lheParticles.PUP.at(i)[3]
            return lorentz(px, py, pz, pE)

        nZlep = 0
        p4 = lorentz()
        for i in range(lheParticles.NUP):
            if abs(lheParticles.IDUP[i]) in [11, 13, 15]:
                nZlep += 1
                p4 += lhep4(i)
        if nZlep == 2:
            hist.Fill(p4.pt(), weight)
        else:
            print "bad event"
            for i in range(lheParticles.NUP):
                print "part %d id %d pt %f" % (i, lheParticles.IDUP[i], lhep4(i).pt())
    hist.Scale(xs/sumW)


eventsInclusive = Events([
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_HCALDebug_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/00312D7A-FEBD-E611-A713-002590DB923E.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_HCALDebug_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/00355459-F4BD-E611-B5E8-D4AE526A11F3.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_HCALDebug_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/00BCC036-F6BD-E611-92F5-0025905A6118.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_HCALDebug_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/028EE230-F7BD-E611-BCDB-0CC47AA9906E.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_HCALDebug_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/02C57965-0DBE-E611-91EF-70106F4A9254.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_HCALDebug_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/0632A87F-0DBE-E611-8948-F04DA275BF11.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_HCALDebug_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/063CB919-03BE-E611-B04B-70106F4D68D4.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_HCALDebug_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/0668344A-FEBD-E611-9FD3-0025905A48D0.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_HCALDebug_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/0854F2BE-0FBE-E611-8864-E41D2D08DE00.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_HCALDebug_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/0881F329-F4BD-E611-88F0-FA163E4A37A7.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_HCALDebug_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/089C4B33-FFBD-E611-BEE7-70106F48BA5E.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_HCALDebug_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/0A6C3F97-FDBD-E611-8E50-842B2B758AD8.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_HCALDebug_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/0A900466-F0BD-E611-8E89-001C23C105CF.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_HCALDebug_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/0AC8C964-F9BD-E611-A796-0025905A60CA.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_HCALDebug_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/0C255783-F3BD-E611-BE80-7845C4FC39C8.root',
])
fillhist(hInclusive, eventsInclusive, 5938. * 1e3)

events50to100 = Events([
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Pt-50To100_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/100000/00E3D7B3-9DCE-E611-A42D-0025905A609A.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Pt-50To100_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/100000/0462F883-C0CE-E611-8CEE-0CC47A4D764A.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Pt-50To100_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/100000/0E1E6882-A0CE-E611-BB4A-0CC47A7452D0.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Pt-50To100_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/100000/0EC51971-9FCE-E611-A1FE-0025905B85FC.root',
])
fillhist(h50to100, events50to100, 354.6 * 1e3)

events100to250 = Events([
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Pt-100To250_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/120000/104AF025-6DCB-E611-BFB4-0025904B8708.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Pt-100To250_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/120000/2029BD2C-7DCB-E611-8E08-0025904A87E2.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Pt-100To250_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/120000/20E39922-6ECB-E611-8C93-0025904C7F80.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Pt-100To250_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/120000/28BA2C57-7CCB-E611-BECC-0025901D4894.root',
])
fillhist(h100to250, events100to250, 83.05 * 1e3)

events250to400 = Events([
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Pt-250To400_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/120000/0A953D30-0BCD-E611-B4CB-0CC47AD99112.root',
])
fillhist(h250to400, events250to400, 3.043 * 1e3)

hInclusive.Draw('histex0')
h50to100.Draw('histex0same')
h100to250.Draw('histex0same')
h250to400.Draw('histex0same')
hstitched = h50to100.Clone("stitched")
hstitched.Add(h100to250)
hstitched.Add(h250to400)
hstitched.SetTitle("Stitched")
hstitched.SetLineColor(ROOT.kBlack)
hstitched.SetLineStyle(ROOT.kDashed)
hstitched.Draw('histex0same')
ROOT.gPad.Print("ptStitch.root")
