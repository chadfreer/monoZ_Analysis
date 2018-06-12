#ifndef METResponse_h
#define METResponse_h

#include "Analysis/monoZ/interface/SelectorBase.h"
#include "Analysis/monoZ/interface/ScaleFactor.h"
#include "Analysis/monoZ/interface/CategorizedHist.h"

#include "TMath.h"

class METResponse : public SelectorBase {
public :
  METResponse(TTree * /*tree*/ =0) { }
  virtual ~METResponse() { }
  virtual void   SlaveBegin(TTree *) override;
  virtual void   SetupNewDirectory() override;
  virtual Bool_t Process(Long64_t entry) override;
  void ProcessSystematic(EventTupleUtil::Systematic systematic);

  ScaleFactor * pileupSF;
  ScaleFactor * electronIdSF;
  ScaleFactor * electronGsfSF;
  ScaleFactor * muonIdSF;
  ScaleFactor * muonIsoSF;

  bool skipSystematics{false};
  bool nvtxPUreweighting{false};

  TH1D * weightsMonitor;
  enum WeightTypes {
    StartingWeight,
    METFilter,
    GenWeight,
    PileupWeight,
    PostCategorize,
    LeadingLepID,
    TrailingLepID,
    NWeightTypes
  };

  TH1F * weirdStuffCounter;
  enum WeirdStuff {
    ElectronOverlapsMuon,
    NWeirdStuff
  };

  TH1D * triggerBits;
  TH1D * nvtxRaw;
  TH1D * nvtxWeight;

  TH1D * nAllLeptons;
  TH1D * nGoodLeptons;
  TH1D * nExtraLeptons;
  TH2D * nLooseTightLeptons;

  CategorizedHist1D * categories_systs;
  CategorizedHist1D * noSeed;
  CategorizedHist1D * triggerBitsPostCategorize;
  CategorizedHist1D * nvtx;
  CategorizedHist1D * leadingLepPtPreSF;
  CategorizedHist1D * leadingLepEtaPreSF;
  CategorizedHist1D * trailingLepPtPreSF;
  CategorizedHist1D * trailingLepEtaPreSF;
  CategorizedHist1D * leadingLepPt;
  CategorizedHist1D * leadingLepEta;
  CategorizedHist1D * trailingLepPt;
  CategorizedHist1D * trailingLepEta;
  CategorizedHist1D * Zmass;
  CategorizedHist1D * Zpt;
  CategorizedHist1D * Zeta;
  CategorizedHist1D * Zrapidity;

  CategorizedHist2D * pfMet_systs;
  CategorizedHist2D * pfMetResponse_systs;
  CategorizedHist2D * pfMetResolution_systs;
  CategorizedHist2D * metSignificance_systs;
  CategorizedHist2D * metSigChi2_systs;

  CategorizedHist2D * pfMet_nvtx;
  CategorizedHist2D * pfMet_nJets;
  CategorizedHist2D * metSignificance_nJets;
  CategorizedHist1D * genMetHist;
  CategorizedHist1D * ZptMcut;
  CategorizedHist1D * ZetaMcut;
  CategorizedHist1D * ZrapidityMcut;

  ClassDefOverride(METResponse,0);
};

#endif

