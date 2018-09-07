from CRABClient.UserUtilities import config
import ConfigParser
import hashlib
import os
import re
import subprocess
import sys

settingsFile = "local.txt"
if not os.path.exists(settingsFile):
    print "Please copy local.template.txt to local.txt and edit as appropriate"
    exit()
localSettings = ConfigParser.ConfigParser()
localSettings.read(settingsFile)

gitDescription = subprocess.check_output(["git", "describe", "--always"]).strip()
gitStatus = subprocess.check_output(["git", "status", "--porcelain", "-uno"])
if gitStatus != "":
    print "\033[33mWARNING: git status is dirty!\033[0m"
    print gitStatus
    gitDescription += "*"

# We have to hack our way around how crab parses command line arguments :<
dataset = 'dummy'
for arg in sys.argv:
    if 'Data.inputDataset=' in arg:
        dataset = arg.split('=')[1]
if dataset == 'dummy':
    raise Exception("Must pass dataset argument as Data.inputDataset=...")

(_, primaryDS, conditions, dataTier) = dataset.split('/')
if dataTier == 'MINIAOD':
    isMC = 0
elif dataTier == 'MINIAODSIM':
    isMC = 1
else:
    raise Exception("Dataset malformed? Couldn't deduce isMC parameter")

def getUnitsPerJob(ds):
    if 'DYJets' in ds:
        return 5
    elif isMC == 0:
        # Data is split by lumisection
        # The difference is due to trigger rates
        if 'Double' in ds:
            return 200
        elif 'MuonEG' in ds:
            return 400
        else:
            return 200
    else:
        return 10

config = config()
config.Data.inputDataset = dataset
config.Data.outputDatasetTag = conditions
configParams = ['isMC=%d' % isMC, 'crabDataset='+dataset, 'gitversion="%s"' % gitDescription]

if isMC:
    config.General.requestName = '_'.join([localSettings.get("local", "campaign"), primaryDS])
    # Check for extension dataset, force unique request name
    m = re.match(r".*(_ext[0-9]*)-", conditions)
    if m:
        config.General.requestName += m.groups()[0]
    config.Data.splitting = 'FileBased'
    config.Data.unitsPerJob = getUnitsPerJob(primaryDS)
    if 'ZZTo' in primaryDS or 'WZTo' in primaryDS or 'DarkMatter' in primaryDS or 'HToInv' in primaryDS or 'ADDMonoZ' in primaryDS or 'Unpart' in primaryDS:
        configParams.append('nLHEWeights=111')
        configParams.append('genParticleCut="isHardProcess || isPromptFinalState || isDirectPromptTauDecayProductFinalState"')
    if 'DYJetsToLL' in primaryDS:
        configParams.append('genParticleCut="isHardProcess || isDirectPromptTauDecayProductFinalState"')
    if '_reHLT_' in conditions and 'QCD' in primaryDS:
        configParams.append('filterTriggers=1')
    if 'GJets' in primaryDS or 'QCD_HT' in primaryDS or 'WJetsToLNu_HT' in primaryDS or 'WGToLNuG' in primaryDS or 'ZGTo2LG' in primaryDS:
        configParams.append('filterLeptons=0')
        configParams.append('filterPhotons=1')
else:
    # Since a PD will have several eras, add conditions to name to differentiate
    config.General.requestName = '_'.join([localSettings.get("local", "campaign"), primaryDS, conditions])
    config.Data.lumiMask ='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/ReReco/Final/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt'
    config.Data.splitting = 'LumiBased'
    config.Data.unitsPerJob = getUnitsPerJob(primaryDS)
    configParams.append('filterTriggers=1')
    if 'SinglePhoton' in primaryDS:
        configParams.append('filterLeptons=0')
        configParams.append('filterPhotons=1')

# Max requestName is 100 characters
if len(config.General.requestName) > 100:
    bits = 5
    h = hashlib.sha256(config.General.requestName).hexdigest()
    config.General.requestName = config.General.requestName[:(100-bits)] + h[:bits]

config.JobType.pyCfgParams = configParams

# Things that don't change with dataset
config.General.workArea = '.'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'ANALYSIS'
config.JobType.psetName = '../test/eventTuples.py'
config.JobType.numCores = 1

config.Data.inputDBS = 'global'
config.Data.useParent = False
config.Data.publication = False
config.Data.outLFNDirBase = localSettings.get("local", "outLFNDirBase")
config.Data.ignoreLocality = False

config.Site.storageSite = localSettings.get("local", "storageSite")
