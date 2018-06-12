import FWCore.ParameterSet.Config as cms
'''
Produces "preppedMuons"

Uses the MET uncertainty variations from runMETCorrectionsAndUncertainties() function
See L768 of PhysicsTools/PatUtils/python/tools/runMETCorrectionsAndUncertainties.py
'''
def muonPrep(process, isMC):
    # TODO: corrections
    # KaMuCa? Rochester? MuscleFit?

    process.preppedMuons = cms.EDProducer("MuonPrep",
        muonSrc = cms.InputTag("slimmedMuons"),
        vertexSrc = cms.InputTag("offlineSlimmedPrimaryVertices"),
        kinematicCutAnyVariation = cms.string('pt > 10 && abs(eta) < 2.4'),
        finalCut = cms.string('isLooseMuon'),
    )
