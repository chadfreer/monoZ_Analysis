#include "Analysis/monoZ/interface/PhotonSelector.h"

ClassImp(PhotonSelector)

// Constants and functions local to this selector
namespace {
  constexpr double nominalZmass{91.1876};
}

void PhotonSelector::SlaveBegin(TTree *)
{
  pileupSF = (ScaleFactor *) GetInputList()->FindObject("pileupSF");
  if ( pileupSF == nullptr ) Abort("PhotonSelector needs Pileup Scale factors!");

  photonIdSF = (ScaleFactor *) GetInputList()->FindObject("photonIdSF");
  //TODO: if ( photonIdSF == nullptr ) Abort("PhotonSelector needs photon ID Scale factors!");

  auto systFlag = (TParameter<bool> *) GetInputList()->FindObject("skipSystematics");
  if ( systFlag != nullptr ) skipSystematics = systFlag->GetVal();

  auto nvtxFlag = (TParameter<bool> *) GetInputList()->FindObject("nvtxPUreweighting");
  if ( nvtxFlag != nullptr ) nvtxPUreweighting = nvtxFlag->GetVal();
}

void PhotonSelector::SetupNewDirectory()
{
  // Must call base class setup
  SelectorBase::SetupNewDirectory();

  AddObject<TH1D>(weightsMonitor, "weightsMonitor", "Event weight multipliers", WeightTypes::NWeightTypes, 0, WeightTypes::NWeightTypes);
  weightsMonitor->GetXaxis()->SetBinLabel(1+StartingWeight, "StartingWeight");
  weightsMonitor->GetXaxis()->SetBinLabel(1+METFilter,      "METFilter");
  weightsMonitor->GetXaxis()->SetBinLabel(1+GenWeight,      "GenWeight/Deduplicate");
  weightsMonitor->GetXaxis()->SetBinLabel(1+PileupWeight,   "PileupWeight");
  weightsMonitor->GetXaxis()->SetBinLabel(1+KFactorWeight,  "KFactorWeight");
  weightsMonitor->GetXaxis()->SetBinLabel(1+PrescaleWeight, "PrescaleWeight");
  weightsMonitor->GetXaxis()->SetBinLabel(1+PhotonID,       "PhotonID");

  AddObject<TH1F>(weirdStuffCounter, "weirdStuffCounter", "Counters for things that are unlikely", WeirdStuff::NWeirdStuff, 0, WeirdStuff::NWeirdStuff);

  AddObject<TH1D>(triggerDeltaR, "triggerDeltaR", "DR trigger-photon;#DeltaR(#gamma_{trg},#gamma_{reco});Counts", 100, 0., 1.);
  AddObject<TH1D>(nAllLeptons, "nAllLeptons", "Number of leptons stored in tuple;# Skimmed Leptons;Counts", 6, -0.5, 5.5);
  AddObject<TH1D>(nGoodJets, "nGoodJets", "Number selected jets;# Selected Jets;Counts", 6, -0.5, 5.5);
  AddObject<TH1D>(nBtagMediumJets, "nBtagMediumJets", "Number b-tagged jets;# CSVv2 Medium B-tagged Jets;Counts", 6, -0.5, 5.5);

  AddObject<TH1D>(nvtx, "nvtx", "Number reco vertices;# Reco Vertices;Counts", 70, 0, 70);
  AddObject<TH1D>(photonPtPreSF, "photonPtPreSF", "Leading Photon Pre-ScaleFactor;Leading Photon p_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<TH1D>(photonEtaPreSF, "photonEtaPreSF", "Leading Photon Pre-ScaleFactor;Leading Photon #eta;Counts / 0.1", 50, -2.5, 2.5);
  AddObject<TH1D>(photonPt, "photonPt", "Leading Photon;Leading Photon p_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<TH1D>(photonEta, "photonEta", "Leading Photon;Leading Photon #eta;Counts / 0.1", 50, -2.5, 2.5);

  AddObject<TH1D>(oldAnalyzerCutflow, "oldAnalyzerCutflow", "Old Analyzer Cutflow", 13, 0, 13);
  oldAnalyzerCutflow->GetXaxis()->SetBinLabel(1+ 0, "Trigger+Category");
  oldAnalyzerCutflow->GetXaxis()->SetBinLabel(1+ 1, "pass3dLeptonVeto");
  oldAnalyzerCutflow->GetXaxis()->SetBinLabel(1+ 2, "passBveto");
  oldAnalyzerCutflow->GetXaxis()->SetBinLabel(1+ 3, "passTauVeto");
  oldAnalyzerCutflow->GetXaxis()->SetBinLabel(1+ 4, "nJetsGood30");
  oldAnalyzerCutflow->GetXaxis()->SetBinLabel(1+ 5, "passZmass");
  oldAnalyzerCutflow->GetXaxis()->SetBinLabel(1+ 6, "passZpt");
  oldAnalyzerCutflow->GetXaxis()->SetBinLabel(1+ 7, "passMET");
  oldAnalyzerCutflow->GetXaxis()->SetBinLabel(1+ 8, "passDphiZMET");
  oldAnalyzerCutflow->GetXaxis()->SetBinLabel(1+ 9, "passBalanceCut");
  oldAnalyzerCutflow->GetXaxis()->SetBinLabel(1+10, "passDphiJetMET");
  oldAnalyzerCutflow->GetXaxis()->SetBinLabel(1+11, "realMETcutCR");

  AddObject<TH1D>(oldAnalyzerCutflowUnweighted, "oldAnalyzerCutflowUnweighted", "Old Analyzer Cutflow, Raw Counts", 13, 0, 13);
  oldAnalyzerCutflowUnweighted->GetXaxis()->SetBinLabel(1+ 0, "Trigger+Category");
  oldAnalyzerCutflowUnweighted->GetXaxis()->SetBinLabel(1+ 1, "pass3dLeptonVeto");
  oldAnalyzerCutflowUnweighted->GetXaxis()->SetBinLabel(1+ 2, "passBveto");
  oldAnalyzerCutflowUnweighted->GetXaxis()->SetBinLabel(1+ 3, "passTauVeto");
  oldAnalyzerCutflowUnweighted->GetXaxis()->SetBinLabel(1+ 4, "nJetsGood30");
  oldAnalyzerCutflowUnweighted->GetXaxis()->SetBinLabel(1+ 5, "passZmass");
  oldAnalyzerCutflowUnweighted->GetXaxis()->SetBinLabel(1+ 6, "passZpt");
  oldAnalyzerCutflowUnweighted->GetXaxis()->SetBinLabel(1+ 7, "passMET");
  oldAnalyzerCutflowUnweighted->GetXaxis()->SetBinLabel(1+ 8, "passDphiZMET");
  oldAnalyzerCutflowUnweighted->GetXaxis()->SetBinLabel(1+ 9, "passBalanceCut");
  oldAnalyzerCutflowUnweighted->GetXaxis()->SetBinLabel(1+10, "passDphiJetMET");
  oldAnalyzerCutflowUnweighted->GetXaxis()->SetBinLabel(1+11, "realMETcutCR");

  AddObject<TH2D>(pfMet_systs, "pfMet_systs", "PF MET; PF #slash{E}_{T} [GeV];Systematic;Counts / 5 GeV", 100, 0, 500, EventTupleUtil::NSystematics, 0, EventTupleUtil::NSystematics);
  for(int syst=EventTupleUtil::SystNominal; syst<EventTupleUtil::NSystematics; ++syst) {
    pfMet_systs->GetYaxis()->SetBinLabel(1+syst, EventTupleUtil::SystematicNames.at(static_cast<EventTupleUtil::Systematic>(syst)));
  }
  AddObject<TH1D>(realPfMet, "realPfMet", "PF MET; PF #slash{E}_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<TH1D>(genMetHist, "genMetHist", "Generator MET;Generator #slash{E}_{T} [GeV];Counts / 5 GeV", 100, 0, 500);

  AddObject<TTree>(finalSelection, "finalSelection", "Final selection");
  finalSelection->Branch("run", &eventList.run, "run/l");
  finalSelection->Branch("lumi", &eventList.lumi, "lumi/l");
  finalSelection->Branch("event", &eventList.event, "event/l");
  finalSelection->Branch("channel", &eventList.channel);
  finalSelection->Branch("pfMet", &eventList.pfMet);
  finalSelection->Branch("Zmass", &eventList.Zmass);
}

Bool_t PhotonSelector::Process(Long64_t entry)
{
  // Must call base class process (sets reader entry, counter)
  if ( ! SelectorBase::Process(entry) ) return false;

  if ( !skipSystematics ) {
    for(int syst=EventTupleUtil::SystNominal; syst<EventTupleUtil::NSystematics; ++syst) {
      ProcessSystematic(static_cast<EventTupleUtil::Systematic>(syst));
    }
  } else {
    ProcessSystematic(EventTupleUtil::SystNominal);
  }

  return true;
}

void PhotonSelector::ProcessSystematic(EventTupleUtil::Systematic systematic)
{
  using namespace EventTupleUtil;

  // Convention: for histograms not labeled x_systs,
  // only fill them for SystNominal
  const bool isNominal{systematic==SystNominal};

  double weight = 1.0;
  if ( isNominal ) weightsMonitor->Fill(WeightTypes::StartingWeight, weight);

  // Check MET Filters (flags are true if passing filter)
  // Latest recommendations at https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2
  constexpr int filterMask{0
      | bit(EventTuple::Flag_HBHENoiseFilter)
      | bit(EventTuple::Flag_HBHENoiseIsoFilter)
      | bit(EventTuple::Flag_EcalDeadCellTriggerPrimitiveFilter)
      | bit(EventTuple::Flag_goodVertices)
      | bit(EventTuple::Flag_eeBadScFilter)
      // FIXME: for sync use 2015 filter
      // | bit(EventTuple::Flag_globalTightHalo2016Filter)
      | bit(EventTuple::Flag_CSCTightHalo2015Filter)
      | bit(EventTuple::Flag_BadChargedCandidateFilter)
      | bit(EventTuple::Flag_BadPFMuonFilter)
  };
  if ( (*metFilters & filterMask) != filterMask ) return;
  if ( isNominal ) weightsMonitor->Fill(WeightTypes::METFilter, weight);

  // Generator & Pileup weight
  double pileupWeight{1.};
  if ( !isRealData() ) {
    weight *= (*genWeight > 0) ? 1. : -1.;
    if ( isNominal ) weightsMonitor->Fill(WeightTypes::GenWeight, weight);

    const double pileupValue = ( nvtxPUreweighting ) ? *nPrimaryVertices : *nTruePileup;
    if ( systematic == SystPileupUp ) {
      pileupWeight = pileupSF->Evaluate1D(pileupValue, ScaleFactor::ShiftUp);
    }
    else if ( systematic == SystPileupDown ) {
      pileupWeight = pileupSF->Evaluate1D(pileupValue, ScaleFactor::ShiftDown);
    }
    else {
      pileupWeight = pileupSF->Evaluate1D(pileupValue);
    }
    weight *= pileupWeight;
    if ( isNominal ) weightsMonitor->Fill(WeightTypes::PileupWeight, weight);

    // Any k-factors go here

    if ( isNominal ) weightsMonitor->Fill(WeightTypes::KFactorWeight, weight);
  }

  // Find photon of interest
  LorentzVector photon;
  int nPhotons{0};
  for (size_t i=0; i<photon_id.GetSize(); ++i) {
    LorentzVector p4 = photon_p4[i];
    if ( systematic == SystPhotonEnUp ) applyObjectShift(p4, photon_p4Error[i], true);
    else if ( systematic == SystPhotonEnDown ) applyObjectShift(p4, photon_p4Error[i], false);

    int ids = photon_id[i];
    float dR = photon_matchedTriggerDeltaR[i];
    if ( p4.Pt() > 50 && fabs(p4.Eta()) < 1.4442 && passes(ids, EventTuple::PhotonTight) ) {
      nPhotons++;
      photon = p4;
      if ( isRealData() ) weight *= photon_minPrescale[i];
      if ( isNominal ) {
        weightsMonitor->Fill(WeightTypes::PrescaleWeight, weight);
        triggerDeltaR->Fill(dR, weight);
      }
    }
    else if ( passes(ids, EventTuple::PhotonLoose) ) {
      nPhotons++;
    }
    if ( nPhotons > 1 ) return;
  }

  // Find any loose e/mu/tau for veto purposes
  std::vector<Lepton> extraLeptons;
  std::vector<Lepton> extraTaus;
  for (size_t i=0; i<muon_id.GetSize(); ++i) {
    LorentzVector p4 = muon_p4[i];
    if ( systematic == SystMuonEnUp ) applyObjectShift(p4, muon_p4Error[i], true);
    else if ( systematic == SystMuonEnDown ) applyObjectShift(p4, muon_p4Error[i], false);

    int ids = muon_id[i];
    float iso = muon_relIsoDBetaR04[i];
    if ( p4.Pt() > 10 && fabs(p4.Eta()) <= 2.4 && passes(ids, EventTuple::MuonLoose) && iso < 0.25 ) {
      if ( DeltaR(p4, photon) < 0.3 ) continue;
      extraLeptons.emplace_back(p4, 13*muon_q[i]);
    }
  }
  for (size_t i=0; i<electron_id.GetSize(); ++i) {
    LorentzVector p4 = electron_p4[i];
    if ( systematic == SystElectronEnUp ) applyObjectShift(p4, electron_p4Error[i], true);
    else if ( systematic == SystElectronEnDown ) applyObjectShift(p4, electron_p4Error[i], false);

    int ids = electron_id[i];
    if ( p4.Pt() > 10 && fabs(p4.Eta()) <= 2.5 && (passes(ids, EventTuple::ElectronMedium)||passes(ids, EventTuple::ElectronVeto)) ) {
      if ( DeltaR(p4, photon) < 0.3 ) continue;
      extraLeptons.emplace_back(p4, 11*electron_q[i]);
    }
  }
  for (size_t i=0; i<tau_id.GetSize(); ++i) {
    LorentzVector p4 = tau_p4[i];

    int ids = tau_id[i];
    if ( p4.Pt() > 18 && fabs(p4.Eta()) <= 2.3 && passes(ids, EventTuple::TauDecayModeFinding) ) {
      if ( DeltaR(p4, photon) < 0.3 ) continue;
      extraTaus.emplace_back(p4, 15*tau_q[i]);
    }
  }
  if ( isNominal ) nAllLeptons->Fill(extraLeptons.size()+extraTaus.size(), weight);


  // Jet counting
  // TODO: future hope to remove jet count vetos
  // in favor of hadronic activity cut.  See slide 28:
  // https://agenda.hep.wisc.edu/event/965/session/0/contribution/0/material/slides/0.pdf
  std::vector<LorentzVector> goodJets;
  int nBjetsCSVmedium{0};
  for (size_t i=0; i<jet_id.GetSize(); ++i) {
    LorentzVector p4;
    if ( systematic == SystJetEnUp ) p4 = jet_p4_JetEnUp[i];
    if ( systematic == SystJetEnDown ) p4 = jet_p4_JetEnDown[i];
    if ( systematic == SystJetResUp ) p4 = jet_p4_JetResUp[i];
    if ( systematic == SystJetResDown ) p4 = jet_p4_JetResDown[i];
    else p4 = jet_p4[i];

    int ids = jet_id[i];
    if ( p4.Pt() <= 20 || fabs(p4.Eta()) > 5 || !passes(ids, EventTuple::JetLoose) ) continue;
    if ( DeltaR(p4, photon) < 0.4 ) continue;

    float btagMVAvalue = jet_btagCSVv2[i];
    if ( fabs(p4.Eta()) <= 2.4 && btagMVAvalue > 0.8 ) nBjetsCSVmedium++;

    if ( p4.Pt() >= 30 ) goodJets.emplace_back(p4);
  }
  if ( isNominal ) {
    nGoodJets->Fill(goodJets.size(), weight);
    nBtagMediumJets->Fill(nBjetsCSVmedium, weight);
  }

  if ( isNominal ) {
    photonPtPreSF->Fill(photon.Pt(), weight);
    photonEtaPreSF->Fill(photon.Eta(), weight);
  }


  // Apply relevant photon scale factors
  // TODO: photon efficiency & systematics
  // if ( !isRealData() ) {
  //   weight *= photonIdSF->Evaluate2D(photon.Eta(), photon.Pt());
  //   if ( isNominal ) weightsMonitor->Fill(WeightTypes::PhotonID, weight);
  // }

  if ( isNominal ) {
    nvtx->Fill(*nPrimaryVertices, weight);
    photonPt->Fill(photon.Pt(), weight);
    photonEta->Fill(photon.Eta(), weight);
  }

  Vector2D met = getMet(systematic);
  // TODO: systShiftsForMet
  Vector2D metForCuts = met;

  //
  // Old analyzer cut flow starting here
  //
  float iCut{0.};
  if ( isNominal ) oldAnalyzerCutflow->Fill(iCut, weight);
  if ( isNominal ) oldAnalyzerCutflowUnweighted->Fill(iCut++, 1.);

  // pass3dLeptonVeto
  if ( extraLeptons.size() == 0 ) {
    if ( isNominal ) oldAnalyzerCutflow->Fill(iCut, weight);
    if ( isNominal ) oldAnalyzerCutflowUnweighted->Fill(iCut++, 1.);
  }
  else return;

  // passBveto
  if ( nBjetsCSVmedium == 0 ) {
    if ( isNominal ) oldAnalyzerCutflow->Fill(iCut, weight);
    if ( isNominal ) oldAnalyzerCutflowUnweighted->Fill(iCut++, 1.);
  }
  else return;

  // passTauVeto
  if ( extraTaus.size() == 0 ) {
    if ( isNominal ) oldAnalyzerCutflow->Fill(iCut, weight);
    if ( isNominal ) oldAnalyzerCutflowUnweighted->Fill(iCut++, 1.);
  }
  else return;

  // nJetsGood30
  if ( goodJets.size() < 2 ) {
    if ( isNominal ) oldAnalyzerCutflow->Fill(iCut, weight);
    if ( isNominal ) oldAnalyzerCutflowUnweighted->Fill(iCut++, 1.);
  }
  else return;

  // passZmass
  if ( true ) {
    if ( isNominal ) oldAnalyzerCutflow->Fill(iCut, weight);
    if ( isNominal ) oldAnalyzerCutflowUnweighted->Fill(iCut++, 1.);
  }
  else return;

  // passZpt
  if ( photon.Pt() > 60. ) {
    if ( isNominal ) oldAnalyzerCutflow->Fill(iCut, weight);
    if ( isNominal ) oldAnalyzerCutflowUnweighted->Fill(iCut++, 1.);
  }
  else return;

  // passMET
  if ( metForCuts.R() > 100. ) {
    if ( isNominal ) oldAnalyzerCutflow->Fill(iCut, weight);
    if ( isNominal ) oldAnalyzerCutflowUnweighted->Fill(iCut++, 1.);
  }
  else return;

  // passDphiZMET
  if ( fabs(DeltaPhi(photon, metForCuts)) > 2.8 ) {
    if ( isNominal ) oldAnalyzerCutflow->Fill(iCut, weight);
    if ( isNominal ) oldAnalyzerCutflowUnweighted->Fill(iCut++, 1.);
  }
  else return;

  // passBalanceCut
  float balance = fabs(1-metForCuts.R()/photon.Pt());
  if ( balance < 0.4 ) {
    if ( isNominal ) oldAnalyzerCutflow->Fill(iCut, weight);
    if ( isNominal ) oldAnalyzerCutflowUnweighted->Fill(iCut++, 1.);
  }
  else return;

  // passDphiJetMET
  float jetMetMinDphi{999.};
  for(const auto& jet : goodJets) if ( fabs(DeltaPhi(jet, metForCuts)) < jetMetMinDphi ) jetMetMinDphi = fabs(DeltaPhi(jet, metForCuts));
  if ( jetMetMinDphi > 0.5 ) {
    if ( isNominal ) oldAnalyzerCutflow->Fill(iCut, weight);
    if ( isNominal ) oldAnalyzerCutflowUnweighted->Fill(iCut++, 1.);
  }
  else return;

  // realMETcutCR
  // Should do nothing for signal regions or ZZ CR
  if ( true ) {
    if ( isNominal ) oldAnalyzerCutflow->Fill(iCut, weight);
    if ( isNominal ) oldAnalyzerCutflowUnweighted->Fill(iCut++, 1.);
  }
  else return;

  pfMet_systs->Fill(metForCuts.R(), systematic, weight);
  if ( isNominal ) {
    realPfMet->Fill(met.R(), weight);
    genMetHist->Fill(genMet->R(), weight);
  }

  if ( isNominal && isRealData() ) {
    eventList.run = *run;
    eventList.lumi = *lumi;
    eventList.event = *event;
    eventList.channel = 0;
    eventList.pfMet = metForCuts.R();
    eventList.Zmass = photon.M();
    finalSelection->Fill();
  }
}

