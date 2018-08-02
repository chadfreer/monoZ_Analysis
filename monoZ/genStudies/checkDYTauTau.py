#!/usr/bin/env python
import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
from DataFormats.FWLite import Handle, Runs, Lumis, Events
import sys
import math

lorentz = ROOT.Math.LorentzVector('ROOT::Math::PxPyPzE4D<double>')

genHandle = Handle('vector<reco::GenParticle>')

hInclusive = ROOT.TH1D("Inclusive", "Inclusive;Z p_{T};Counts / fb", 300, 0, 300)
hInclusive.SetLineColor(ROOT.kBlack)

eventsInclusive = Events([
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v2/120000/02A210D6-F5C3-E611-B570-008CFA197BD4.root',
])
eventsTau = Events([
    'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DYJetsToTauTau_ForcedMuEleDecay_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/06C6F31F-5BB9-E611-B13C-0025907277A0.root',
])

npassCut = 0
for iev, event in enumerate(eventsInclusive):
    event.getByLabel('prunedGenParticles', genHandle)
    genParticles = genHandle.product()

    ntaus = sum(map(lambda p: p.isHardProcess() and abs(p.pdgId()) == 15, genParticles))
    ourCut = lambda p: p.isDirectPromptTauDecay() and abs(p.pdgId()) in [11, 13]
    nCut = sum(map(ourCut, genParticles))
    if nCut < 2:
        npassCut += 1

    if iev > 1000:
        break
    if ntaus == 2 and nCut < 2:
        print "="*40

        allTauLep = []
        def printP(p, indent=0):
            print "\t"*indent, "Particle", p.pdgId(), "status", p.status(), "directTau=", p.isDirectPromptTauDecay(), "directTauFinal=", p.isDirectPromptTauDecayProductFinalState()
            if ourCut(p):
                allTauLep.append(p)
            for i in range(p.numberOfDaughters()):
                printP(p.daughter(i), indent+1)
        hardZ = filter(lambda p: p.isHardProcess() and p.pdgId() in [22,23], genParticles)
        if len(hardZ) == 1:
            printP(hardZ[0])
        else:
            print repr(hardZ)

        missing = filter(lambda p: ourCut(p) and p not in allTauLep, genParticles)
        for p in missing:
            print "Missing Particle", p.pdgId(), "status", p.status(), "directTau=", p.isDirectPromptTauDecay(), "directTauFinal=", p.isDirectPromptTauDecayProductFinalState()

        if len(hardZ) != 1 or len(missing) > 0:
            print "---- All genParticles"
            printP(genParticles[0])

