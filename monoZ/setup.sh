#!/bin/bash

SETUP_REMOTES=false

echo
echo "Welcome to the automagic setup script!"

if [ ! -f $CMSSW_BASE/src/.git/HEAD ];
then
  echo "CMSSW area appears not to be set up correctly. Check README carefully."
  echo
  return 1
fi

NFILES=`ls -1 ${CMSSW_BASE}/src | wc -l`
if [ ! ${NFILES} = "1" ]
then
  echo "CMSSW area appears to have extra files already. Start over and check README carefully."
  echo "You can remove this condition from the setup script if you wish, but proceed with caution!"
  echo
fi

cd $CMSSW_BASE/src/monoZ_Analysis
git remote rename origin upstream
#git remote set-url --push upstream DISALLOWED
GITHUBUSERNAME=`git config user.github`

echo "Setting up a new origin repo, assuming your fork name is ${GITHUBUSERNAME} - check this!"
git remote add origin git@github.com:${GITHUBUSERNAME}/monoZ_Analysis.git
git config branch.master.remote origin
git config merge.renamelimit 2000




cd $CMSSW_BASE/src

# MET filters & uncertainties
git cms-merge-topic -u cms-met:METRecipe_8020

# Summer16 Cut-based EG ID
git cms-merge-topic -u Sam-Harper:HEEPV70VID_8010_ReducedCheckout
git cms-merge-topic -u ikrav:egm_id_80X_v3
git cms-merge-topic -u ikrav:egm_id_80X_v3_photons

# EGM smearer
git cms-merge-topic -u gfasanel:Moriond17_23Jan
pushd EgammaAnalysis/ElectronTools/data
git clone git@github.com:/ScalesSmearings.git
popd

# The smearer git history makes some insanity, refresh everything
rm .git/info/sparse-checkout
git cms-sparse-checkout $CMSSW_VERSION HEAD
git read-tree -mu HEAD


