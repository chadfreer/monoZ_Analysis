#include "Analysis/monoZ/interface/NVtxDistribution.h"

ClassImp(NVtxDistribution)

void NVtxDistribution::SlaveBegin(TTree *)
{
  pileupSF = (ScaleFactor *) GetInputList()->FindObject("pileupSF");
  if ( pileupSF == nullptr ) Abort("NVtxDistribution needs Pileup Scale factors!");

  auto nvtxFlag = (TParameter<bool> *) GetInputList()->FindObject("nvtxPUreweighting");
  if ( nvtxFlag != nullptr ) nvtxPUreweighting = nvtxFlag->GetVal();
}

void NVtxDistribution::SetupNewDirectory()
{
  // Must call base class setup
  SelectorBase::SetupNewDirectory();

  AddObject<TH1D>(nvtx, "nvtx", "Number reco vertices;# Reco Vertices;Counts", 75, 0, 75);
  AddObject<TH2D>(nvtx_nTruePU, "nvtx_nTruePU", "Number reco vertices vs true PU;# Reco Vertices;Gen. Mixing nTruePU;Counts", 75, 0, 75, 75, 0, 75);
  AddObject<TH1D>(nvtxReweighted, "nvtxReweighted", "Number reco vertices reweighted;# Reco Vertices;Counts", 75, 0, 75);
  AddObject<TH1D>(nvtxSelection, "nvtxSelection", "Number reco vertices with selection;# Reco Vertices;Counts", 75, 0, 75);
  AddObject<TH2D>(nvtxSelection_nTruePU, "nvtxSelection_nTruePU", "Number reco vertices vs true PU with selection;# Reco Vertices;Gen. Mixing nTruePU;Counts", 75, 0, 75, 75, 0, 75);
  AddObject<TH1D>(nvtxSelectionReweighted, "nvtxSelectionReweighted", "Number reco vertices with selection, reweighted;# Reco Vertices;Counts", 75, 0, 75);

  AddObject<TH1D>(rho, "rho", "Avg energy density rho;#rho;Counts", 75, 0, 75);
  AddObject<TH2D>(rho_nTruePU, "rho_nTruePU", "Avg energy density rho vs true PU;#rho;Gen. Mixing nTruePU;Counts", 75, 0, 75, 75, 0, 75);
  AddObject<TH1D>(rhoReweighted, "rhoReweighted", "Avg energy density rho reweighted;#rho;Counts", 75, 0, 75);
  AddObject<TH1D>(rhoSelection, "rhoSelection", "Avg energy density rho with selection;#rho;Counts", 75, 0, 75);
  AddObject<TH2D>(rhoSelection_nTruePU, "rhoSelection_nTruePU", "Avg energy density rho vs true PU with selection;#rho;Gen. Mixing nTruePU;Counts", 75, 0, 75, 75, 0, 75);
  AddObject<TH1D>(rhoSelectionReweighted, "rhoSelectionReweighted", "Avg energy density rho with selection, reweighted;#rho;Counts", 75, 0, 75);
}

Bool_t NVtxDistribution::Process(Long64_t entry)
{
  using namespace EventTupleUtil;

  // Must call base class process (sets reader entry, counter)
  if ( ! SelectorBase::Process(entry) ) return false;

  double weight = 1.;
  if ( !isRealData() ) {
    weight *= (*genWeight > 0) ? 1. : -1.;
  }
  nvtx->Fill(*nPrimaryVerticesCut, weight);
  rho->Fill(*fixedGridRhoAll, weight);
  if ( !isRealData() ) nvtx_nTruePU->Fill(*nPrimaryVerticesCut, *nTruePileup, weight);
  if ( !isRealData() ) rho_nTruePU->Fill(*fixedGridRhoAll, *nTruePileup, weight);

  double pileupWeight = 1.;
  if ( !isRealData() ) {
    const double pileupValue = ( nvtxPUreweighting ) ? *nPrimaryVerticesCut : *nTruePileup;
    pileupWeight *= pileupSF->Evaluate1D(pileupValue);
  }
  nvtxReweighted->Fill(*nPrimaryVerticesCut, weight*pileupWeight);
  rhoReweighted->Fill(*fixedGridRhoAll, weight*pileupWeight);

  // Rough 2l selection
  // Choose tight-quality e/mu for event categorization
  std::vector<Lepton> goodLeptons;
  for (size_t i=0; i<muon_id.GetSize(); ++i) {
    LorentzVector p4 = muon_p4[i];
    int ids = muon_id[i];
    float iso = muon_relIsoDBetaR04[i];
    if ( p4.Pt() > 20 && fabs(p4.Eta()) < 2.4 && passes(ids, EventTuple::MuonTight) && iso < 0.15 ) {
      goodLeptons.emplace_back(p4, 13*muon_q[i]);
    }
  }
  for (size_t i=0; i<electron_id.GetSize(); ++i) {
    LorentzVector p4 = electron_p4[i];
    int ids = electron_id[i];
    if ( p4.Pt() > 20 && fabs(p4.Eta()) < 2.5 && passes(ids, EventTuple::ElectronMedium) ) {
      if ( overlaps(goodLeptons, p4, 0.01) ) {
        continue;
      }
      goodLeptons.emplace_back(p4, 11*electron_q[i]);
    }
  }

  if ( goodLeptons.size() == 2 ) {
    if ( false
        || ( goodLeptons[0].pdgId * goodLeptons[1].pdgId == -11*11 && hasTrigger(EventTuple::TriggerDoubleElectron) )
        || ( goodLeptons[0].pdgId * goodLeptons[1].pdgId == -13*13 && hasTrigger(EventTuple::TriggerDoubleMuon) )
       )
    {
      LorentzVector Zcand = goodLeptons[0].p4 + goodLeptons[1].p4;
      if ( abs(Zcand.M() - 91.) < 15 ) {
        nvtxSelection->Fill(*nPrimaryVerticesCut, weight);
        rhoSelection->Fill(*fixedGridRhoAll, weight);
        if ( !isRealData() ) nvtxSelection_nTruePU->Fill(*nPrimaryVerticesCut, *nTruePileup, weight);
        if ( !isRealData() ) rhoSelection_nTruePU->Fill(*fixedGridRhoAll, *nTruePileup, weight);
        nvtxSelectionReweighted->Fill(*nPrimaryVerticesCut, weight*pileupWeight);
        rhoSelectionReweighted->Fill(*fixedGridRhoAll, weight*pileupWeight);
      }
    }
  }

  return true;
}

