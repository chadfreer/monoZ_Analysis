#ifndef PhotonSelector_h
#define PhotonSelector_h

#include "Analysis/monoZ/interface/SelectorBase.h"
#include "Analysis/monoZ/interface/ScaleFactor.h"

class PhotonSelector : public SelectorBase {
public :
  PhotonSelector(TTree * /*tree*/ =0) { }
  virtual ~PhotonSelector() { }
  virtual void   SlaveBegin(TTree *) override;
  virtual void   SetupNewDirectory() override;
  virtual Bool_t Process(Long64_t entry) override;
  void ProcessSystematic(EventTupleUtil::Systematic systematic);

  ScaleFactor * pileupSF;
  ScaleFactor * photonIdSF;

  bool skipSystematics{false};
  bool nvtxPUreweighting{false};

  TH1D * weightsMonitor;
  enum WeightTypes {
    StartingWeight,
    METFilter,
    GenWeight,
    PileupWeight,
    KFactorWeight,
    PrescaleWeight,
    PhotonID,
    NWeightTypes
  };

  TH1F * weirdStuffCounter;
  enum WeirdStuff {
    NWeirdStuff
  };

  TH1D * triggerDeltaR;
  TH1D * nAllLeptons;
  TH1D * nGoodJets;
  TH1D * nBtagMediumJets;

  TH1D * nvtx;
  TH1D * photonPtPreSF;
  TH1D * photonEtaPreSF;
  TH1D * photonPt;
  TH1D * photonEta;

  TH1D * oldAnalyzerCutflow;
  TH1D * oldAnalyzerCutflowUnweighted;

  TH2D * pfMet_systs;
  TH1D * realPfMet;
  TH1D * genMetHist;
  TTree * finalSelection;
  struct EventList {
    ULong_t run;
    ULong_t lumi;
    ULong_t event;
    int channel;
    float pfMet;
    float Zmass;
  };
  EventList eventList;

  ClassDefOverride(PhotonSelector,0);
};

#endif
