# Getting started

Checkout a CMSSW version and get the monoZ_Analysis code
```bash
cmsrel CMSSW_8_0_26_patch1
cd CMSSW_8_0_26_patch1/src
cmsenv
git cms-init
git clone git@github.com:NEUAnalyses/monoZ_Analysis.git
source monoZ_Analysis/setup.sh
```
You will still need to compile 
```bash 
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
