import FWCore.ParameterSet.Config as cms
'''
From https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2
In future, these filters will be part of miniAOD TriggerResults::RECO

Returns a cms.PSet of input tags to add to the EventTupler
'''
def metFilters(process, isMC):
    # Some might argue that
    # if isMC:
    #     return
    process.load('RecoMET.METFilters.BadChargedCandidateFilter_cfi')
    process.BadChargedCandidateFilter.muons = cms.InputTag("slimmedMuons")
    process.BadChargedCandidateFilter.PFCandidates = cms.InputTag("packedPFCandidates")

    process.load('RecoMET.METFilters.BadPFMuonFilter_cfi')
    process.BadPFMuonFilter.muons = cms.InputTag("slimmedMuons")
    process.BadPFMuonFilter.PFCandidates = cms.InputTag("packedPFCandidates")

    filters = cms.PSet(
        BadPFMuonFilter = cms.InputTag("BadPFMuonFilter"),
        BadChargedCandidateFilter = cms.InputTag("BadChargedCandidateFilter"),
    )

    return filters
