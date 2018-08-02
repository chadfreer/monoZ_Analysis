import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing
import re

# Define the CMSSW process
process = cms.Process("ANA")

options = VarParsing('analysis')
options.register(
    "isMC",
    True,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Treat as MC.  Ignored if crabDataset is specified or input file is a LFN (will deduce from name)"
)
options.register(
    "filterTriggers",
    False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Filter nTuples for trigger paths. Usually to shink file size for datasets we don't care about, like QCD"
)
options.register(
    "filterLeptons",
    True,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Filter nTuples for at least 1 e/mu/tau"
)
options.register(
    "filterPhotons",
    False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Filter nTuples for at least 1 photon"
)
options.register(
    "nLHEWeights",
    9,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.int,
    "If isMC, save vector of first N weights. Good choices: 9 for Scale, 111 for Scale+NNPDF (if NNPDF is first!)"
)
options.register(
    "genParticleCut",
    "isHardProcess",
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    "If isMC, only save genParticles passing this cut."
)
options.register(
    "crabDataset",
    "",
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    "If using CRAB, fill in dataset info since we cannot get it from the filename"
)
options.register(
    "gitversion",
    "unknown",
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    "Fill with `git describe` value when submitting to CRAB"
)
options.register(
    "debug",
    False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Increases process verbosity and saves EDM file (edmDebug.root) with all EventTupler input products"
)

options.parseArguments()

# Some test files
if len(options.inputFiles) == 0:
    if options.isMC:
        options.inputFiles = [
            '/store/mc/RunIISummer16MiniAODv2/ZZTo2L2Nu_13TeV_powheg_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/064F9BA2-D9C8-E611-914E-001E67E7195C.root',
            '/store/mc/RunIISummer16MiniAODv2/ZZTo2L2Nu_13TeV_powheg_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/142FCC25-70C7-E611-B494-20CF3027A5A3.root',
            '/store/mc/RunIISummer16MiniAODv2/ZZTo2L2Nu_13TeV_powheg_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/16B52F95-C0C6-E611-A6E4-002590D9D8A4.root',
            '/store/mc/RunIISummer16MiniAODv2/ZZTo2L2Nu_13TeV_powheg_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/181F17CD-57C7-E611-AEA0-141877638F39.root',
            '/store/mc/RunIISummer16MiniAODv2/ZZTo2L2Nu_13TeV_powheg_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/1E4531BD-DBC6-E611-9C9C-0025905AA9CC.root',
            '/store/mc/RunIISummer16MiniAODv2/ZZTo2L2Nu_13TeV_powheg_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/22BD56B7-96C5-E611-A32A-0025907DE22C.root',           
            '/store/mc/RunIISummer16MiniAODv2/ZZTo2L2Nu_13TeV_powheg_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/28442DC8-FDC6-E611-A7FC-0025905A612C.root',
            '/store/mc/RunIISummer16MiniAODv2/ZZTo2L2Nu_13TeV_powheg_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/287A6AAF-DFC6-E611-BA06-0CC47A7C357A.root',
            '/store/mc/RunIISummer16MiniAODv2/ZZTo2L2Nu_13TeV_powheg_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/2E83F857-22C6-E611-AA5C-0242AC130003.root',
            '/store/mc/RunIISummer16MiniAODv2/ZZTo2L2Nu_13TeV_powheg_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/3A2231F1-12C7-E611-A7F2-00266CFFC51C.root',
            '/store/mc/RunIISummer16MiniAODv2/ZZTo2L2Nu_13TeV_powheg_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/3A793D4A-C9C7-E611-82A2-0CC47A745294.root',
            '/store/mc/RunIISummer16MiniAODv2/ZZTo2L2Nu_13TeV_powheg_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/3CEB6105-C5C8-E611-B265-0025905A60F4.root',
            '/store/mc/RunIISummer16MiniAODv2/ZZTo2L2Nu_13TeV_powheg_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/42921CE1-22C8-E611-BCA3-20CF307C98DC.root',
            '/store/mc/RunIISummer16MiniAODv2/ZZTo2L2Nu_13TeV_powheg_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/4CA671ED-22C6-E611-8524-14187741120B.root',
            '/store/mc/RunIISummer16MiniAODv2/ZZTo2L2Nu_13TeV_powheg_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/529B831A-DBC4-E611-B616-001E674440E2.root',

            #'/store/mc/RunIISummer16MiniAODv2/ZZJJ_ZZTo2L2Nu_EWK_13TeV-madgraph-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/130000/12A322F3-19F6-E611-80F1-0CC47A1E0486.root',
            #'/store/mc/RunIISummer16MiniAODv2/ZZJJ_ZZTo2L2Nu_EWK_13TeV-madgraph-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/130000/0A3DAC22-1AF6-E611-B927-20474791CCC4.root',
            #'/store/mc/RunIISummer16MiniAODv2/ZZJJ_ZZTo2L2Nu_EWK_13TeV-madgraph-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/130000/46305389-19F6-E611-8915-44A842CFC97E.root',
            #'/store/mc/RunIISummer16MiniAODv2/ZZJJ_ZZTo2L2Nu_EWK_13TeV-madgraph-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/130000/524B006A-1EF6-E611-8B30-A0369FC5B844.root',
            #'/store/mc/RunIISummer16MiniAODv2/ZZJJ_ZZTo2L2Nu_EWK_13TeV-madgraph-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/130000/7C84B198-1CF6-E611-A6B8-FA163EE34286.root',
            #'/store/mc/RunIISummer16MiniAODv2/ZZJJ_ZZTo2L2Nu_EWK_13TeV-madgraph-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/130000/80AB2CE1-19F6-E611-8A97-02163E01A25F.root',
            #'/store/mc/RunIISummer16MiniAODv2/ZZJJ_ZZTo2L2Nu_EWK_13TeV-madgraph-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/130000/84687B0D-1AF6-E611-AAC0-842B2B768127.root',
            #'/store/mc/RunIISummer16MiniAODv2/ZZJJ_ZZTo2L2Nu_EWK_13TeV-madgraph-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/130000/AA79B3D9-1FF6-E611-8662-02163E00B8DE.root',
            #'/store/mc/RunIISummer16MiniAODv2/ZZJJ_ZZTo2L2Nu_EWK_13TeV-madgraph-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/130000/BA67B94F-19F6-E611-AE51-7845C4F932D8.root',
            #'/store/mc/RunIISummer16MiniAODv2/ZZJJ_ZZTo2L2Nu_EWK_13TeV-madgraph-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/130000/C8CBE011-1DF6-E611-8654-10983627C3C1.root',
            #'/store/mc/RunIISummer16MiniAODv2/ZZJJ_ZZTo2L2Nu_EWK_13TeV-madgraph-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/130000/EA9000C8-19F6-E611-9EF8-20CF3027A5E8.root',
            #'/store/mc/RunIISummer16MiniAODv2/ZZJJ_ZZTo2L2Nu_EWK_13TeV-madgraph-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/50000/2689ACCB-84ED-E611-8FD8-FA163E488B70.root',
            #'/store/mc/RunIISummer16MiniAODv2/ZZJJ_ZZTo2L2Nu_EWK_13TeV-madgraph-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/50000/3A1C1C6F-EEF1-E611-8380-02163E0140D8.root',
            #'/store/mc/RunIISummer16MiniAODv2/ZZJJ_ZZTo2L2Nu_EWK_13TeV-madgraph-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/50000/4CEC10DC-E9EE-E611-89D8-5065F382C2F1.root',
            #'/store/mc/RunIISummer16MiniAODv2/ZZJJ_ZZTo2L2Nu_EWK_13TeV-madgraph-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/50000/EA912C92-C7EC-E611-91BF-5065F382C251.root',
            #'/store/mc/RunIISummer16MiniAODv2/ZZJJ_ZZTo2L2Nu_EWK_13TeV-madgraph-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/50000/F65A658C-E7F1-E611-81E6-02163E0146F1.root', 
        ]
    else:
        options.inputFiles = [
            '/store/data/Run2016F/DoubleMuon/MINIAOD/03Feb2017-v1/100000/04B25C93-58EB-E611-86E4-002590FD5694.root',
        ]

# Load the standard set of configuration modules
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')

# Message Logger settings
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.destinations = ['cout', 'cerr']
process.MessageLogger.cerr.FwkReport.reportEvery = 1 if options.debug else 1000

# Set the process options -- Display summary at the end, enable unscheduled execution
process.options = cms.untracked.PSet( 
    allowUnscheduled = cms.untracked.bool(True),
    wantSummary = cms.untracked.bool(True if options.debug else False),
    numberOfThreads = cms.untracked.uint32(1),
    numberOfStreams = cms.untracked.uint32(1),
)

# How many events to process
process.maxEvents = cms.untracked.PSet( 
   input = cms.untracked.int32(options.maxEvents)
)

# Define the input source
process.source = cms.Source("PoolSource", 
    fileNames = cms.untracked.vstring(options.inputFiles)
)

# Figure out some dataset info
dataset = 'Unknown'
conditions = '??'
if options.crabDataset == "":
    # Try to guess the dataset from filename
    datasetRE = re.compile(".*/store.*/([^/]*)/([^/]*)/(MINIAODSIM|MINIAOD)/([^/]*).*root")
    m = datasetRE.match(process.source.fileNames[0])
    if m:
        (era, dataset, dataTier, condition) = m.groups()
        conditions = era + '-' + condition
else:
    (dataset, conditions, dataTier) = options.crabDataset.split('/')[1:]
    # Its a little annoying that this cannot be filled when using CRAB
    # since it evaluates the PSet at job submission
    process.source.fileNames = []

# Guess isMC if possible
if dataset != 'Unknown':
    if dataTier == 'MINIAOD':
        options.isMC = False
    elif dataTier == 'MINIAODSIM':
        options.isMC = True

# Choose a global tag
# Reference: https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideFrontierConditions
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
if options.isMC:
    if 'RunIISpring16' in conditions:
        process.GlobalTag.globaltag =  cms.string('80X_mcRun2_asymptotic_2016_miniAODv2_v1')
    elif 'RunIISummer16' in conditions:
        process.GlobalTag.globaltag =  cms.string('80X_mcRun2_asymptotic_2016_TrancheIV_v8')
    else:
        print "WARNING: could not guess the global tag from dataset!"
        process.GlobalTag.globaltag =  cms.string('80X_mcRun2_asymptotic_2016_miniAODv2_v1')
else:
    if 'PromptReco' in conditions:
        if '2016H' in conditions:
            process.GlobalTag.globaltag =  cms.string('80X_dataRun2_Prompt_v16')
        else:
            # ICHEP16 Prompt reco 2016[B-D]
            # Miniaod tag, not in reference?!        
            process.GlobalTag.globaltag =  cms.string('80X_dataRun2_Prompt_ICHEP16JEC_v0')
    elif '23Sep2016' in conditions:
        # Rereco for 2016[B-G]
        process.GlobalTag.globaltag =  cms.string('80X_dataRun2_2016SeptRepro_v7')
    elif '03Feb2017' in conditions:
        # Follow GT recommendation of https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMiniAOD#Run2016_03Feb2017_Re_miniAOD_80X
        if '2016H' in conditions:
            process.GlobalTag.globaltag =  cms.string('80X_dataRun2_Prompt_v16')
        else:
            process.GlobalTag.globaltag =  cms.string('80X_dataRun2_2016SeptRepro_v7')
    else:
        print "WARNING: could not guess the global tag from dataset!"
        process.GlobalTag.globaltag =  cms.string('80X_dataRun2_Prompt_v15')

# Take first vertex for granted
# https://indico.cern.ch/event/369417/contributions/1788757/attachments/734933/1008272/pv-sorting-xpog.pdf

from Analysis.monoZ.muonPrep import muonPrep
muonPrep(
    process,
    isMC=options.isMC,
    )

from Analysis.monoZ.electronPrep import electronPrep
electronPrep(
    process,
    isMC=options.isMC,
    useCalibration=True,
    )

from Analysis.monoZ.photonPrep import photonPrep
photonPrep(
    process,
    isMC=options.isMC,
    useCalibration=True,
    )

from Analysis.monoZ.tauPrep import tauPrep
tauPrep(
    process,
    isMC=options.isMC,
    )

from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD
runMetCorAndUncFromMiniAOD(
    process,
    isData=not options.isMC,
    electronColl = "preppedElectrons",
    photonColl = "preppedPhotons",
    muonColl = "preppedMuons",
    tauColl = "preppedTaus",
    )

from Analysis.monoZ.jetPrep import jetPrep
jetPrep(
    process,
    isMC=options.isMC,
    )

from Analysis.monoZ.metFilters import metFilters
extraMetFiltersPSet = metFilters(process, options.isMC)

from Analysis.monoZ.subMets import subMets
subMetInputsPSet = subMets(process, options.isMC)

#from PhysicsTools.PatAlgos.slimming.puppiForMET_cff import makePuppiesFromMiniAOD
#makePuppiesFromMiniAOD( process, True )
#runMetCorAndUncFromMiniAOD(process,
#     isData=(not options.isMC),
#     metType="Puppi",
#     postfix="Puppi"
#     )

# All PAT formats set up at this point
process.tuplePath = cms.Path()

#process.counters = cms.EDAnalyzer("EventCounters",
#    genEventInfoSrc = cms.InputTag("generator"),
#    lheEventSrc = cms.InputTag("externalLHEProducer"),
#    nLHEWeights = cms.uint32(options.nLHEWeights),
#    metaData = cms.PSet(
#        isMC = cms.string("Yes" if options.isMC else "No"),
#        dataset = cms.string(dataset),
#        conditions = cms.string(conditions),
#        globalTag = process.GlobalTag.globaltag,
#        inputFiles = cms.vstring(process.source.fileNames),
#        gitversion = cms.string(options.gitversion),
#    )
#)
#print repr(process.counters.metaData)
#process.tuplePath += process.counters

process.triggerFilter = cms.EDFilter("HLTHighLevel",
    TriggerResultsTag = cms.InputTag("TriggerResults::HLT2" if '_reHLT_' in conditions else "TriggerResults::HLT"),
    HLTPaths = cms.vstring(
        # All paths that we might want to look at in EventTupler::readTriggerBits()
        #  and/or paths to check minimum prescale for in e.g. photonPrep.py
        # Note that in readTriggerBits(), no * at the end, but here we do need the wildcard
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v*",
        "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v*",
        "HLT_IsoMu24_v*",
        "HLT_IsoTkMu24_v*",
        "HLT_Mu45_eta2p1_v*",
        "HLT_Mu50_v*",
        "HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v*",
        "HLT_Ele25_eta2p1_WPTight_Gsf_v*",
        "HLT_Ele27_WPTight_Gsf_v*",
        "HLT_Ele115_CaloIdVT_GsfTrkIdT_v*",
        "HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v*",
        "HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v*",
        "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v*",
        "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v*",
        "HLT_Photon50_R9Id90_HE10_IsoM_v*",
        "HLT_Photon75_R9Id90_HE10_IsoM_v*",
        "HLT_Photon90_R9Id90_HE10_IsoM_v*",
        "HLT_Photon120_R9Id90_HE10_IsoM_v*",
        "HLT_Photon165_HE10_v*",
        "HLT_Mu17_v*",
        "HLT_Ele23_CaloIdL_TrackIdL_IsoVL_v*",
    ),
    eventSetupPathsKey = cms.string(""),
    andOr = cms.bool(True), # OR mode
    throw = cms.bool(False), # Exepction if missing path name
)
if options.filterTriggers:
    process.tuplePath += process.triggerFilter

process.leptonCountFilter = cms.EDFilter("PATLeptonCountFilter",
    electronSource = cms.InputTag("preppedElectrons"),
    countElectrons = cms.bool(True),
    muonSource = cms.InputTag("preppedMuons"),
    countMuons = cms.bool(True),
    tauSource = cms.InputTag("preppedTaus"),
    countTaus = cms.bool(True),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
)
if options.filterLeptons:
    process.tuplePath += process.leptonCountFilter

process.photonCountFilter = cms.EDFilter("PATCandViewCountFilter",
    src = cms.InputTag("preppedPhotons"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
)
if options.filterPhotons:
    process.tuplePath += process.photonCountFilter

events = cms.EDAnalyzer("EventTupler",
    extraMetFiltersPSet,
    subMetInputsPSet,
    metSrc = cms.InputTag("slimmedMETs::ANA"),
    #altMetSrc = cms.InputTag("slimmedMETs::PAT" if options.isMC else "slimmedMETsMuEGClean::PAT"),
    altMetSrc = cms.InputTag("slimmedMETsPuppi::PAT"),
    uncleanedMetSrc = cms.InputTag("slimmedMETs::PAT" if options.isMC else "slimmedMETsUncorrected::PAT"),
    packedCandidatesSrc = cms.InputTag("packedPFCandidates"),
    electronSrc = cms.InputTag("preppedElectrons"),
    muonSrc = cms.InputTag("preppedMuons"),
    photonSrc = cms.InputTag("preppedPhotons"),
    tauSrc = cms.InputTag("preppedTaus"),
    jetSrc = cms.InputTag("preppedJets"),
    vertexSrc = cms.InputTag("offlineSlimmedPrimaryVertices"),
    fixedGridRhoAll = cms.InputTag("fixedGridRhoAll"),
    fixedGridRhoFastjetCentralCalo = cms.InputTag("fixedGridRhoFastjetCentralCalo"),
    genParticleSrc = cms.InputTag("prunedGenParticles"),
    genParticleCut = cms.string(options.genParticleCut.strip('"')),
    genEventInfoSrc = cms.InputTag("generator"),
    lheEventSrc = cms.InputTag("externalLHEProducer"),
    nLHEWeights = cms.uint32(options.nLHEWeights),
    pileupSrc = cms.InputTag("slimmedAddPileupInfo"),
    metFilterFlagsSrc = cms.InputTag("TriggerResults::PAT"),
    triggerBits = cms.InputTag("TriggerResults::HLT2" if '_reHLT_' in conditions else "TriggerResults::HLT"),
)
# process.tuplePath += process.events
# 
# process.TFileService = cms.Service(
#     "TFileService", fileName = cms.string(options.outputFile),
#     closeFileFast = cms.untracked.bool(True)
# )
# 

def makeKeepString(inputTag):
    module = inputTag.getModuleLabel()
    instance = inputTag.getProductInstanceLabel() or '*'
    process = inputTag.getProcessName() or '*'
    return 'keep *_%s_%s_%s' % (module, instance, process)

# Find and keep any product that is read by EventTupler
interestingProducers = filter(lambda (k,v): type(v) is cms.InputTag, events.parameters_().iteritems() )
namesToKeep = map(lambda (k,v): makeKeepString(v), interestingProducers)
# TODO: VInputTag support? For now, just subMets
print " -------------------- " 
print " names to keep : ", namesToKeep
print " producers     : ", interestingProducers
print " -------------------- " 
namesToKeep.append('keep *_subMET*__ANA')

process.edmOutput = cms.OutputModule(
    "PoolOutputModule",
    compressionLevel = cms.untracked.int32(4),
    compressionAlgorithm = cms.untracked.string('LZMA'),
    eventAutoFlushCompressedSize = cms.untracked.int32(15728640),
    outputCommands = cms.untracked.vstring(namesToKeep),
    fileName = cms.untracked.string('edmDebug.root'),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('')
        ),
    dropMetaData = cms.untracked.string('ALL'),
    fastCloning = cms.untracked.bool(False),
    overrideInputFileSplitLevels = cms.untracked.bool(True),
    )
process.outputStep = cms.EndPath(process.edmOutput)
