set CMSSW_PROJECT_SRC="/CMSSW_8_0_26_patch1/src/"
set CFG_FILE="selector/runSelector"
set OUTPUT_FILE="MonoZSelector-demoFiles.root"
set TOP="$PWD"

cd /afs/cern.ch/user/c/cfreer/Mono_Z/apr2/CMSSW_8_0_26_patch1/src/Analysis/monoZ/selector
eval `scramv1 runtime -csh`
cd $TOP
./runSelector --inputs scaleFactors.root MonoZSelector demoFiles.txt
rfcp MonoZSelector-demoFiles.root /castor/cern.ch/user/c/cfreer/$OUTPUT_FILE
