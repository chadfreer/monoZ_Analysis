import FWCore.ParameterSet.Config as cms
'''
EG regression: https://twiki.cern.ch/twiki/bin/view/CMS/EGMRegression
EG smearing: https://twiki.cern.ch/twiki/bin/view/CMS/EGMSmearer
TODO: smearing seed energy correction not applied since they promise it in final recipe (as of Jan 5 2017)
EG IDs from https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2
High-pt IDs from https://twiki.cern.ch/twiki/bin/view/CMS/HEEPElectronIdentificationRun2
Produces "preppedElectrons"

Uses the MET uncertainty variations from runMETCorrectionsAndUncertainties() function
See L754 of PhysicsTools/PatUtils/python/tools/runMETCorrectionsAndUncertainties.py
'''
def electronPrep(process, isMC, useCalibration):
    # from EgammaAnalysis.ElectronTools.regressionWeights_cfi import regressionWeights
    # process = regressionWeights(process)
    # process.load('EgammaAnalysis.ElectronTools.regressionApplication_cff')
    # 
    # if not hasattr(process, "RandomNumberGeneratorService"):
    #     process.load('Configuration.StandardSequences.Services_cff')
    #     process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService")
    # 
    # process.RandomNumberGeneratorService.calibratedPatElectrons = cms.PSet( initialSeed = cms.untracked.uint32(81),
    #     engineName = cms.untracked.string('TRandom3'),
    # )
    process.load('EgammaAnalysis.ElectronTools.calibratedElectronsRun2_cfi')
    process.calibratedPatElectrons.isMC = cms.bool(isMC)

    electronInput = cms.InputTag("slimmedElectrons::ANA")
    if useCalibration:
        electronInput = cms.InputTag("calibratedPatElectrons")

    import PhysicsTools.SelectorUtils.tools.vid_id_tools as vid_tools
    vid_tools.switchOnVIDElectronIdProducer(process, vid_tools.DataFormat.MiniAOD)

    my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Summer16_80X_V1_cff',
                    'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV70_cff']

    for idmod in my_id_modules:
        vid_tools.setupAllVIDIdsInModule(process, idmod, vid_tools.setupVIDElectronSelection)

    process.egmGsfElectronIDs.physicsObjectSrc = electronInput
    process.electronMVAValueMapProducer.srcMiniAOD = electronInput
    process.electronRegressionValueMapProducer.srcMiniAOD = electronInput
    process.heepIDVarValueMaps.elesMiniAOD = electronInput
    print " --- electronInput : ", electronInput
    process.preppedElectrons = cms.EDProducer("ElectronPrep",
        electronSrc = electronInput,
        uncalibratedSrc = cms.InputTag("slimmedElectrons::ANA"),
        vetoId = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-veto"),
        looseId = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-loose"),
        mediumId = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-medium"),
        tightId = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-tight"),
        heepId = cms.InputTag("egmGsfElectronIDs:heepElectronID-HEEPV70"),
        vertexSrc = cms.InputTag("offlineSlimmedPrimaryVertices"),
        kinematicCutAnyVariation = cms.string('pt > 10 && abs(eta) < 2.5'),
        finalCut = cms.string('userInt("VetoId")'),
    )
