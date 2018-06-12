Recipe:
```bash
cmsrel CMSSW_8_0_26_patch1
cd CMSSW_8_0_26_patch1/src
cmsenv
git cms-init

# If you don't plan to submit nTuples from this work area, feel free to skip to `mkdir Analysis`

# MET filters & uncertainties
git cms-merge-topic -u cms-met:METRecipe_8020

# Summer16 Cut-based EG ID
git cms-merge-topic -u Sam-Harper:HEEPV70VID_8010_ReducedCheckout
git cms-merge-topic -u ikrav:egm_id_80X_v3
git cms-merge-topic -u ikrav:egm_id_80X_v3_photons

# EGM smearer
git cms-merge-topic -u gfasanel:Moriond17_23Jan
pushd EgammaAnalysis/ElectronTools/data
git clone git@github.com:gfasanel/ScalesSmearings.git
popd

# The smearer git history makes some insanity, refresh everything
rm .git/info/sparse-checkout
git cms-sparse-checkout $CMSSW_VERSION HEAD
git read-tree -mu HEAD

mkdir Analysis && cd Analysis
# Or, kerberos via
# git clone https://:@gitlab.cern.ch:8443/ncsmith/monoZ.git
git clone ssh://git@gitlab.cern.ch:7999/ncsmith/monoZ.git
cd $CMSSW_BASE/src
scram b -j 12
```

5 minute demo (not counting tuple time):
```bash
cd $CMSSW_BASE/src/Analysis/monoZ/test
cmsRun eventTuples.py outputFile=demo.root
cmsRun eventTuples.py inputFiles_load=ttbar.txt maxEvents=10000 outputFile=demo_ttbar.root
cd ../selector
echo "../test/demo.root" > demoFiles.txt
echo "../test/demo_ttbar_numEvent10000.root" >> demoFiles.txt
wget http://www.hep.wisc.edu/~nsmith/monoZ/scaleFactors.root
./runSelector --inputs scaleFactors.root MonoZSelector demoFiles.txt
root -l demo.root
```

Higgs combine recipe:
(check https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideHiggsAnalysisCombinedLimit for updates)
```bash
export SCRAM_ARCH=slc6_amd64_gcc491
cmsrel CMSSW_7_4_7
cd CMSSW_7_4_7/src
cmsenv
git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit
cd HiggsAnalysis/CombinedLimit
git fetch origin
git checkout v6.3.0
bash <(curl -s https://raw.githubusercontent.com/cms-analysis/CombineHarvester/master/CombineTools/scripts/sparse-checkout-ssh.sh)
scram b -j 8
```