#!/usr/bin/env python
import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
from DataFormats.FWLite import Handle, Runs, Lumis, Events
import sys

genHandle = Handle('vector<reco::GenParticle>')

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISpring16MiniAODv2/ZH_ZToEE_HToInvisible_M125_13TeV_powheg_pythia8/MINIAODSIM/PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-v1/80000/B240E2B8-953A-E611-B6F2-549F358EB7BD.root')
#events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIIFall15MiniAODv2/ggZH_HToInv_ZToLL_M125_13TeV_powheg_pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/00000/1C9160F1-ABD1-E511-B264-002590A80E1E.root')
#events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_Zpt-150toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/34D8250D-1CB7-E611-82A5-24BE05C616E1.root')

def cand(pair):
    return pair[0].p4() + pair[1].p4()

dphi = ROOT.Math.VectorUtil.DeltaPhi
deltaR = ROOT.Math.VectorUtil.DeltaR

hdrpt = ROOT.TH2D("ZdeltaRllvsPt", "DeltaR Z leptons;#DeltaR(l_{1}, l_{2});p_{T,Z};Counts", 30, 0, 3, 60, 0, 600);

for iev, event in enumerate(events):
    event.getByLabel('prunedGenParticles', genHandle)
    genParticles = genHandle.product()

    leptons = [p for p in genParticles if p.isHardProcess() and abs(p.pdgId()) in [11,13]]
    leadingPair = None
    for i, lep1 in enumerate(leptons):
        lep2 = filter(lambda l: l.pdgId() == -lep1.pdgId() and l.mother() == lep1.mother(), leptons)
        if len(lep2) != 1:
            print "wtf"
        lep2 = lep2[0]
        pair = (lep1, lep2) if lep1.pt() > lep2.pt() else (lep2, lep1)
        if not leadingPair:
            leadingPair = pair
        elif cand(pair).pt() > cand(leadingPair).pt():
            print "found another Z?"
            leadingPair = pair

    if not leadingPair:
        continue

    Zcand = cand(leadingPair)
    deltaRll = deltaR(pair[0].p4(), pair[1].p4())
    hdrpt.Fill(deltaRll, Zcand.pt())
    
hdrpt.Draw("colz")
ROOT.gPad.Print("drll_vs_pt.root")
