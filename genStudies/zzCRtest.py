#!/usr/bin/env python
import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
from DataFormats.FWLite import Handle, Runs, Lumis, Events
import sys

genHandle = Handle('vector<reco::GenParticle>')

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISpring16MiniAODv2/ZZTo4L_13TeV_powheg_pythia8/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/00000/024C8A3E-7D1A-E611-A094-002590494C82.root')
#events = Events('root://cmsxrootd.fnal.gov//store/backfill/1/RunIISpring16MiniAODv2/ZZTo2L2Nu_13TeV_powheg_pythia8/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/70000/0408B23E-4A21-E611-86A7-0025905AC824.root')

def cand(pair):
    return pair[0].p4() + pair[1].p4()

dphi = ROOT.Math.VectorUtil.DeltaPhi

counts = [0]*8

for iev, event in enumerate(events):
    counts[0] += 1
    event.getByLabel('prunedGenParticles', genHandle)
    genParticles = genHandle.product()

    leptons = [p for p in genParticles if p.isHardProcess() and abs(p.pdgId()) >= 11 and abs(p.pdgId()) <= 14]
    leadingPair = None
    trailingPair = None
    for i, lep1 in enumerate(leptons):
        lep2 = filter(lambda l: l.pdgId() == -lep1.pdgId() and l.mother() == lep1.mother(), leptons)
        if len(lep2) != 1:
            print "wtf"
        lep2 = lep2[0]
        pair = (lep1, lep2) if lep1.pt() > lep2.pt() else (lep2, lep1)
        if not leadingPair:
            leadingPair = pair
        elif cand(pair).pt() > cand(leadingPair).pt():
            trailingPair = leadingPair
            leadingPair = pair
        else:
            trailingPair = pair

    # We're skipping taus so sometimes no pair is found
    if not leadingPair:
        continue

    def goodLep(lep):
        isCharged = lep.pdgId() % 2
        goodPt = lep.pt() > 20
        goodEta = abs(lep.eta()) < 2.4 if abs(lep.pdgId()) == 11 else abs(lep.eta()) < 2.5
        return isCharged and goodPt and goodEta
    
    goodPair = lambda p: goodLep(p[0]) and goodLep(p[1])

    visiblePair = None
    if goodPair(leadingPair) and goodPair(trailingPair):
        visiblePair = leadingPair if ROOT.gRandom.Integer(2) else trailingPair
    elif goodPair(leadingPair):
        visiblePair = leadingPair
    elif goodPair(trailingPair):
        visiblePair = trailingPair
    else:
        continue
    invisiblePair = leadingPair if visiblePair == trailingPair else trailingPair

    goodMass = abs(cand(visiblePair).M()-91.1876) < 15.
    goodPt = cand(visiblePair).Pt() > 60.
    goodMet = cand(invisiblePair).Pt() > 100.
    goodAngle = abs(dphi(cand(visiblePair), cand(invisiblePair))) > 2.8
    goodBalance = abs(1-cand(invisiblePair).Pt()/cand(visiblePair).Pt()) < 0.4

    counts[1] += 1

    if goodMass:
        counts[2] += 1
    else:
        continue

    if goodPt:
        counts[3] += 1
    else:
        continue

    if goodMet:
        counts[4] += 1
    else:
        continue

    if goodAngle:
        counts[5] += 1
    else:
        continue

    if goodBalance:
        counts[6] += 1
    else:
        continue

    # Only relevant for 4l
    if goodLep(invisiblePair[0]) and goodLep(invisiblePair[1]):
        counts[7] += 1
    else:
        continue

print repr(counts)
