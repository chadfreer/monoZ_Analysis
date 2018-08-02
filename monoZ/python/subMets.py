import FWCore.ParameterSet.Config as cms
'''
Recipes for making 'sub'-METs
Includes vector sums of PF candidates for various
detector volumes, and track MET (charged PF associated
with primary vertex)

Returns a cms.PSet of input tags to add to the EventTupler

PDG ID codes for PF candidates:
    1 : Hadron-like HF particle
    2 : EM-like HF particle
   11 : Electron
   13 : Muon
   22 : Photon
  130 : Neutral hadron (K_long)
  211 : Charged hadron (pi+)
From https://github.com/cms-sw/cmssw/blob/CMSSW_8_0_X/DataFormats/ParticleFlowCandidate/src/PFCandidate.cc#L238-L253
'''
def subMets(process, isMC):
    process.trackMETs = cms.EDProducer("SubMETProducer",
        packedCandidatesSrc = cms.InputTag("packedPFCandidates"),
        selection = cms.string("charge != 0"),
        vertexSrc = cms.InputTag("offlineSlimmedPrimaryVertices"),
        dzToPrimaryVertex = cms.double(0.1),
    )

    subMet_generic = cms.EDProducer("SubMETProducer",
        packedCandidatesSrc = cms.InputTag("packedPFCandidates"),
        selection = cms.string("change me"),
        vertexSrc = cms.InputTag("offlineSlimmedPrimaryVertices"),
        dzToPrimaryVertex = cms.double(999.),
    )

    process.subMETchargedBarrel = subMet_generic.clone()
    process.subMETchargedBarrel.selection = "charge != 0 && abs(eta) < 1.3"

    process.subMETneutralBarrel = subMet_generic.clone()
    process.subMETneutralBarrel.selection = "pdgId == 130 && abs(eta) < 1.3"

    process.subMETphotonBarrel = subMet_generic.clone()
    process.subMETphotonBarrel.selection = "pdgId == 22 && abs(eta) < 1.3"

    process.subMETchargedEndcap = subMet_generic.clone()
    process.subMETchargedEndcap.selection = "charge != 0 && abs(eta) > 1.6 && abs(eta) < 2.4"

    process.subMETneutralEndcap = subMet_generic.clone()
    process.subMETneutralEndcap.selection = "pdgId == 130 && abs(eta) > 1.6 && abs(eta) < 2.4"

    process.subMETphotonEndcap = subMet_generic.clone()
    process.subMETphotonEndcap.selection = "pdgId == 22 && abs(eta) > 1.6 && abs(eta) < 2.4"

    process.subMETneutralEndcapNoTracker = subMet_generic.clone()
    process.subMETneutralEndcapNoTracker.selection = "pdgId == 130 && abs(eta) > 2.4 && abs(eta) < 3"

    process.subMETphotonEndcapNoTracker = subMet_generic.clone()
    process.subMETphotonEndcapNoTracker.selection = "pdgId == 22 && abs(eta) > 2.4 && abs(eta) < 3"

    process.subMEThadronicHF = subMet_generic.clone()
    process.subMEThadronicHF.selection = "pdgId == 1 && abs(eta) > 3"

    process.subMETemHF = subMet_generic.clone()
    process.subMETemHF.selection = "pdgId == 2 && abs(eta) > 3"

    process.subMETmissingHt = cms.EDProducer("SubMETProducer",
        packedCandidatesSrc = cms.InputTag("packedPFCandidates"),
        selection = cms.string("abs(pdgId) == 211 || pdgId == 130"),
        vertexSrc = cms.InputTag("offlineSlimmedPrimaryVertices"),
        # TODO: reasonable dZ cut for sumHt?
        dzToPrimaryVertex = cms.double(0.1),
    )

    subMETs = cms.PSet(
        trackMetSrc = cms.InputTag("trackMETs"),
        subMetSrcVect = cms.VInputTag(
            # This order should match the enum in EventTuple.h
            "subMETchargedBarrel",
            "subMETneutralBarrel",
            "subMETphotonBarrel",
            "subMETchargedEndcap",
            "subMETneutralEndcap",
            "subMETphotonEndcap",
            "subMETneutralEndcapNoTracker",
            "subMETphotonEndcapNoTracker",
            "subMEThadronicHF",
            "subMETemHF",
            "subMETmissingHt",
        ),
    )
    return subMETs
