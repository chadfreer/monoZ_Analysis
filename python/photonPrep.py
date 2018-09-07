import FWCore.ParameterSet.Config as cms
'''
EG regression handled in electronPrep.py
Smearing from https://twiki.cern.ch/twiki/bin/view/CMS/EGMSmearer
ID from https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedPhotonIdentificationRun2
Produces "calibratedPatPhotons"
'''
def photonPrep(process, isMC, useCalibration):
    photonInput = cms.InputTag("slimmedPhotons::ANA")
    if useCalibration:
        if not hasattr(process, "RandomNumberGeneratorService"):
            process.load('Configuration.StandardSequences.Services_cff')
            process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService")

        process.RandomNumberGeneratorService.calibratedPatPhotons = cms.PSet(
            initialSeed = cms.untracked.uint32(81),
            engineName = cms.untracked.string('TRandom3'),
        )
        process.load('EgammaAnalysis.ElectronTools.calibratedPhotonsRun2_cfi')
        process.calibratedPatPhotons.isMC = cms.bool(isMC)
        photonInput = cms.InputTag('calibratedPatPhotons')

    import PhysicsTools.SelectorUtils.tools.vid_id_tools as vid_tools
    vid_tools.switchOnVIDPhotonIdProducer(process, vid_tools.DataFormat.MiniAOD)

    my_id_modules = ['RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_Spring16_V2p2_cff']
    for idmod in my_id_modules:
        vid_tools.setupAllVIDIdsInModule(process, idmod, vid_tools.setupVIDPhotonSelection)

    process.egmPhotonIDs.physicsObjectSrc = photonInput
    process.photonMVAValueMapProducer.srcMiniAOD = photonInput
    process.photonIDValueMapProducer.srcMiniAOD = photonInput
    process.photonRegressionValueMapProducer.srcMiniAOD = photonInput

    process.preppedPhotons = cms.EDProducer("PhotonPrep",
        photonSrc = photonInput,
        uncalibratedSrc = cms.InputTag("slimmedPhotons::ANA"),
        looseId = cms.InputTag("egmPhotonIDs:cutBasedPhotonID-Spring16-V2p2-loose"),
        mediumId = cms.InputTag("egmPhotonIDs:cutBasedPhotonID-Spring16-V2p2-medium"),
        tightId = cms.InputTag("egmPhotonIDs:cutBasedPhotonID-Spring16-V2p2-tight"),
        triggerObjects = cms.InputTag("selectedPatTrigger"),
        triggerPrescales = cms.InputTag("patTrigger"),
        triggerPathsForMinPrescale = cms.vstring(
            # Don't forget to add to process.triggerFilter in test/eventTuples.py
            # No stars here
            "HLT_Photon50_R9Id90_HE10_IsoM_v",
            "HLT_Photon75_R9Id90_HE10_IsoM_v",
            "HLT_Photon90_R9Id90_HE10_IsoM_v",
            "HLT_Photon120_R9Id90_HE10_IsoM_v",
            "HLT_Photon165_HE10_v",
        ),
        kinematicCutAnyVariation = cms.string('pt > 50 && abs(eta) < 1.4442'),
        finalCut = cms.string('userInt("LooseId")'),
    )
