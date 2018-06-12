#include "Analysis/monoZ/interface/SelectorBase.h"

ClassImp(SelectorBase)

Vector2D SelectorBase::getMet(EventTupleUtil::Systematic syst)
{
  using namespace EventTupleUtil;

  Vector2D met = *altPfMet;

  switch (syst) {
    case SystNominal:
      return met;
    case SystJetResUp:
      return met + *type1PfMet_JetResUp;
    case SystJetResDown:
      return met + *type1PfMet_JetResDown;
    case SystJetEnUp:
      return met + *type1PfMet_JetEnUp;
    case SystJetEnDown:
      return met + *type1PfMet_JetEnDown;
    case SystUnclusteredEnUp:
      return met + *type1PfMet_UnclusteredEnUp;
    case SystUnclusteredEnDown:
      return met + *type1PfMet_UnclusteredEnDown;
    default:
      return met;
  }
}

// End of convenience functions ---------

void SelectorBase::Init(TTree *tree)
{
  fReader.SetTree(tree);

  auto currentFile = fReader.GetTree()->GetDirectory()->GetFile();

  auto datasetFolder = dynamic_cast<TDirectoryFile*>(currentFile->Get("counters/dataset"));
  if ( datasetFolder == nullptr ) Error("SelectorBase", "Could not find counters/dataset folder in current file");
  auto datasetKey = dynamic_cast<TKey*>(datasetFolder->GetListOfKeys()->First());
  if ( datasetKey == nullptr ) Error("SelectorBase", "Could not load dataset key from current file");
  TString dataset = datasetKey->GetName();

  auto ismcFlagFolder = dynamic_cast<TDirectoryFile*>(currentFile->Get("counters/isMC"));
  if ( ismcFlagFolder == nullptr ) Error("SelectorBase", "Could not find counters/isMC folder in current file");
  auto ismcFlagKey = dynamic_cast<TKey*>(ismcFlagFolder->GetListOfKeys()->First());
  if ( ismcFlagKey == nullptr ) Error("SelectorBase", "Could not load mc flag key from current file");
  if ( ismcFlagKey->GetName() == TString("Yes") ) isMC_ = true;
  else isMC_ = false;

  if ( dataset.Contains("GluGluToContinToZZTo") ) primaryDataset_ = Dataset::MC_ggZZ;
  else if ( dataset.Contains("ZZTo") && dataset.Contains("powheg") ) primaryDataset_ = Dataset::MC_qqZZ_powheg;
  else if ( dataset.Contains("ZZTo") ) primaryDataset_ = Dataset::MC_qqZZ;
  else if ( dataset.Contains("WZTo") && dataset.Contains("powheg") ) primaryDataset_ = Dataset::MC_WZ_powheg;
  else if ( dataset.Contains("WZTo") ) primaryDataset_ = Dataset::MC_WZ;
  else if ( dataset.Contains("DarkMatter_MonoZToLL") ) primaryDataset_ = Dataset::MC_DM_MonoZ;
  else if ( dataset.Contains("HToInv") && dataset.Contains("ZH") ) primaryDataset_ = Dataset::MC_ZH_HToInv;
  else if ( dataset == "DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8" ) primaryDataset_ = Dataset::MC_DY_LOinclusive;
  else if ( dataset == "DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8" ) primaryDataset_ = Dataset::MC_DY_NLOinclusive;
  else if ( dataset == "DYJetsToLL_Pt-50To100_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8" ) primaryDataset_ = Dataset::MC_DY_NLOpt50to100;
  else if ( dataset == "DYJetsToLL_M-10to50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8" ) primaryDataset_ = Dataset::MC_DY_lowmass;
  else if ( dataset.Contains("DYJetsToLL") ) primaryDataset_ = Dataset::MC_DY;
  else if ( isMC_ ) primaryDataset_ = Dataset::MC;
  else if ( dataset == "SingleMuon" ) primaryDataset_ = Dataset::SingleMuon;
  else if ( dataset == "SingleElectron" ) primaryDataset_ = Dataset::SingleElectron;
  else if ( dataset == "DoubleMuon" ) primaryDataset_ = Dataset::DoubleMuon;
  else if ( dataset == "DoubleEG" ) primaryDataset_ = Dataset::DoubleEG;
  else if ( dataset == "MuonEG" ) primaryDataset_ = Dataset::MuonEG;
  else if ( dataset == "SinglePhoton" ) primaryDataset_ = Dataset::SinglePhoton;
  else primaryDataset_ = Dataset::Unknown;

  currentHistDir_ = dynamic_cast<TList*>(fOutput->FindObject(dataset));
  if ( currentHistDir_ == nullptr ) {
    currentHistDir_ = new TList();
    currentHistDir_->SetName(dataset);
    fOutput->Add(currentHistDir_);
    // Watch for something that I hope never happens,
    size_t existingObjectPtrsSize = allObjects_.size();
    SetupNewDirectory();
    if ( existingObjectPtrsSize > 0 && allObjects_.size() != existingObjectPtrsSize ) {
      Abort(Form("SelectorBase: Size of allObjects has changed!: %lu to %lu", existingObjectPtrsSize, allObjects_.size()));
    }
  }
  UpdateDirectory();
}

Bool_t SelectorBase::Notify()
{
  return kTRUE;
}

void SelectorBase::Begin(TTree * /*tree*/)
{
}

void SelectorBase::SlaveBegin(TTree * /*tree*/)
{
}

Bool_t SelectorBase::Process(Long64_t entry)
{
   fReader.SetEntry(entry);

   selectorCounter_->Fill(0);

   return kTRUE;
}

void SelectorBase::SlaveTerminate()
{
}

void SelectorBase::Terminate()
{
}

void SelectorBase::SetupNewDirectory()
{
  AddObject<TH1D>(selectorCounter_, "selectorCounter", "Counter of SelectorBase::Process() calls", 1, 0, 1);
}

void SelectorBase::UpdateDirectory()
{
  for(TNamed** objPtrPtr : allObjects_) {
    if ( *objPtrPtr == nullptr ) Abort("SelectorBase: Call to UpdateObject but existing pointer is null");
    *objPtrPtr = (TNamed *) currentHistDir_->FindObject((*objPtrPtr)->GetName());
    if ( *objPtrPtr == nullptr ) Abort("SelectorBase: Call to UpdateObject but current directory has no instance");
  }
}
