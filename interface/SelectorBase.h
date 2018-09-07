#ifndef SelectorBase_h
#define SelectorBase_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TKey.h>
#include <TDirectory.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TH1D.h>
#include <TParameter.h>

#include "EventTupleUtil.h"


class SelectorBase : public TSelector {
public :
  // Flags are read from tuple file and kept up-to-date
  bool isMC_;
  inline bool isRealData() const { return !isMC_; };
  enum Dataset {
    MC_ggZZ,
    MC_qqZZ_powheg,
    MC_qqZZ,
    MC_WZ_powheg,
    MC_WZ,
    MC_DM_MonoZ,
    MC_ZH_HToInv,
    MC_DY_LOinclusive,
    MC_DY_NLOinclusive,
    MC_DY_NLOpt50to100,
    MC_DY_lowmass,
    MC_DY,
    MC,
    SingleMuon,
    SingleElectron,
    DoubleMuon,
    DoubleEG,
    MuonEG,
    SinglePhoton,
    Unknown,
  };
  Dataset primaryDataset_;
  inline bool isDataset(Dataset d) const { return primaryDataset_ == d; };
  inline bool hasTrigger(const EventTuple::TriggerBits bit) { return EventTupleUtil::passes(*triggers, bit); };

  // Gets the MET value from current event for a given MET uncertainty
  // (or SystNominal for any other systematic)
  // NB: Only implemented for a subset of relevant systematics
  // Do the lepton/photon systematics by hand!
  Vector2D getMet(EventTupleUtil::Systematic syst);

  // ------------ End of convenience functions

  TTreeReader    fReader;  //!the tree reader
  TTree       *fChain = 0;  //!pointer to the analyzed TTree or TChain

  TTreeReaderValue<ULong_t> run = {fReader, "run"};
  TTreeReaderValue<ULong_t> lumi = {fReader, "lumi"};
  TTreeReaderValue<ULong_t> event = {fReader, "event"};

  TTreeReaderArray<LorentzVector> genParticle_p4 = {fReader, "genParticle_p4"};
  TTreeReaderArray<int> genParticle_id = {fReader, "genParticle_id"};
  TTreeReaderArray<int> genParticle_flags = {fReader, "genParticle_flags"};

  TTreeReaderValue<float> genWeight = {fReader, "genWeight"};
  TTreeReaderArray<float> lheWeights = {fReader, "lheWeights"};
  TTreeReaderValue<float> lheSumPartonHT = {fReader, "lheSumPartonHT"};
  TTreeReaderValue<int> lheNpartons = {fReader, "lheNpartons"};
  TTreeReaderValue<float> lheZpt = {fReader, "lheZpt"};

  TTreeReaderValue<float> nTruePileup = {fReader, "nTruePileup"};
  TTreeReaderValue<float> nPoissonPileup = {fReader, "nPoissonPileup"};
  TTreeReaderValue<float> nOutOfTimePileup = {fReader, "nOutOfTimePileup"};
  TTreeReaderValue<float> nAheadOfTimePileup = {fReader, "nAheadOfTimePileup"};

  TTreeReaderValue<int> triggers = {fReader, "triggers"};

  TTreeReaderValue<Vector3D> vertex = {fReader, "vertex"};
  TTreeReaderValue<int> nPrimaryVertices = {fReader, "nPrimaryVertices"};
  TTreeReaderValue<int> nPrimaryVerticesCut = {fReader, "nPrimaryVerticesCut"};

  TTreeReaderValue<float> fixedGridRhoAll = {fReader, "fixedGridRhoAll"};
  TTreeReaderValue<float> fixedGridRhoFastjetCentralCalo = {fReader, "fixedGridRhoFastjetCentralCalo"};

  TTreeReaderValue<int> metFilters = {fReader, "metFilters"};
  TTreeReaderValue<float> metSignificance = {fReader, "metSignificance"};
  TTreeReaderValue<Matrix2D> metSigMatrixInverse = {fReader, "metSigMatrixInverse"};
  TTreeReaderValue<Vector2D> genMet = {fReader, "genMet"};
  TTreeReaderValue<Vector2D> caloMet = {fReader, "caloMet"};
  TTreeReaderValue<Vector2D> trackMet = {fReader, "trackMet"};
  TTreeReaderValue<Vector2D> rawPfMet = {fReader, "rawPfMet"};
  TTreeReaderArray<Vector2D> subMets = {fReader, "subMets"};
  TTreeReaderArray<float> subMetSumEts = {fReader, "subMetSumEts"};
  TTreeReaderValue<Vector2D> type1PfMet = {fReader, "type1PfMet"};
  TTreeReaderValue<float> type1PfSumEt = {fReader, "type1PfSumEt"};
  TTreeReaderValue<Vector2D> type1PfMet_JetResUp = {fReader, "type1PfMet_JetResUp"};
  TTreeReaderValue<Vector2D> type1PfMet_JetResDown = {fReader, "type1PfMet_JetResDown"};
  TTreeReaderValue<Vector2D> type1PfMet_JetEnUp = {fReader, "type1PfMet_JetEnUp"};
  TTreeReaderValue<Vector2D> type1PfMet_JetEnDown = {fReader, "type1PfMet_JetEnDown"};
  TTreeReaderValue<Vector2D> type1PfMet_UnclusteredEnUp = {fReader, "type1PfMet_UnclusteredEnUp"};
  TTreeReaderValue<Vector2D> type1PfMet_UnclusteredEnDown = {fReader, "type1PfMet_UnclusteredEnDown"};
  TTreeReaderValue<Vector2D> altPfMet = {fReader, "altPfMet"};
  TTreeReaderValue<Vector2D> altRawPfMet = {fReader, "altRawPfMet"};
  TTreeReaderValue<Vector2D> uncleanedPfMet = {fReader, "uncleanedPfMet"};
  TTreeReaderValue<int> nPFconstituents = {fReader, "nPFconstituents"};

  TTreeReaderArray<LorentzVector> electron_p4 = {fReader, "electron_p4"};
  TTreeReaderArray<float> electron_p4Error = {fReader, "electron_p4Error"};
  TTreeReaderArray<LorentzVector> electron_p4_raw = {fReader, "electron_p4_raw"};
  TTreeReaderArray<int> electron_q = {fReader, "electron_q"};
  TTreeReaderArray<int> electron_id = {fReader, "electron_id"};
  TTreeReaderArray<float> electron_dxy = {fReader, "electron_dxy"};
  TTreeReaderArray<float> electron_dz = {fReader, "electron_dz"};

  TTreeReaderArray<LorentzVector> muon_p4 = {fReader, "muon_p4"};
  TTreeReaderArray<float> muon_p4Error = {fReader, "muon_p4Error"};
  TTreeReaderArray<int> muon_q = {fReader, "muon_q"};
  TTreeReaderArray<int> muon_id = {fReader, "muon_id"};
  TTreeReaderArray<float> muon_relIsoDBetaR04 = {fReader, "muon_relIsoDBetaR04"};
  TTreeReaderArray<float> muon_dxy = {fReader, "muon_dxy"};
  TTreeReaderArray<float> muon_dz = {fReader, "muon_dz"};

  TTreeReaderArray<LorentzVector> photon_p4 = {fReader, "photon_p4"};
  TTreeReaderArray<float> photon_p4Error = {fReader, "photon_p4Error"};
  TTreeReaderArray<int> photon_id = {fReader, "photon_id"};
  TTreeReaderArray<float> photon_matchedTriggerEt = {fReader, "photon_matchedTriggerEt"};
  TTreeReaderArray<float> photon_matchedTriggerDeltaR = {fReader, "photon_matchedTriggerDeltaR"};
  TTreeReaderArray<int> photon_minPrescale = {fReader, "photon_minPrescale"};

  TTreeReaderArray<LorentzVector> tau_p4 = {fReader, "tau_p4"};
  TTreeReaderArray<float> tau_p4Error = {fReader, "tau_p4Error"};
  TTreeReaderArray<int> tau_q = {fReader, "tau_q"};
  TTreeReaderArray<int> tau_id = {fReader, "tau_id"};

  TTreeReaderArray<LorentzVector> jet_p4 = {fReader, "jet_p4"};
  TTreeReaderArray<LorentzVector> jet_p4_JetResUp = {fReader, "jet_p4_JetResUp"};
  TTreeReaderArray<LorentzVector> jet_p4_JetResDown = {fReader, "jet_p4_JetResDown"};
  TTreeReaderArray<LorentzVector> jet_p4_JetEnUp = {fReader, "jet_p4_JetEnUp"};
  TTreeReaderArray<LorentzVector> jet_p4_JetEnDown = {fReader, "jet_p4_JetEnDown"};
  TTreeReaderArray<int> jet_id = {fReader, "jet_id"};
  TTreeReaderArray<float> jet_btagCSVv2 = {fReader, "jet_btagCSVv2"};
  TTreeReaderArray<int> jet_hadronFlavor = {fReader, "jet_hadronFlavor"};

  // Paste from EventTuple.h, set bounds,
  // :23,95g/std::vect/norm ^dt<iTTreeReaderArray
  // :23,95v/\(TTreeR\|^ *$\)/norm ^diwiTTreeReaderValue<^R">
  // :23,95g/TTree/norm f;bywwi = {fReader, "^R""}

  SelectorBase(TTree * /*tree*/ =0) { }
  virtual ~SelectorBase() { }
  virtual Int_t  Version() const { return 2; }
  virtual void   Begin(TTree *tree);
  virtual void   SlaveBegin(TTree *tree);
  virtual void   Init(TTree *tree);
  virtual Bool_t Notify();
  virtual Bool_t Process(Long64_t entry);
  virtual Int_t  GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
  virtual void   SetOption(const char *option) { fOption = option; }
  virtual void   SetObject(TObject *obj) { fObject = obj; }
  virtual void   SetInputList(TList *input) { fInput = input; }
  virtual TList  *GetOutputList() const { return fOutput; }
  virtual void   SlaveTerminate();
  virtual void   Terminate();

  // Code to handle switching datasets

  // Output directory for current dataset
  TList *currentHistDir_{nullptr};
  // Counter
  TH1D *selectorCounter_{nullptr};
  // We'll collect pointers to objects from derived classes
  // as they are registered with AddObject, and update them to
  // the new object when a dataset is switched
  std::set<TNamed**> allObjects_;

  // Derived classes override (and call) this to register new objects
  // With AddObject<Type>(localPtr, ...);
  virtual void   SetupNewDirectory();

  template<typename T, typename... Args>
  void AddObject(T* &ptr, Args... args) {
    static_assert(std::is_base_of<TNamed, T>::value, "Objects must inheirit from ROOT TNamed to be streamable from PROOF sessions");
    ptr = new T(args...);
    ptr->SetDirectory(gROOT);
    currentHistDir_->Add(ptr);
    allObjects_.insert((TNamed**) &ptr);
  };

  void   UpdateDirectory();

  ClassDef(SelectorBase,0);
};

#endif
