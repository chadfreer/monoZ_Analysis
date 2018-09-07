import FWCore.ParameterSet.Config as cms
'''
Produces "preppedTaus"

Uses the MET uncertainty variations from runMETCorrectionsAndUncertainties() function
See L774 of PhysicsTools/PatUtils/python/tools/runMETCorrectionsAndUncertainties.py
'''
def tauPrep(process, isMC):
    process.preppedTaus = cms.EDProducer("TauPrep",
        tauSrc = cms.InputTag("slimmedTaus"),
        vertexSrc = cms.InputTag("offlineSlimmedPrimaryVertices"),
        minPtAnyVariation = cms.double(18.),
        kinematicCutAnyVariation = cms.string('pt > 18 && abs(eta) < 2.4'),
        finalCut = cms.string('tauID("decayModeFinding") && tauID("byCombinedIsolationDeltaBetaCorrRaw3Hits") < 5'),
    )
