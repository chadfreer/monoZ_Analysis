#ifndef NVtxDistribution_h
#define NVtxDistribution_h

#include "Analysis/monoZ/interface/SelectorBase.h"
#include "Analysis/monoZ/interface/ScaleFactor.h"

class NVtxDistribution : public SelectorBase {
public :
  NVtxDistribution(TTree * /*tree*/ =0) { }
  virtual ~NVtxDistribution() { }
  virtual void   SlaveBegin(TTree *) override;
  virtual void   SetupNewDirectory() override;
  virtual Bool_t Process(Long64_t entry) override;

  ScaleFactor * pileupSF;

  bool nvtxPUreweighting{false};

  TH1D * nvtx;
  TH2D * nvtx_nTruePU;
  TH1D * nvtxReweighted;
  TH1D * nvtxSelection;
  TH2D * nvtxSelection_nTruePU;
  TH1D * nvtxSelectionReweighted;

  TH1D * rho;
  TH2D * rho_nTruePU;
  TH1D * rhoReweighted;
  TH1D * rhoSelection;
  TH2D * rhoSelection_nTruePU;
  TH1D * rhoSelectionReweighted;

  ClassDefOverride(NVtxDistribution,0);
};

#endif

