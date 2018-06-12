#include "Analysis/monoZ/interface/METResponse.h"

ClassImp(METResponse)

void METResponse::SlaveBegin(TTree *)
{
  pileupSF = (ScaleFactor *) GetInputList()->FindObject("pileupSF");
  if ( pileupSF == nullptr ) Abort("METResponse needs Pileup Scale factors!");

  electronIdSF = (ScaleFactor *) GetInputList()->FindObject("electronIdSF");
  if ( electronIdSF == nullptr ) Abort("METResponse needs Electron ID Scale factors!");

  electronGsfSF = (ScaleFactor *) GetInputList()->FindObject("electronGsfSF");
  if ( electronGsfSF == nullptr ) Abort("METResponse needs Electron GSF Reco Scale factors!");

  muonIdSF = (ScaleFactor *) GetInputList()->FindObject("muonIdSF");
  if ( muonIdSF == nullptr ) Abort("METResponse needs muon ID Scale factors!");

  muonIsoSF = (ScaleFactor *) GetInputList()->FindObject("muonIsoSF");
  if ( muonIsoSF == nullptr ) Abort("METResponse needs muon Iso Scale factors!");

  auto systFlag = (TParameter<bool> *) GetInputList()->FindObject("skipSystematics");
  if ( systFlag != nullptr ) skipSystematics = systFlag->GetVal();

  auto nvtxFlag = (TParameter<bool> *) GetInputList()->FindObject("nvtxPUreweighting");
  if ( nvtxFlag != nullptr ) nvtxPUreweighting = nvtxFlag->GetVal();
}

void METResponse::SetupNewDirectory()
{
  // Must call base class setup
  SelectorBase::SetupNewDirectory();

  AddObject<TH1D>(weightsMonitor, "weightsMonitor", "Event weight multipliers", WeightTypes::NWeightTypes, 0, WeightTypes::NWeightTypes);
  weightsMonitor->GetXaxis()->SetBinLabel(1+WeightTypes::StartingWeight, "StartingWeight");
  weightsMonitor->GetXaxis()->SetBinLabel(1+WeightTypes::METFilter,      "METFilter");
  weightsMonitor->GetXaxis()->SetBinLabel(1+WeightTypes::GenWeight,      "GenWeight/Deduplicate");
  weightsMonitor->GetXaxis()->SetBinLabel(1+WeightTypes::PileupWeight,   "PileupWeight");
  weightsMonitor->GetXaxis()->SetBinLabel(1+WeightTypes::PostCategorize, "PostCategorize");
  weightsMonitor->GetXaxis()->SetBinLabel(1+WeightTypes::LeadingLepID,   "LeadingLepID");
  weightsMonitor->GetXaxis()->SetBinLabel(1+WeightTypes::TrailingLepID,  "TrailingLepID");

  AddObject<TH1F>(weirdStuffCounter, "weirdStuffCounter", "Counters for things that are unlikely", WeirdStuff::NWeirdStuff, 0, WeirdStuff::NWeirdStuff);
  weirdStuffCounter->GetXaxis()->SetBinLabel(1+ElectronOverlapsMuon, "e Overlaps #mu");

  AddObject<TH1D>(triggerBits, "triggerBits", "Trigger bit groups fired;Trigger group;Counts", 5, 0, 5);
  triggerBits->GetXaxis()->SetBinLabel(1+EventTuple::TriggerDoubleMuon,      "DoubleMuon");
  triggerBits->GetXaxis()->SetBinLabel(1+EventTuple::TriggerSingleMuon,      "SingleMuon");
  triggerBits->GetXaxis()->SetBinLabel(1+EventTuple::TriggerDoubleElectron,  "DoubleElectron");
  triggerBits->GetXaxis()->SetBinLabel(1+EventTuple::TriggerSingleElectron,  "SingleElectron");
  triggerBits->GetXaxis()->SetBinLabel(1+EventTuple::TriggerMuonElectron,    "MuonEG");

  AddObject<TH1D>(nvtxRaw, "nvtxRaw", "Number reco vertices;# Reco Vertices;Counts", 70, 0, 70);
  AddObject<TH1D>(nvtxWeight, "nvtxWeight", "Number reco vertices;# Reco Vertices;Counts", 70, 0, 70);

  AddObject<TH1D>(nAllLeptons, "nAllLeptons", "Number of leptons stored in tuple;# Skimmed Leptons;Counts", 6, -0.5, 5.5);
  AddObject<TH1D>(nGoodLeptons, "nGoodLeptons", "Number of high quality leptons selected;# Selected Leptons;Counts", 6, -0.5, 5.5);
  AddObject<TH1D>(nExtraLeptons, "nExtraLeptons", "Number of extra leptons selected;# Selected Leptons;Counts", 6, -0.5, 5.5);
  AddObject<TH2D>(nLooseTightLeptons, "nLooseTightLeptons", "Number loose vs. tight;# Loose Leptons (e#mu#tau);# Tight Leptons (e#mu);Counts", 6, -0.5, 5.5, 6, -0.5, 5.5);

  AddObject<CategorizedHist1D>(categories_systs, "categories_systs", "Event categories counter;Systematic;Counts", EventTupleUtil::NSystematics, 0, EventTupleUtil::NSystematics);
  for(int syst=EventTupleUtil::SystNominal; syst<EventTupleUtil::NSystematics; ++syst) {
    categories_systs->SetXaxisBinLabel(1+syst, EventTupleUtil::SystematicNames.at(static_cast<EventTupleUtil::Systematic>(syst)));
  }

  AddObject<CategorizedHist1D>(noSeed, "noSeed", "No seed;No seed;Counts", 1, 0, 1);

  AddObject<CategorizedHist1D>(triggerBitsPostCategorize, "triggerBitsPostCategorize", "Trigger bit groups fired;Trigger group;Counts", 5, 0, 5);
  triggerBitsPostCategorize->SetXaxisBinLabel(1+EventTuple::TriggerDoubleMuon,      "DoubleMuon");
  triggerBitsPostCategorize->SetXaxisBinLabel(1+EventTuple::TriggerSingleMuon,      "SingleMuon");
  triggerBitsPostCategorize->SetXaxisBinLabel(1+EventTuple::TriggerDoubleElectron,  "DoubleElectron");
  triggerBitsPostCategorize->SetXaxisBinLabel(1+EventTuple::TriggerSingleElectron,  "SingleElectron");
  triggerBitsPostCategorize->SetXaxisBinLabel(1+EventTuple::TriggerMuonElectron,    "MuonEG");

  AddObject<CategorizedHist1D>(nvtx, "nvtx", "Number reco vertices;# Reco Vertices;Counts", 70, 0, 70);

  AddObject<CategorizedHist1D>(leadingLepPtPreSF, "leadingLepPtPreSF", "Leading Lepton Pre-ScaleFactor;Leading Lepton p_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(leadingLepEtaPreSF, "leadingLepEtaPreSF", "Leading Lepton Pre-ScaleFactor;Leading Lepton #eta;Counts / 0.1", 50, -2.5, 2.5);
  AddObject<CategorizedHist1D>(trailingLepPtPreSF, "trailingLepPtPreSF", "Trailing Lepton Pre-ScaleFactor;Trailing Lepton p_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(trailingLepEtaPreSF, "trailingLepEtaPreSF", "Trailing Lepton Pre-ScaleFactor;Trailing Lepton #eta;Counts / 0.1", 50, -2.5, 2.5);
  AddObject<CategorizedHist1D>(leadingLepPt, "leadingLepPt", "Leading Lepton;Leading Lepton p_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(leadingLepEta, "leadingLepEta", "Leading Lepton;Leading Lepton #eta;Counts / 0.1", 50, -2.5, 2.5);
  AddObject<CategorizedHist1D>(trailingLepPt, "trailingLepPt", "Trailing Lepton;Trailing Lepton p_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(trailingLepEta, "trailingLepEta", "Trailing Lepton;Trailing Lepton #eta;Counts / 0.1", 50, -2.5, 2.5);

  AddObject<CategorizedHist1D>(Zmass, "Zmass", "Z Mass;m_{ll} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(Zpt,   "Zpt",   "Z Pt;q_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(Zeta,  "Zeta",  "Z Eta;#eta_{ll} [GeV];Counts / 0.1", 100, -5., 5.);
  AddObject<CategorizedHist1D>(Zrapidity,  "Zrapidity",  "Z Rapidity;y_{ll} [GeV];Counts / 0.1", 60, -3., 3.);

  AddObject<CategorizedHist2D>(pfMet_systs, "pfMet_systs", "PF MET; PF #slash{E}_{T} [GeV];Systematic;Counts / 5 GeV", 100, 0, 500, EventTupleUtil::NSystematics, 0, EventTupleUtil::NSystematics);
  AddObject<CategorizedHist2D>(pfMetResponse_systs, "pfMetResponse_systs", "PF MET Response; u_{#parallel} + q_{T} [GeV];Systematic;Counts / 5 GeV", 100, -250, 250, EventTupleUtil::NSystematics, 0, EventTupleUtil::NSystematics);
  AddObject<CategorizedHist2D>(pfMetResolution_systs, "pfMetResolution_systs", "PF MET Resolution; u_{#perp} [GeV];Systematic;Counts / 5 GeV", 100, -250, 250, EventTupleUtil::NSystematics, 0, EventTupleUtil::NSystematics);
  AddObject<CategorizedHist2D>(metSignificance_systs, "metSignificance_systs", "PF MET Significance;#slash{E}_{T} Significance;Systematic;Counts / 1", 100, 0, 100, EventTupleUtil::NSystematics, 0, EventTupleUtil::NSystematics);
  AddObject<CategorizedHist2D>(metSigChi2_systs, "metSigChi2_systs", "PF MET Significance Probability;#slash{E}_{T} Sig. #chi^{2} Prob.;Systematic;Counts / 0.01", 100, 0, 1, EventTupleUtil::NSystematics, 0, EventTupleUtil::NSystematics);
  for(int syst=EventTupleUtil::SystNominal; syst<EventTupleUtil::NSystematics; ++syst) {
    pfMet_systs->SetYaxisBinLabel(1+syst, EventTupleUtil::SystematicNames.at(static_cast<EventTupleUtil::Systematic>(syst)));
    pfMetResponse_systs->SetYaxisBinLabel(1+syst, EventTupleUtil::SystematicNames.at(static_cast<EventTupleUtil::Systematic>(syst)));
    pfMetResolution_systs->SetYaxisBinLabel(1+syst, EventTupleUtil::SystematicNames.at(static_cast<EventTupleUtil::Systematic>(syst)));
    metSignificance_systs->SetYaxisBinLabel(1+syst, EventTupleUtil::SystematicNames.at(static_cast<EventTupleUtil::Systematic>(syst)));
    metSigChi2_systs->SetYaxisBinLabel(1+syst, EventTupleUtil::SystematicNames.at(static_cast<EventTupleUtil::Systematic>(syst)));
  }

  AddObject<CategorizedHist2D>(pfMet_nvtx, "pfMet_nvtx", "MET vs. vertex;PF #slash{E}_{T} [GeV];# Reco. Vertices;Counts", 100, 0, 500, 70, 0, 70);
  AddObject<CategorizedHist2D>(pfMet_nJets, "pfMet_nJets", "MET vs. jets;PF #slash{E}_{T} [GeV];# Jets (>20GeV);Counts", 100, 0, 500, 5, 0, 5);
  AddObject<CategorizedHist2D>(metSignificance_nJets, "metSignificance_nJets", "MET Significance vs. jets;#slash{E}_{T} Significance;# Jets (>20GeV);Counts", 100, 0, 100, 5, 0, 5);
  AddObject<CategorizedHist1D>(genMetHist,   "genMet",   "genMet;Generator #slash{E}_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(ZptMcut,   "ZptMcut",   "Z Pt;q_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(ZetaMcut,  "ZetaMcut",  "Z Eta;#eta_{ll} [GeV];Counts / 0.1", 100, -5., 5.);
  AddObject<CategorizedHist1D>(ZrapidityMcut,  "ZrapidityMcut",  "Z Rapidity;y_{ll} [GeV];Counts / 0.1", 60, -3., 3.);
}

Bool_t METResponse::Process(Long64_t entry)
{
  // Must call base class process (sets reader entry, counter)
  if ( ! SelectorBase::Process(entry) ) return false;

  if ( !skipSystematics ) {
    for(int syst=EventTupleUtil::SystNominal; syst<EventTupleUtil::NSystematics; ++syst) {
      ProcessSystematic(static_cast<EventTupleUtil::Systematic>(syst));
      // Don't care about MonoZ systematics as much
      if ( syst == EventTupleUtil::SystPileupDown ) break;
    }
  } else {
    ProcessSystematic(EventTupleUtil::SystNominal);
  }

  return true;
}

void METResponse::ProcessSystematic(EventTupleUtil::Systematic systematic)
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
      | bit(EventTuple::Flag_globalTightHalo2016Filter)
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
  }


  // Find possible trigger seeds
  // Future: seeds bool could be replaced with trigger matching
  bool seedsEEcat{false};
  bool seedsEMcat{false};
  bool seedsMMcat{false};
  if ( hasTrigger(EventTuple::TriggerSingleElectron) ) {
    if ( isNominal ) triggerBits->Fill(EventTuple::TriggerSingleElectron, weight);
    seedsEEcat |= true;
    seedsEMcat |= true;
  }
  if ( hasTrigger(EventTuple::TriggerDoubleElectron) ) {
    if ( isNominal ) triggerBits->Fill(EventTuple::TriggerDoubleElectron, weight);
    seedsEEcat |= true;
  }
  if ( hasTrigger(EventTuple::TriggerSingleMuon) ) {
    if ( isNominal ) triggerBits->Fill(EventTuple::TriggerSingleMuon, weight);
    seedsEMcat |= true;
    seedsMMcat |= true;
  }
  if ( hasTrigger(EventTuple::TriggerDoubleMuon) ) {
    if ( isNominal ) triggerBits->Fill(EventTuple::TriggerDoubleMuon, weight);
    seedsMMcat |= true;
  }
  if ( hasTrigger(EventTuple::TriggerMuonElectron) ) {
    if ( isNominal ) triggerBits->Fill(EventTuple::TriggerMuonElectron, weight);
    seedsEMcat |= true;
  }
  // ICHEP MC problem: no triggers! :<
  // But at least the Drell-Yan was reHLT'd
  if ( !isRealData() && !isDataset(MC_DY) ) { seedsEEcat = true; seedsEMcat = true; seedsMMcat = true; }


  // Duplicate removal
  // For data, same event could come from different datasets
  if ( isRealData() ) {
    bool allowedPath{false};
    if ( isDataset(DoubleMuon)
         && hasTrigger(EventTuple::TriggerDoubleMuon)
       )
    {
      allowedPath = true;
    }
    else if ( isDataset(DoubleEG)
              && hasTrigger(EventTuple::TriggerDoubleElectron)
              && !hasTrigger(EventTuple::TriggerDoubleMuon)
            )
    {
      allowedPath = true;
    }
    else if ( isDataset(MuonEG)
              && hasTrigger(EventTuple::TriggerMuonElectron)
              && !hasTrigger(EventTuple::TriggerDoubleMuon)
              && !hasTrigger(EventTuple::TriggerDoubleElectron)
            )
    {
      allowedPath = true;
    }
    else if ( isDataset(SingleMuon)
              && hasTrigger(EventTuple::TriggerSingleMuon)
              && !hasTrigger(EventTuple::TriggerDoubleMuon)
              && !hasTrigger(EventTuple::TriggerDoubleElectron)
              && !hasTrigger(EventTuple::TriggerMuonElectron)
            )
    {
      allowedPath = true;
    }
    else if ( isDataset(SingleElectron)
              && hasTrigger(EventTuple::TriggerDoubleElectron)
              && !hasTrigger(EventTuple::TriggerDoubleMuon)
              && !hasTrigger(EventTuple::TriggerDoubleElectron)
              && !hasTrigger(EventTuple::TriggerMuonElectron)
              && !hasTrigger(EventTuple::TriggerSingleMuon)
            )
    {
      allowedPath = true;
    }
    if ( !allowedPath ) return;
    // Use GenWeight for deduplicate monitor in data
    if ( isNominal ) weightsMonitor->Fill(WeightTypes::GenWeight, weight);
  }


  // Monitor PU vertex distribution
  if ( isNominal && pileupWeight != 0 ) nvtxRaw->Fill(*nPrimaryVertices, weight/pileupWeight);
  if ( isNominal ) nvtxWeight->Fill(*nPrimaryVertices, weight);


  // Choose tight-quality e/mu for event categorization
  if ( isNominal ) nAllLeptons->Fill(muon_id.GetSize()+electron_id.GetSize(), weight);
  std::vector<Lepton> goodLeptons;
  for (size_t i=0; i<muon_id.GetSize(); ++i) {
    LorentzVector p4 = muon_p4[i];
    if ( systematic == SystMuonEnUp ) applyObjectShift(p4, muon_p4Error[i], true);
    else if ( systematic == SystMuonEnDown ) applyObjectShift(p4, muon_p4Error[i], false);

    int ids = muon_id[i];
    float iso = muon_relIsoDBetaR04[i];
    if ( p4.Pt() > 20 && fabs(p4.Eta()) < 2.4 && passes(ids, EventTuple::MuonTight) && iso < 0.15 ) {
      goodLeptons.emplace_back(p4, 13*muon_q[i]);
    }
  }
  for (size_t i=0; i<electron_id.GetSize(); ++i) {
    LorentzVector p4 = electron_p4[i];
    if ( systematic == SystElectronEnUp ) applyObjectShift(p4, electron_p4Error[i], true);
    else if ( systematic == SystElectronEnDown ) applyObjectShift(p4, electron_p4Error[i], false);

    int ids = electron_id[i];
    if ( p4.Pt() > 20 && fabs(p4.Eta()) < 2.5 && passes(ids, EventTuple::ElectronMedium) ) {
      // Electron fake muon? Unlikely
      if ( overlaps(goodLeptons, p4, 0.01) ) {
        if ( isNominal ) weirdStuffCounter->Fill(ElectronOverlapsMuon);
        continue;
      }
      goodLeptons.emplace_back(p4, 11*electron_q[i]);
    }
  }
  if ( isNominal ) nGoodLeptons->Fill(goodLeptons.size(), weight);


  // Find any remaining e/mu/tau that pass looser selection
  std::vector<Lepton> extraLeptons;
  for (size_t i=0; i<muon_id.GetSize(); ++i) {
    LorentzVector p4 = muon_p4[i];
    if ( systematic == SystMuonEnUp ) applyObjectShift(p4, muon_p4Error[i], true);
    else if ( systematic == SystMuonEnDown ) applyObjectShift(p4, muon_p4Error[i], false);

    if ( overlaps(goodLeptons, p4, 0.01) ) continue;
    int ids = muon_id[i];
    float iso = muon_relIsoDBetaR04[i];
    if ( p4.Pt() > 10 && fabs(p4.Eta()) < 2.4 && passes(ids, EventTuple::MuonLoose) && iso < 0.25 ) {
      extraLeptons.emplace_back(p4, 13*muon_q[i]);
    }
  }
  for (size_t i=0; i<electron_id.GetSize(); ++i) {
    LorentzVector p4 = electron_p4[i];
    if ( systematic == SystElectronEnUp ) applyObjectShift(p4, electron_p4Error[i], true);
    else if ( systematic == SystElectronEnDown ) applyObjectShift(p4, electron_p4Error[i], false);

    if ( overlaps(goodLeptons, p4, 0.01) ) continue;
    int ids = electron_id[i];
    if ( p4.Pt() > 10 && fabs(p4.Eta()) < 2.5 && passes(ids, EventTuple::ElectronLoose) ) {
      extraLeptons.emplace_back(p4, 11*electron_q[i]);
    }
  }
  for (size_t i=0; i<tau_id.GetSize(); ++i) {
    LorentzVector p4 = tau_p4[i];

    if ( overlaps(goodLeptons, p4, 0.4) ) continue;
    int ids = tau_id[i];
    if ( p4.Pt() > 18 && fabs(p4.Eta()) < 2.3 && passes(ids, EventTuple::TauDecayModeFinding) && passes(ids, EventTuple::TauLooseCombinedIso3Hits) ) {
      extraLeptons.emplace_back(p4, 15*tau_q[i]);
    }
  }
  if ( isNominal ) nExtraLeptons->Fill(extraLeptons.size(), weight);
  if ( isNominal ) nLooseTightLeptons->Fill(extraLeptons.size(), goodLeptons.size(), weight);


  // Find event category
  EventCategory eventCategory{CatUnknown};
  LorentzVector Zcand;
  Lepton leadingLep{Zcand, 0};
  Lepton trailingLep{Zcand, 0};
  if ( goodLeptons.size() < 2 ) {
    // Not enough leptons!
    return;
  }
  else if ( goodLeptons.size() == 2 ) {
    // Z signal region
    leadingLep = goodLeptons[0];
    trailingLep = goodLeptons[1];
    if ( trailingLep.p4.Pt() > leadingLep.p4.Pt() ) std::swap(leadingLep, trailingLep);

    // FIXME: Hack to get out of trigger efficiency turn-on
    if ( abs(leadingLep.pdgId) == 11 && leadingLep.p4.Pt() < 25 ) return;

    if ( goodLeptons[0].pdgId * goodLeptons[1].pdgId == -11*11 ) {
      eventCategory = CatEE;
      if ( !seedsEEcat ) noSeed->Fill(eventCategory, 0.5, weight);
      if ( !seedsEEcat ) return;
    }
    else if ( goodLeptons[0].pdgId * goodLeptons[1].pdgId == -11*13 ) {
      eventCategory = CatEM;
      if ( !seedsEMcat ) noSeed->Fill(eventCategory, 0.5, weight);
      if ( !seedsEMcat ) return;
    }
    else if ( goodLeptons[0].pdgId * goodLeptons[1].pdgId == -13*13 ) {
      eventCategory = CatMM;
      if ( !seedsMMcat ) noSeed->Fill(eventCategory, 0.5, weight);
      if ( !seedsMMcat ) return;
    }
    else {
      // Same-sign
      return;
    }
    Zcand = goodLeptons[0].p4 + goodLeptons[1].p4;
  }
  else {
    // Too many leptons!
    return;
  }
  categories_systs->Fill(eventCategory, systematic, weight);

  if ( isNominal ) {
    weightsMonitor->Fill(WeightTypes::PostCategorize, weight);

    if ( hasTrigger(EventTuple::TriggerSingleElectron) ) {
      triggerBitsPostCategorize->Fill(eventCategory, EventTuple::TriggerSingleElectron, weight);
    }
    if ( hasTrigger(EventTuple::TriggerDoubleElectron) ) {
      triggerBitsPostCategorize->Fill(eventCategory, EventTuple::TriggerDoubleElectron, weight);
    }
    if ( hasTrigger(EventTuple::TriggerSingleMuon) ) {
      triggerBitsPostCategorize->Fill(eventCategory, EventTuple::TriggerSingleMuon, weight);
    }
    if ( hasTrigger(EventTuple::TriggerDoubleMuon) ) {
      triggerBitsPostCategorize->Fill(eventCategory, EventTuple::TriggerDoubleMuon, weight);
    }
    if ( hasTrigger(EventTuple::TriggerMuonElectron) ) {
      triggerBitsPostCategorize->Fill(eventCategory, EventTuple::TriggerMuonElectron, weight);
    }

    nvtx->Fill(eventCategory, *nPrimaryVertices, weight);

    leadingLepPtPreSF->Fill(eventCategory, leadingLep.p4.Pt(), weight);
    leadingLepEtaPreSF->Fill(eventCategory, leadingLep.p4.Eta(), weight);
    trailingLepPtPreSF->Fill(eventCategory, trailingLep.p4.Pt(), weight);
    trailingLepEtaPreSF->Fill(eventCategory, trailingLep.p4.Eta(), weight);
  }


  // Apply relevant lepton scale factors
  if ( !isRealData() ) {
    if ( abs(leadingLep.pdgId) == 11 ) {
      weight *= electronGsfSF->Evaluate2D(leadingLep.p4.Eta(), leadingLep.p4.Pt());
      weight *= electronIdSF->Evaluate2D(leadingLep.p4.Eta(), leadingLep.p4.Pt());
      if ( isNominal ) weightsMonitor->Fill(WeightTypes::LeadingLepID, weight);
    }
    else if ( abs(leadingLep.pdgId) == 13 ) {
      weight *= muonIdSF->Evaluate1D(leadingLep.p4.Eta());
      weight *= muonIsoSF->Evaluate2D(fabs(leadingLep.p4.Eta()), leadingLep.p4.Pt());
      if ( isNominal ) weightsMonitor->Fill(WeightTypes::LeadingLepID, weight);
    }

    if ( abs(trailingLep.pdgId) == 11 ) {
      weight *= electronGsfSF->Evaluate2D(trailingLep.p4.Eta(), trailingLep.p4.Pt());
      weight *= electronIdSF->Evaluate2D(trailingLep.p4.Eta(), trailingLep.p4.Pt());
      if ( isNominal ) weightsMonitor->Fill(WeightTypes::TrailingLepID, weight);
    }
    else if ( abs(trailingLep.pdgId) == 13 ) {
      weight *= muonIdSF->Evaluate1D(trailingLep.p4.Eta());
      weight *= muonIsoSF->Evaluate2D(fabs(trailingLep.p4.Eta()), trailingLep.p4.Pt());
      if ( isNominal ) weightsMonitor->Fill(WeightTypes::TrailingLepID, weight);
    }
  }

  if ( isNominal ) {
    leadingLepPt->Fill(eventCategory, leadingLep.p4.Pt(), weight);
    leadingLepEta->Fill(eventCategory, leadingLep.p4.Eta(), weight);
    trailingLepPt->Fill(eventCategory, trailingLep.p4.Pt(), weight);
    trailingLepEta->Fill(eventCategory, trailingLep.p4.Eta(), weight);

    Zmass->Fill(eventCategory, Zcand.M(), weight);
    Zpt->Fill(eventCategory, Zcand.Pt(), weight);
    Zeta->Fill(eventCategory, Zcand.Eta(), weight);
    Zrapidity->Fill(eventCategory, Zcand.Rapidity(), weight);
  }

  if ( fabs(Zcand.M()-91.) <= 10 ) {
    Vector2D met = getMet(systematic);
    // TODO: systShiftsForMet
    float metSig = ROOT::Math::Similarity({met.X(), met.Y()}, *metSigMatrixInverse);
    Vector2D recoil = - met - Vector2D(Zcand);
    recoil.Rotate(-Zcand.Phi());

    pfMet_systs->Fill(eventCategory, met.R(), systematic, weight);
    pfMetResponse_systs->Fill(eventCategory, recoil.X() + Zcand.pt(), systematic, weight);
    pfMetResolution_systs->Fill(eventCategory, recoil.Y(), systematic, weight);
    metSignificance_systs->Fill(eventCategory, metSig, systematic, weight);
    metSigChi2_systs->Fill(eventCategory, TMath::Prob(*metSignificance, 2), systematic, weight);

    if ( isNominal ) {
      pfMet_nvtx->Fill(eventCategory, met.R(), *nPrimaryVertices, weight);
      pfMet_nJets->Fill(eventCategory, met.R(), jet_id.GetSize(), weight); // Tuple cut: >20GeV, eta<5, pf Loose
      metSignificance_nJets->Fill(eventCategory, *metSignificance, jet_id.GetSize(), weight);
      if ( !isRealData() ) genMetHist->Fill(eventCategory, genMet->R(), weight);
      ZptMcut->Fill(eventCategory, Zcand.Pt(), weight);
      ZetaMcut->Fill(eventCategory, Zcand.Eta(), weight);
      ZrapidityMcut->Fill(eventCategory, Zcand.Rapidity(), weight);
    }
  }
}

