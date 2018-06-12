#ifndef MonoZSelector_h
#define MonoZSelector_h

#include "SelectorBase.h"
#include "ScaleFactor.h"
#include "CategorizedHist.h"
#include "BTagData.h"
#include "CondTools/BTau/interface/BTagCalibrationReader.h"
#include "DibosonCorrections.h"

class MonoZSelector : public SelectorBase {
public :
  MonoZSelector(TTree * /*tree*/ =0) { }
  virtual ~MonoZSelector() { }
  virtual void   SlaveBegin(TTree *) override;
  virtual void   SlaveTerminate() override;
  virtual void   SetupNewDirectory() override;
  virtual Bool_t Process(Long64_t entry) override;
  void ProcessSystematic(EventTupleUtil::Systematic systematic);

  ScaleFactor * pileupSF;
  ScaleFactor * electronIdSF;
  ScaleFactor * electronGsfSF;
  ScaleFactor * muonIdSF;
  ScaleFactor * muonIsoSF;
  ScaleFactor * btagEfficiency;

  BTagCalibrationReader * btagReader;
  DibosonCorrections * dibosonCorrector;

  bool skipSystematics{false};
  bool nvtxPUreweighting{false};
  bool printEventList{false};
  bool doBtagEfficiency{false};

  TH1D * weightsMonitor;
  enum WeightTypes {
    StartingWeight,
    METFilter,
    GenWeight,
    PileupWeight,
    KFactorWeight,
    BTagWeight,
    PostCategorize,
    LeadingLepID,
    TrailingLepID,
    NWeightTypes
  };

  TH1D * dyTauTauCutCheck;
  TH1D * dyStitchCheck;
  TH2D * vvKfactorCheck;

  TH1F * weirdStuffCounter;
  enum WeirdStuff {
    ElectronOverlapsMuon,
    NWeirdStuff
  };

  TH1D * triggerBits;

  TH1D * nAllLeptons;
  TH1D * nGoodLeptons;
  
  TH1D * nChadLeptons;

  TH2D * electronCorrection;
  TH1D * nExtraLeptons;
  TH2D * nLooseTightLeptons;
  TH2D * nLooseTightLeptonsWTau;

  TH1D * nGoodJets;
  TH1D * nBtagMediumJets;

  TH3D * btagEffNumerator;
  TH3D * btagEffDenominator;

  CategorizedHist1D * categories_systs;
  CategorizedHist1D * triggerBitsPostCategorize;
  CategorizedHist1D * nvtx;
  CategorizedHist1D * rho;
  CategorizedHist1D * leadingLepPtPreSF;
  CategorizedHist1D * leadingLepEtaPreSF;
  CategorizedHist1D * trailingLepPtPreSF;
  CategorizedHist1D * trailingLepEtaPreSF;

  CategorizedHist1D * triggerEfficiencyPostZpt;
  CategorizedHist1D * leadingLepPtPostZpt;
  CategorizedHist1D * trailingLepPtPostZpt;
  CategorizedHist1D * leadingLepPtPostZptWtrigger;
  CategorizedHist1D * trailingLepPtPostZptWtrigger;

  CategorizedHist2D * metVsUncleaned;
  CategorizedHist2D * metAltVsUncleaned;
  CategorizedHist2D * metVsAlt;
  CategorizedHist1D * metFiltersMuonBug;

  CategorizedHist1D * leadingLepPt;
  CategorizedHist1D * leadingLepEta;
  CategorizedHist1D * trailingLepPt;
  CategorizedHist1D * trailingLepEta;
  CategorizedHist1D * Zmass;
  CategorizedHist1D * Zpt;
  CategorizedHist1D * Zeta;
  CategorizedHist1D * Zrapidity;
  CategorizedHist2D * ZdeltaRllvsPt;
  CategorizedHist2D * pfMet_nJets;

  enum Cuts {
    passZmass,
    passBveto,
    passTauVeto,
    nJetsGood30,
    WZCRrealMETcut,
    WZCRm3lcut,
    WZCRm2lcut,
    ZZCRsecondZcut,
    ZZCRtightSecondZcut,
    passZpt,
    passMET,
    passDphiZMET,
    passBalanceCut,
    passDphiJetMET,
    passDeltaRll,
    passAll,
  };

  CategorizedHist1D * cutFlow;
  CategorizedHist1D * cutFlowUnweighted;

  CategorizedHist1D * cutFlow_Zmass;
  CategorizedHist1D * cutFlow_BVeto;
  CategorizedHist1D * cutFlow_TauVeto;
  CategorizedHist1D * cutFlow_nJets;
  CategorizedHist2D * xJet_xZ_jet20;
  CategorizedHist2D * xJet_xZ_jet30;
  CategorizedHist1D * cutFlow_realMETcutCR;
  CategorizedHist1D * cutFlow_secondZcutCR;
  CategorizedHist1D * multiLeptonMass;
  CategorizedHist1D * cutFlow_Zpt;
  CategorizedHist1D * metResponse;
  CategorizedHist1D * cutFlow_MET;
  CategorizedHist1D * cutFlow_dPhiZMET;
  CategorizedHist1D * cutFlow_balance;
  CategorizedHist1D * cutFlow_dPhiJetMET;
  CategorizedHist1D * cutFlow_deltaRll;

  CategorizedHist1D * nMinus1_Zmass;
  CategorizedHist1D * nMinus1_BVeto;
  CategorizedHist1D * nMinus1_TauVeto;
  CategorizedHist1D * nMinus1_nJets;
  CategorizedHist1D * nMinus1_nJets_xJet20;
  CategorizedHist1D * nMinus1_nJets_xJet30;
  CategorizedHist1D * nMinus1_realMETcutCR;
  CategorizedHist1D * nMinus1_secondZcutCR;
  CategorizedHist1D * nMinus1_Zpt;
  CategorizedHist1D * nMinus1_MET;
  CategorizedHist1D * nMinus1_dPhiZMET;
  CategorizedHist1D * nMinus1_balance;
  CategorizedHist1D * nMinus1_dPhiJetMET;
  CategorizedHist1D * nMinus1_deltaRll;

  CategorizedHist1D * nMinus2_dPhiZMET_balance_response;
  CategorizedHist1D * nMinus2_nJets_dPhiJetMET_xJet20;

  CategorizedHist1D * metCheck_dPhiJetMet_failBalance;
  CategorizedHist1D * metCheck_pfVsCaloMet_failBalance;
  CategorizedHist1D * metCheck_dPhiJetMet_failDphi;
  CategorizedHist1D * metCheck_pfVsCaloMet_failDphi;

  CategorizedHist2D * pfMet_systs;
  CategorizedHist2D * pfMet_lheWeights;
  CategorizedHist1D * realPfMet;
  CategorizedHist1D * genMetHist;
  CategorizedHist1D * triggerBitsFinal;
  TTree *preSelection;
  TTree * finalSelection;
  struct EventList {
    ULong_t run;
    ULong_t lumi;
    ULong_t event;
    int channel;
    float pfMet;
    float Zmass;
    ULong_t prerun;
    ULong_t prelumi;
    ULong_t preevent;
    int prechannel;
    float prepfMet;
    float preZmass;
    float Zpt_Reco;
    float DeltaPhiZMet;
    float Balance;
    //float DeltaRll;
    float DeltaPhill;
    float DeltaEtall;
    float ll_leadingLepPt;
    float ll_trailingLepPt;
    float ll_leadingLepEta;
    float ll_trailingLepEta;
    float ll_Zeta;
    float ll_Zrapidity;
    float weight;
    float ngoodjets;
    float jj_leadingPt;
    float jj_subleadingPt;
    float jj_leadingEta;
    float jj_subleadingEta;
    float dijet_mass;
    //float DeltaRjj;
    float DeltaPhijj;
    float DeltaEtajj;
    float Zeppenfeld; 
    int nBjets;
    float xJet; 
    float xZ; 
    int genBJets;
  };
  EventList eventList;

  TTree * unfoldingtest;
  struct ufd {
  float ptgen;
  float ptrec;
  int passSel;
  float weight;
  int PassMET;
  int PassDphiZMET;
  int PassBalanceCut;
  int PassDeltaRll;
  //float ll_leadingLepPt;
  //float ll_trailingLepPt;
  //float ll_leadingLepEta;
  //float ll_trailingLepEta;
  //float ll_Zmass;
  //float ll_Zeta;
  //float ll_Zrapidity;
  };
  ufd ufd;

  ClassDefOverride(MonoZSelector,0);
};

#endif

