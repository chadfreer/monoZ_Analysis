#!/usr/bin/env python
import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
from DataFormats.FWLite import Handle, Runs, Lumis, Events
import sys
import math

lorentz = ROOT.Math.LorentzVector('ROOT::Math::PxPyPzE4D<double>')

lheHandle = Handle('LHEEventProduct')

hInclusive = ROOT.TH1D("Inclusive", "Inclusive;Z p_{T};Counts / fb", 300, 0, 300)
hInclusive.SetLineColor(ROOT.kBlack)
h100to200 = ROOT.TH1D("100to200", "100 #leq p_{T} #leq 200;Z p_{T};Counts / fb", 300, 0, 300)
h100to200.SetLineColor(ROOT.kGreen)
h200toinf = ROOT.TH1D("200toinf", "200 #leq p_{T};Z p_{T};Counts / fb", 300, 0, 300)
h200toinf.SetLineColor(ROOT.kBlue)

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
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v2/120000/02A210D6-F5C3-E611-B570-008CFA197BD4.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v2/120000/0EA60289-18C4-E611-8A8F-008CFA110AB4.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v2/120000/101D622A-85C4-E611-A7C2-C4346BC80410.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v2/120000/1057A261-06C4-E611-A94C-008CFA11113C.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v2/120000/10F21356-7DC4-E611-B7CE-008CFA052C0C.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v2/120000/143EF93E-00C4-E611-8E98-008CFA11131C.root',
])
fillhist(hInclusive, eventsInclusive, 4958. * 1e3)

events100to200 = Events([
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Zpt-100to200_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/02E81655-F4C7-E611-B2B8-28924A33AF26.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Zpt-100to200_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/06FB2F3C-05C8-E611-B2BF-FA163ED5203D.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Zpt-100to200_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/144799D8-06D5-E611-8845-02163E0129EF.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Zpt-100to200_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/18BD14FF-F7C7-E611-872B-0026B95A45FB.root',
])
fillhist(h100to200, events100to200, 57.25 * 1e3)

events200toinf = Events([
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Zpt-200toInf_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/327F1D49-5CCB-E611-9CDB-20CF305616FF.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Zpt-200toInf_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/3A411EDB-43CD-E611-9874-20CF307C98DC.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Zpt-200toInf_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/862756D7-52CB-E611-ABF4-20CF3027A5C5.root',
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Zpt-200toInf_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/8ABFC78B-3ECD-E611-BFC2-20CF3027A62B.root',
])
fillhist(h200toinf, events200toinf, 6.753 * 1e3)

hInclusive.Draw('histex0')
h100to200.Draw('histex0same')
h200toinf.Draw('histex0same')
hstitched = h100to200.Clone("stitched")
hstitched.Add(h200toinf)
hstitched.SetTitle("Stitched")
hstitched.SetLineColor(ROOT.kBlack)
hstitched.SetLineStyle(ROOT.kDashed)
hstitched.Draw('histex0same')
ROOT.gPad.Print("ptStitchLO.root")
