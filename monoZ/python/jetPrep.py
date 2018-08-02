import FWCore.ParameterSet.Config as cms
'''
Produces "preppedJets"
Embeds JetEnUp/Down and JetResUp/Down variations
(JetEn = JES, JetRes = JER)
Using jets selected for Type1 MET corrections

Also embeds Jet IDs described here:
https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID#Recommendations_for_13_TeV_data
as 'looseJetId' and 'tightJetId'

jetSelectorForMet comes from runMETCorrectionsAndUncertainties() function
See L1280 of PhysicsTools/PatUtils/python/tools/runMETCorrectionsAndUncertainties.py
'''
def jetPrep(process, isMC):
    process.preppedJets = cms.EDProducer("JetPrep",
        jetSrc = cms.InputTag("jetSelectorForMet"),
        shiftedJESUp   = cms.InputTag("shiftedPatJetEnUp"),
        shiftedJESDown = cms.InputTag("shiftedPatJetEnDown"),
        # If data, these are ignored
        shiftedJERUp   = cms.InputTag("shiftedPatSmearedJetResUp"),
        shiftedJERDown = cms.InputTag("shiftedPatSmearedJetResDown"),
        kinematicCutAnyVariation = cms.string('pt > 20 && abs(eta) < 5.'),
        finalCut = cms.string('userInt("looseJetId")'),
    )
