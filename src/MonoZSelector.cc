#include "Analysis/monoZ/interface/MonoZSelector.h"

ClassImp(MonoZSelector)

// Constants and functions local to this selector
namespace {
  constexpr double nominalZmass{91.1876};
}

void MonoZSelector::SlaveBegin(TTree *)
{
  pileupSF = (ScaleFactor *) GetInputList()->FindObject("pileupSF");
  if ( pileupSF == nullptr ) Abort("MonoZSelector needs Pileup Scale factors!");

  electronIdSF = (ScaleFactor *) GetInputList()->FindObject("electronIdSF");
  if ( electronIdSF == nullptr ) Abort("MonoZSelector needs Electron ID Scale factors!");

  electronGsfSF = (ScaleFactor *) GetInputList()->FindObject("electronGsfSF");
  if ( electronGsfSF == nullptr ) Abort("MonoZSelector needs Electron GSF Reco Scale factors!");

  muonIdSF = (ScaleFactor *) GetInputList()->FindObject("muonIdSF");
  if ( muonIdSF == nullptr ) Abort("MonoZSelector needs muon ID Scale factors!");

  muonIsoSF = (ScaleFactor *) GetInputList()->FindObject("muonIsoSF");
  if ( muonIsoSF == nullptr ) Abort("MonoZSelector needs muon Iso Scale factors!");

  btagEfficiency = (ScaleFactor *) GetInputList()->FindObject("btagEfficiency");
  // If nullptr we use dummy values

  auto btagData  = (BTagData *) GetInputList()->FindObject("btagData");
  if ( btagData == nullptr ) Abort("MonoZSelector needs B Tagging data files!");
  btagReader = new BTagCalibrationReader(BTagEntry::OP_MEDIUM, "central", {"up", "down"});
  btagReader->load(btagData->data, BTagEntry::FLAV_B, "comb");
  btagReader->load(btagData->data, BTagEntry::FLAV_C, "comb");
  btagReader->load(btagData->data, BTagEntry::FLAV_UDSG, "incl");

  dibosonCorrector = (DibosonCorrections *) GetInputList()->FindObject("dibosonCorrector");
  if ( dibosonCorrector == nullptr ) Abort("MonoZSelector needs a DibosonCorrections input!");

  auto getFlag = [this] (const char * name) -> bool {
    auto flag = (TParameter<bool> *) GetInputList()->FindObject(name);
    if ( flag != nullptr ) return flag->GetVal();
    return false;
  };

  skipSystematics = getFlag("skipSystematics");
  nvtxPUreweighting = getFlag("nvtxPUreweighting");
  printEventList = getFlag("printEventList");
  doBtagEfficiency = getFlag("doBtagEfficiency");
}

void MonoZSelector::SlaveTerminate()
{
  // Deallocate any objects from SlaveBegin
  SafeDelete(btagReader);
}

void MonoZSelector::SetupNewDirectory()
{
  // Must call base class setup
  SelectorBase::SetupNewDirectory();

  AddObject<TH1D>(weightsMonitor, "weightsMonitor", "Event weight multipliers", WeightTypes::NWeightTypes, 0, WeightTypes::NWeightTypes);
  weightsMonitor->GetXaxis()->SetBinLabel(1+StartingWeight, "StartingWeight");
  weightsMonitor->GetXaxis()->SetBinLabel(1+METFilter,      "METFilter");
  weightsMonitor->GetXaxis()->SetBinLabel(1+GenWeight,      "GenWeight/Deduplicate");
  weightsMonitor->GetXaxis()->SetBinLabel(1+PileupWeight,   "PileupWeight");
  weightsMonitor->GetXaxis()->SetBinLabel(1+KFactorWeight,  "KFactorWeight");
  weightsMonitor->GetXaxis()->SetBinLabel(1+BTagWeight,     "BTagWeight");
  weightsMonitor->GetXaxis()->SetBinLabel(1+PostCategorize, "PostCategorize");
  weightsMonitor->GetXaxis()->SetBinLabel(1+LeadingLepID,   "LeadingLepID");
  weightsMonitor->GetXaxis()->SetBinLabel(1+TrailingLepID,  "TrailingLepID");

  AddObject<TH1D>(dyTauTauCutCheck, "dyTauTauCutCheck", "check;# Leptonic #tau;Counts", 4, -0.5, 3.5);
  AddObject<TH1D>(dyStitchCheck, "dyStitchCheck", "check;LHE Z p_{T};Counts / 5 GeV", 100, 0., 500.);
  AddObject<TH2D>(vvKfactorCheck, "vvKfactorCheck", "EWK k-factor vs. lower V pt;Trailing V p_{T} [GeV];EWK k-factor;Counts", 25, 0, 500, 35, 0.5, 1.2);

  AddObject<TH1F>(weirdStuffCounter, "weirdStuffCounter", "Counters for things that are unlikely", WeirdStuff::NWeirdStuff, 0, WeirdStuff::NWeirdStuff);
  weirdStuffCounter->GetXaxis()->SetBinLabel(1+ElectronOverlapsMuon, "e Overlaps #mu");

  AddObject<TH1D>(triggerBits, "triggerBits", "Trigger bit groups fired;Trigger group;Counts", 5, 0, 5);
  triggerBits->GetXaxis()->SetBinLabel(1+EventTuple::TriggerDoubleMuon,      "DoubleMuon");
  triggerBits->GetXaxis()->SetBinLabel(1+EventTuple::TriggerSingleMuon,      "SingleMuon");
  triggerBits->GetXaxis()->SetBinLabel(1+EventTuple::TriggerDoubleElectron,  "DoubleElectron");
  triggerBits->GetXaxis()->SetBinLabel(1+EventTuple::TriggerSingleElectron,  "SingleElectron");
  triggerBits->GetXaxis()->SetBinLabel(1+EventTuple::TriggerMuonElectron,    "MuonEG");

  AddObject<TH1D>(nAllLeptons, "nAllLeptons", "Number of leptons stored in tuple;# Skimmed Leptons;Counts", 6, -0.5, 5.5);
  AddObject<TH1D>(nGoodLeptons, "nGoodLeptons", "Number of high quality leptons selected;# Selected Leptons;Counts", 6, -0.5, 5.5);
 
  AddObject<TH1D>(nChadLeptons, "nChadLeptons", "Creative Title", 6, -0.5, 5.5);

  AddObject<TH2D>(electronCorrection, "electronCorrection", "Correlation EGMSmearer;EGMSmearer (no reg.) p_{T};EGMRegression p_{T}", 100, 0, 500, 100, 0, 500);
  AddObject<TH1D>(nExtraLeptons, "nExtraLeptons", "Number of extra leptons selected;# Selected Leptons;Counts", 6, -0.5, 5.5);
  AddObject<TH2D>(nLooseTightLeptons, "nLooseTightLeptons", "Number loose vs. tight;# Loose Leptons (e#mu);# Tight Leptons (e#mu);Counts", 6, -0.5, 5.5, 6, -0.5, 5.5);
  AddObject<TH2D>(nLooseTightLeptonsWTau, "nLooseTightLeptonsWTau", "Number loose vs. tight;# Loose Leptons (e#mu#tau);# Tight Leptons (e#mu);Counts", 6, -0.5, 5.5, 6, -0.5, 5.5);

  AddObject<TH1D>(nGoodJets, "nGoodJets", "Number selected jets;# Selected Jets;Counts", 6, -0.5, 5.5);
  AddObject<TH1D>(nBtagMediumJets, "nBtagMediumJets", "Number b-tagged jets;# CSVv2 Medium B-tagged Jets;Counts", 6, -0.5, 5.5);

  if ( doBtagEfficiency ) {
    AddObject<TH3D>(btagEffNumerator, "btagEffNumerator", "B-tagging efficiency numerator;Jet hadronFlavor mod 3;Jet p_{T};Jet #eta", 3, 0, 3, 4, 20, 100, 2, -2.4, 2.4);
    AddObject<TH3D>(btagEffDenominator, "btagEffDenominator", "B-tagging efficiency denominator;Jet hadronFlavor mod 3;Jet p_{T};Jet #eta", 3, 0, 3, 4, 20, 100, 2, -2.4, 2.4);
  }

  AddObject<CategorizedHist1D>(categories_systs, "categories_systs", "Event categories counter;Systematic;Counts", EventTupleUtil::NSystematics, 0, EventTupleUtil::NSystematics);
  for(int syst=EventTupleUtil::SystNominal; syst<EventTupleUtil::NSystematics; ++syst) {
    categories_systs->SetXaxisBinLabel(1+syst, EventTupleUtil::SystematicNames.at(static_cast<EventTupleUtil::Systematic>(syst)));
  }

  AddObject<CategorizedHist1D>(triggerBitsPostCategorize, "triggerBitsPostCategorize", "Trigger bit groups fired;Trigger group;Counts", 5, 0, 5);
  triggerBitsPostCategorize->SetXaxisBinLabel(1+EventTuple::TriggerDoubleMuon,      "DoubleMuon");
  triggerBitsPostCategorize->SetXaxisBinLabel(1+EventTuple::TriggerSingleMuon,      "SingleMuon");
  triggerBitsPostCategorize->SetXaxisBinLabel(1+EventTuple::TriggerDoubleElectron,  "DoubleElectron");
  triggerBitsPostCategorize->SetXaxisBinLabel(1+EventTuple::TriggerSingleElectron,  "SingleElectron");
  triggerBitsPostCategorize->SetXaxisBinLabel(1+EventTuple::TriggerMuonElectron,    "MuonEG");

  AddObject<CategorizedHist1D>(nvtx, "nvtx", "Number reco vertices;# Reco Vertices;Counts", 75, 0, 75);
  AddObject<CategorizedHist1D>(rho, "rho", "Avg. energy density;#rho;Counts", 75, 0, 75);
  AddObject<CategorizedHist1D>(leadingLepPtPreSF, "leadingLepPtPreSF", "Leading Lepton Pre-ScaleFactor;Leading Lepton p_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(leadingLepEtaPreSF, "leadingLepEtaPreSF", "Leading Lepton Pre-ScaleFactor;Leading Lepton #eta;Counts / 0.1", 50, -2.5, 2.5);
  AddObject<CategorizedHist1D>(trailingLepPtPreSF, "trailingLepPtPreSF", "Trailing Lepton Pre-ScaleFactor;Trailing Lepton p_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(trailingLepEtaPreSF, "trailingLepEtaPreSF", "Trailing Lepton Pre-ScaleFactor;Trailing Lepton #eta;Counts / 0.1", 50, -2.5, 2.5);

  AddObject<CategorizedHist1D>(triggerEfficiencyPostZpt, "triggerEfficiencyPostZpt", "Trigger efficiencyPostZpt;Bin;Counts", 2, 0, 2);
  triggerEfficiencyPostZpt->SetXaxisBinLabel(1, "All");
  triggerEfficiencyPostZpt->SetXaxisBinLabel(2, "Pass");
  AddObject<CategorizedHist1D>(leadingLepPtPostZpt, "leadingLepPtPostZpt", "Leading Lepton;Leading Lepton p_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(trailingLepPtPostZpt, "trailingLepPtPostZpt", "Trailing Lepton;Trailing Lepton p_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(leadingLepPtPostZptWtrigger, "leadingLepPtPostZptWtrigger", "Leading Lepton;Leading Lepton p_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(trailingLepPtPostZptWtrigger, "trailingLepPtPostZptWtrigger", "Trailing Lepton;Trailing Lepton p_{T} [GeV];Counts / 5 GeV", 100, 0, 500);

  AddObject<CategorizedHist2D>(metVsUncleaned, "metVsUncleaned", "MET;Mono-Z Cleaned PF MET;Uncleaned PF MET;Counts", 100, 0, 500, 100, 0, 500);
  AddObject<CategorizedHist2D>(metAltVsUncleaned, "metAltVsUncleaned", "MET;MuEGCleaned PF MET;Uncleaned PF MET;Counts", 100, 0, 500, 100, 0, 500);
  AddObject<CategorizedHist2D>(metVsAlt, "metVsAlt", "MET;Mono-Z Cleaned PF MET;MuEGCleaned PF MET;Counts", 100, 0, 500, 100, 0, 500);
  AddObject<CategorizedHist1D>(metFiltersMuonBug, "metFiltersMuonBug", "MET Filters;Filter;Counts", 2, 0, 2);
  metFiltersMuonBug->SetXaxisBinLabel(1, "Bad PF Muon");
  metFiltersMuonBug->SetXaxisBinLabel(2, "Duplicate PF Muon");

  AddObject<CategorizedHist1D>(leadingLepPt, "leadingLepPt", "Leading Lepton;Leading Lepton p_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(leadingLepEta, "leadingLepEta", "Leading Lepton;Leading Lepton #eta;Counts / 0.1", 50, -2.5, 2.5);
  AddObject<CategorizedHist1D>(trailingLepPt, "trailingLepPt", "Trailing Lepton;Trailing Lepton p_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(trailingLepEta, "trailingLepEta", "Trailing Lepton;Trailing Lepton #eta;Counts / 0.1", 50, -2.5, 2.5);

  AddObject<CategorizedHist1D>(Zmass, "Zmass", "Z Mass;m_{ll} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(Zpt,   "Zpt",   "Z Pt;q_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(Zeta,  "Zeta",  "Z Eta;#eta_{ll} [GeV];Counts / 0.1", 100, -5., 5.);
  AddObject<CategorizedHist1D>(Zrapidity,  "Zrapidity",  "Z Rapidity;y_{ll} [GeV];Counts / 0.1", 100, -5., 5.);
  AddObject<CategorizedHist2D>(ZdeltaRllvsPt, "ZdeltaRllvsPt", "DeltaR Z leptons;#DeltaR(l_{1}, l_{2});p_{T,Z};Counts", 20, 0, M_PI, 50, 0, 500);
  AddObject<CategorizedHist2D>(pfMet_nJets, "pfMet_nJets", "MET vs. jets;PF #slash{E}_{T} [GeV];# Jets (>20GeV);Counts", 100, 0, 500, 5, 0, 5);

  constexpr const char * cutFlowNames[] = {
    "Trigger+Category",
    "passZmass",
    "passBveto",
    "passTauVeto",
    "nJetsGood30",
    "WZCRrealMETcut",
    "WZCRm3lcut",
    "WZCRm2lcut",
    "ZZCRsecondZcut",
    "ZZCRtightSecondZcut",
    "passZpt",
    "passMET",
    "passDphiZMET",
    "passBalanceCut",
    "passDphiJetMET",
    "passDeltaRll",
  };
  constexpr size_t nCuts = sizeof(cutFlowNames) / sizeof(char *);
  static_assert((size_t) passAll==nCuts-1, "Double-check cutflow names!");

  AddObject<CategorizedHist1D>(cutFlow, "cutFlow", "Old Analyzer Cutflow", nCuts+1, 0, nCuts+1);
  AddObject<CategorizedHist1D>(cutFlowUnweighted, "cutFlowUnweighted", "Old Analyzer Cutflow, Raw Counts", nCuts+1, 0, nCuts+1);
  for(size_t i=0; i<nCuts; ++i) {
    cutFlow->SetXaxisBinLabel(1+i, cutFlowNames[i]);
    cutFlowUnweighted->SetXaxisBinLabel(1+i, cutFlowNames[i]);
  }

  AddObject<CategorizedHist1D>(cutFlow_Zmass, "cutFlow_Zmass", "Z Mass;m_{ll} [GeV];Counts / 1 GeV", 300, 0, 300);
  AddObject<CategorizedHist1D>(cutFlow_BVeto, "cutFlow_BVeto", "B jets;Number B-tagged jets;Counts", 5, -0.5, 4.5);
  AddObject<CategorizedHist1D>(cutFlow_TauVeto, "cutFlow_TauVeto", "Taus;Number taus;Counts", 5, -0.5, 4.5);
  AddObject<CategorizedHist1D>(cutFlow_nJets, "cutFlow_nJets", "Jets;Number jets;Counts", 7, -0.5, 6.5);
  AddObject<CategorizedHist2D>(xJet_xZ_jet20, "xJet_xZ_jet20", "x_jet vs x_Z;x_{j};x_{Z}", 100, 0., 1.0, 100, 0., 1.0);
  AddObject<CategorizedHist2D>(xJet_xZ_jet30, "xJet_xZ_jet30", "x_jet vs x_Z;x_{j};x_{Z}", 100, 0., 1.0, 100, 0., 1.0);
  AddObject<CategorizedHist1D>(cutFlow_realMETcutCR, "cutFlow_realMETcutCR", "Real MET WZ control region;PF #slash{E}_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(cutFlow_secondZcutCR, "cutFlow_secondZcutCR", "Second Z Mass ZZ control region;m_{ll,2} [GeV];Counts / 2 GeV", 100, 0, 200);
  AddObject<CategorizedHist1D>(multiLeptonMass, "multiLeptonMass", "All lepton Mass;m_{3l/4l} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(cutFlow_Zpt, "cutFlow_Zpt", "Z Pt;p_{T,ll} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(metResponse, "metResponse", "MET Response;u_{#parallel} / p_{T,ll};Counts / 0.05 ", 40, -1., 1.);
  AddObject<CategorizedHist1D>(cutFlow_MET, "cutFlow_MET", "MET;PF #slash{E}_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(cutFlow_dPhiZMET, "cutFlow_dPhiZMET", "DeltaPhi;#Delta#phi(Z, #slash{E}_{T});Counts * 20/#pi", 20, 0, M_PI);
  AddObject<CategorizedHist1D>(cutFlow_balance, "cutFlow_balance", "Balance;|1-#slash{E}_{T}/p_{T,ll}|;Counts / .05", 20, 0, 1);
  AddObject<CategorizedHist1D>(cutFlow_dPhiJetMET, "cutFlow_dPhiJetMET", "JetMET dphi;#Delta#phi(j_{1}, #slash{E}_{T});Counts *20/#pi", 20, 0, M_PI);
  AddObject<CategorizedHist1D>(cutFlow_deltaRll, "cutFlow_deltaRll", "DeltaR Z leptons;#DeltaR(l_{1}, l_{2});Counts *20/#pi", 20, 0, M_PI);

  AddObject<CategorizedHist1D>(nMinus1_Zmass, "nMinus1_Zmass", "Z Mass;m_{ll} [GeV];Counts / 1 GeV", 300, 0, 300);
  AddObject<CategorizedHist1D>(nMinus1_BVeto, "nMinus1_BVeto", "B jets;Number B-tagged jets;Counts", 5, -0.5, 4.5);
  AddObject<CategorizedHist1D>(nMinus1_TauVeto, "nMinus1_TauVeto", "Taus;Number taus;Counts", 5, -0.5, 4.5);
  AddObject<CategorizedHist1D>(nMinus1_nJets, "nMinus1_nJets", "Jets;Number jets;Counts", 7, -0.5, 6.5);
  AddObject<CategorizedHist1D>(nMinus1_nJets_xJet20, "nMinus1_nJets_xJet20", "xJet;x_{jet>20};Counts", 50, 0., 1.);
  AddObject<CategorizedHist1D>(nMinus1_nJets_xJet30, "nMinus1_nJets_xJet30", "xJet;x_{jet>30};Counts", 50, 0., 1.);
  AddObject<CategorizedHist1D>(nMinus1_realMETcutCR, "nMinus1_realMETcutCR", "Real MET WZ control region;PF #slash{E}_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(nMinus1_secondZcutCR, "nMinus1_secondZcutCR", "Second Z Mass ZZ control region;m_{ll,2} [GeV];Counts / 2 GeV", 100, 0, 200);
  AddObject<CategorizedHist1D>(nMinus1_Zpt, "nMinus1_Zpt", "Z Pt;p_{T,ll} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(nMinus1_MET, "nMinus1_MET", "MET;PF #slash{E}_{T} [GeV];Counts / 10 GeV", 120, 0, 1200);
  AddObject<CategorizedHist1D>(nMinus1_dPhiZMET, "nMinus1_dPhiZMET", "DeltaPhi;#Delta#phi(Z, #slash{E}_{T});Counts / 0.1", 32, 0, 3.2);
  AddObject<CategorizedHist1D>(nMinus1_balance, "nMinus1_balance", "Balance;|1-#slash{E}_{T}/p_{T,ll}|;Counts / .05", 20, 0, 1);
  AddObject<CategorizedHist1D>(nMinus1_dPhiJetMET, "nMinus1_dPhiJetMET", "JetMET dphi;#Delta#phi(j_{1}, #slash{E}_{T});Counts / 0.1", 32, 0, 3.2);
  AddObject<CategorizedHist1D>(nMinus1_deltaRll, "nMinus1_deltaRll", "DeltaR Z leptons;#DeltaR(l_{1}, l_{2});Counts / 0.1", 32, 0, 3.2);

  AddObject<CategorizedHist1D>(nMinus2_dPhiZMET_balance_response, "nMinus2_dPhiZMET_balance_response", "response;u_{#parallel} / p_{T,ll};Counts / 0.05 ", 40, -1., 1.);
  AddObject<CategorizedHist1D>(nMinus2_nJets_dPhiJetMET_xJet20, "nMinus2_nJets_dPhiJetMET_xJet20", "xJet;x_{jet>20};Counts", 50, 0., 1.);

  AddObject<CategorizedHist1D>(metCheck_dPhiJetMet_failBalance, "metCheck_dPhiJetMet_failBalance", "JetMET dphi;#Delta#phi(j_{1}, #slash{E}_{T});Counts / 0.1", 32, 0, 3.2);
  AddObject<CategorizedHist1D>(metCheck_pfVsCaloMet_failBalance, "metCheck_pfVsCaloMet_failBalance", "PF vs. Calo MET;|#slash{E}_{T}^{PF}-#slash{E}_{T}^{Calo}|/#slash{E}_{T}^{PF};Counts / 0.1", 20, 0., 2.);
  AddObject<CategorizedHist1D>(metCheck_dPhiJetMet_failDphi, "metCheck_dPhiJetMet_failDphi", "JetMET dphi;#Delta#phi(j_{1}, #slash{E}_{T});Counts / 0.1", 32, 0, 3.2);
  AddObject<CategorizedHist1D>(metCheck_pfVsCaloMet_failDphi, "metCheck_pfVsCaloMet_failDphi", "PF vs. Calo MET;|#slash{E}_{T}^{PF}-#slash{E}_{T}^{Calo}|/#slash{E}_{T}^{PF};Counts / 0.1", 20, 0., 2.);

  AddObject<CategorizedHist2D>(pfMet_systs, "pfMet_systs", "PF MET; PF #slash{E}_{T} [GeV];Systematic;Counts / 10 GeV", 150, 50, 800, EventTupleUtil::NSystematics, 0, EventTupleUtil::NSystematics);
  for(int syst=EventTupleUtil::SystNominal; syst<EventTupleUtil::NSystematics; ++syst) {
    pfMet_systs->SetYaxisBinLabel(1+syst, EventTupleUtil::SystematicNames.at(static_cast<EventTupleUtil::Systematic>(syst)));
  }
  AddObject<CategorizedHist2D>(pfMet_lheWeights, "pfMet_lheWeights", "PF MET; PF #slash{E}_{T} [GeV];LHE Weight index;Counts / 10 GeV", 150, 50, 800, 111, 0, 111);
  AddObject<CategorizedHist1D>(realPfMet, "realPfMet", "PF MET; PF #slash{E}_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(genMetHist, "genMetHist", "Generator MET;Generator #slash{E}_{T} [GeV];Counts / 5 GeV", 100, 0, 500);
  AddObject<CategorizedHist1D>(triggerBitsFinal, "triggerBitsFinal", "Trigger bit groups fired;Trigger group;Counts", 5, 0, 5);
  triggerBitsFinal->SetXaxisBinLabel(1+EventTuple::TriggerDoubleMuon,      "DoubleMuon");
  triggerBitsFinal->SetXaxisBinLabel(1+EventTuple::TriggerSingleMuon,      "SingleMuon");
  triggerBitsFinal->SetXaxisBinLabel(1+EventTuple::TriggerDoubleElectron,  "DoubleElectron");
  triggerBitsFinal->SetXaxisBinLabel(1+EventTuple::TriggerSingleElectron,  "SingleElectron");
  triggerBitsFinal->SetXaxisBinLabel(1+EventTuple::TriggerMuonElectron,    "MuonEG");

  //AddObject<TTree>(finalSelection, "finalSelection", "Final selection");
  //finalSelection->Branch("run", &eventList.run, "run/l",2560000);
  //finalSelection->Branch("lumi", &eventList.lumi, "lumi/l",2560000);
  //finalSelection->Branch("event", &eventList.event, "event/l",2560000);
  //finalSelection->Branch("channel", &eventList.channel,2560000);
  //finalSelection->Branch("pfMet", &eventList.pfMet,2560000);
  //finalSelection->Branch("Zmass", &eventList.Zmass,2560000);

  AddObject<TTree>(preSelection, "preSelection", "Pre selection");
  preSelection->Branch("prerun", &eventList.prerun, "run/l",102400000);
  preSelection->Branch("prelumi", &eventList.prelumi, "lumi/l",102400000);
  preSelection->Branch("preevent", &eventList.preevent, "event/l",102400000);
  preSelection->Branch("prechannel", &eventList.prechannel,102400000);
  preSelection->Branch("prepfMet", &eventList.prepfMet,102400000);
  preSelection->Branch("preZmass", &eventList.preZmass,102400000);
  preSelection->Branch("Zpt_Reco", &eventList.Zpt_Reco,102400000);
  preSelection->Branch("DeltaPhiZMet", &eventList.DeltaPhiZMet,102400000);
  preSelection->Branch("Balance", &eventList.Balance,102400000);
  //preSelection->Branch("DeltaRll", &eventList.DeltaRll,102400000);
  preSelection->Branch("DeltaPhill", &eventList.DeltaPhill,102400000);
  preSelection->Branch("DeltaEtall", &eventList.DeltaEtall,102400000);
  preSelection->Branch("LeadingPtll", &eventList.ll_leadingLepPt, 102400000);
  preSelection->Branch("TrailingPtll", &eventList.ll_trailingLepPt, 102400000);
  preSelection->Branch("LeadingEtall", &eventList.ll_leadingLepEta, 102400000);
  preSelection->Branch("TrailingEtall", &eventList.ll_trailingLepEta, 102400000);
  preSelection->Branch("Etall", &eventList.ll_Zeta, 102400000);
  preSelection->Branch("Rapidityll", &eventList.ll_Zrapidity, 102400000);
  preSelection->Branch("weight", &eventList.weight, 102400000);
  preSelection->Branch("ngoodjets", &eventList.ngoodjets, 102400000);
  preSelection->Branch("LeadingJetPt", &eventList.jj_leadingPt, 102400000);
  preSelection->Branch("subLeadingJetPt", &eventList.jj_subleadingPt, 102400000);
  preSelection->Branch("LeadingJetEta", &eventList.jj_leadingEta, 102400000);
  preSelection->Branch("subLeadingJetEta", &eventList.jj_subleadingEta, 102400000);
  preSelection->Branch("DijetMass", &eventList.dijet_mass, 102400000);
  //preSelection->Branch("DeltaRjj", &eventList.DeltaRjj, 102400000);
  preSelection->Branch("DeltaPhijj", &eventList.DeltaPhijj, 102400000);
  preSelection->Branch("DeltaEtajj", &eventList.DeltaEtajj, 102400000);
  preSelection->Branch("Zeppenfeld", &eventList.Zeppenfeld, 102400000);
  preSelection->Branch("nBjets", &eventList.nBjets, 102400000);
  preSelection->Branch("xJet", &eventList.xJet, 102400000);
  preSelection->Branch("xZ", &eventList.xZ, 102400000);  
  preSelection->Branch("genBJets", &eventList.genBJets, 102400000);
/*
  AddObject<TTree>(unfoldingtest, "unfoldingtest", "Unfoldingtest");
  unfoldingtest->Branch("Zpt_Gen", &ufd.ptgen, "Zpt_Gen/F", 25600000);
  unfoldingtest->Branch("Zpt_Reco", &ufd.ptrec, "Zpt_Reco/F", 25600000);
  unfoldingtest->Branch("PassSel", &ufd.passSel, "PassSel/I", 25600000);
  unfoldingtest->Branch("Weight", &ufd.weight, "Weight/F", 25600000);
  unfoldingtest->Branch("PassMET", &ufd.PassMET, "PassMET/I", 25600000);
  unfoldingtest->Branch("PassDphiZMET", &ufd.PassDphiZMET, "PassDphiZMET/I", 25600000);
  unfoldingtest->Branch("PassBalanceCut", &ufd.PassBalanceCut, "PassBalanceCut/I", 25600000);
  unfoldingtest->Branch("PassDeltaRll", &ufd.PassDeltaRll, "PassDeltaRll/I", 25600000);
*/
}

Bool_t MonoZSelector::Process(Long64_t entry)
{
  // Must call base class process (sets reader entry, counter)
  if ( ! SelectorBase::Process(entry) ) return false;

  if ( !skipSystematics ) {
    for(int syst=EventTupleUtil::SystNominal; syst<EventTupleUtil::NSystematics; ++syst) {
      ProcessSystematic(static_cast<EventTupleUtil::Systematic>(syst));
      if ( printEventList && syst == EventTupleUtil::SystNominal ) {
        std::cout << std::endl;
      }
    }
  } else {
    ProcessSystematic(EventTupleUtil::SystNominal);
    if ( printEventList ) {
      std::cout << std::endl;
    }
  }

  return true;
}

void MonoZSelector::ProcessSystematic(EventTupleUtil::Systematic systematic)
{
  using namespace EventTupleUtil;

  // Convention: for histograms not labeled x_systs,
  // only fill them for SystNominal
  const bool isNominal{systematic==SystNominal};

  double weight = 1.0;
  if ( isNominal ) weightsMonitor->Fill(WeightTypes::StartingWeight, weight);

  if ( isNominal && printEventList ) {
    std::cout <<
      *run << "," <<
      *lumi << "," <<
      *event << "," <<
      *metFilters << ",";
  }

  // Check MET Filters (flags are true if passing filter)
  // Latest recommendations at https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2
  constexpr int filterMask{0
      | bit(EventTuple::Flag_HBHENoiseFilter)
      | bit(EventTuple::Flag_HBHENoiseIsoFilter)
      | bit(EventTuple::Flag_EcalDeadCellTriggerPrimitiveFilter)
      | bit(EventTuple::Flag_goodVertices)
      | bit(EventTuple::Flag_eeBadScFilter)
      | bit(EventTuple::Flag_globalTightHalo2016Filter)
      // No longer recommended
      // | bit(EventTuple::Flag_CSCTightHalo2015Filter)
      | bit(EventTuple::Flag_BadChargedCandidateFilter)
      | bit(EventTuple::Flag_BadPFMuonFilter)
  };
  if ( (*metFilters & filterMask) != filterMask ) return;
  if ( isNominal ) weightsMonitor->Fill(WeightTypes::METFilter, weight);

  // Generator & Pileup weight
  double pileupWeight{1.};
  double kFactorWeight{1.};
  if ( !isRealData() ) {
    weight *= (*genWeight > 0) ? 1. : -1.;

    // Phase space cuts for overlapping datasets
    if ( isDataset(MC_DY_LOinclusive) || isDataset(MC_DY_NLOinclusive) || isDataset(MC_DY_NLOpt50to100) || isDataset(MC_DY) ) {
      // Remove Tau-tau events that are in DYJetsToTauTau_ForcedMuEleDecay
      int nLeptonicTauLegs{0};
      bool hasTau{false};
      for(size_t i=0; i<genParticle_id.GetSize(); ++i) {
        // The GenPromptTauDecay flag ( GenParticle::isDirectPromptTauDecayProductFinalState() )
        // is not working as expected, but if we find a tau and two e or mu neutrinos
        // then it is a fully leptonic DY->tautau event
        if ( std::abs(genParticle_id[i]) == 15 ) hasTau |= true;
        if ( (std::abs(genParticle_id[i])==12) || (std::abs(genParticle_id[i])==14) )
        {
          nLeptonicTauLegs++;
        }
      }
      if ( hasTau && isNominal ) dyTauTauCutCheck->Fill(nLeptonicTauLegs, weight);
      if ( hasTau && nLeptonicTauLegs == 2 ) return;
    }
    if ( isNominal ) dyStitchCheck->Fill(*lheZpt, weight);
    if ( isDataset(MC_DY_LOinclusive) ) {
      // Remove LHE Z pt above first binned sample
      // We have two options, 100 or 150
      // since there is a 150-inf sample and a 100-200,200-inf set
      //if ( *lheZpt > 100 ) return;
    }
    else if ( isDataset(MC_DY_NLOinclusive) ) {
      // First bin is 50-100 but because of the way aMC@NLO does shower matching,
      // the cut is not exactly 50 and there are enetries with negative weights below
      // See checkDYstitching.py in genStudies
      //if ( *lheZpt > 70 ) return;
    }
    else if ( isDataset(MC_DY_NLOpt50to100) ) {
      //if ( *lheZpt < 70 ) return;
    }

    if ( isNominal ) weightsMonitor->Fill(WeightTypes::GenWeight, weight);

    const double pileupValue = ( nvtxPUreweighting ) ? *nPrimaryVerticesCut : *nTruePileup;
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

    // Any k-factor corrections here
    if ( isDataset(MC_qqZZ_powheg) ) {
      // TODO: separate EWK and QCD?
      DibosonCorrections::TheoryUncertainty shift{DibosonCorrections::CentralValue};
      if ( systematic == SystKFactorsUp ) shift = DibosonCorrections::kEWK_up;
      else if ( systematic == SystKFactorsDown ) shift = DibosonCorrections::kEWK_down;
      //weight *= dibosonCorrector->getQCDNNLOcorrection(this); // QCD NNLO corrections 
      //weight *= dibosonCorrector->getEWcorrection(this, shift); // EW NLO corrections 
      kFactorWeight *= dibosonCorrector->getVVcorrection(this, shift); // QCD NNLO + EW NLO corrections

      auto trailingV = genParticle_p4[2];
      if ( genParticle_p4[3].Pt() < trailingV.Pt() ) trailingV = genParticle_p4[3];
      if ( isNominal ) vvKfactorCheck->Fill(trailingV.Pt(), dibosonCorrector->getEWcorrection(this), weight);
    }
    else if ( isDataset(MC_WZ_powheg) ) {
      DibosonCorrections::TheoryUncertainty shift{DibosonCorrections::CentralValue};
      if ( systematic == SystKFactorsUp ) shift = DibosonCorrections::kEWK_up;
      else if ( systematic == SystKFactorsDown ) shift = DibosonCorrections::kEWK_down;
      kFactorWeight *= dibosonCorrector->getEWcorrection(this, shift);

      auto trailingV = genParticle_p4[2];
      if ( genParticle_p4[3].Pt() < trailingV.Pt() ) trailingV = genParticle_p4[3];
      if ( isNominal ) vvKfactorCheck->Fill(trailingV.Pt(), dibosonCorrector->getEWcorrection(this), weight);
    }

    weight *= kFactorWeight;
    if ( isNominal ) weightsMonitor->Fill(WeightTypes::KFactorWeight, weight);
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

  // As we build objects, if shifting for a systematic, add negative shift to MET
  Vector2D systShiftsForMet;

  // Choose tight-quality e/mu for event categorization
  if ( isNominal ) nAllLeptons->Fill(muon_id.GetSize()+electron_id.GetSize(), weight);
  std::vector<Lepton> goodLeptons;
  for (size_t i=0; i<muon_id.GetSize(); ++i) {
    LorentzVector p4 = muon_p4[i];
    if ( systematic == SystMuonEnUp ) applyObjectShift(p4, muon_p4Error[i], true);
    else if ( systematic == SystMuonEnDown ) applyObjectShift(p4, muon_p4Error[i], false);

    int ids = muon_id[i];
    float iso = muon_relIsoDBetaR04[i];
    float dxy = muon_dxy[i];
    float dz = muon_dz[i];
    bool passesIP{ fabs(dxy) < 0.02 && fabs(dz) < 0.1 };
    if ( p4.Pt() >= 20 && fabs(p4.Eta()) <= 2.4 && passes(ids, EventTuple::MuonTight) && passesIP && iso < 0.15 ) {
      goodLeptons.emplace_back(p4, 13*muon_q[i]);
      systShiftsForMet -= Vector2D(p4) - Vector2D(muon_p4[i]);
    }
  }
  for (size_t i=0; i<electron_id.GetSize(); ++i) {
    LorentzVector p4 = electron_p4_raw[i];
    if ( systematic == SystElectronEnUp ) applyObjectShift(p4, electron_p4Error[i], true);
    else if ( systematic == SystElectronEnDown ) applyObjectShift(p4, electron_p4Error[i], false);

    int ids = electron_id[i];
    int charge = electron_q[i];
    if ( p4.Pt() >= 20 && fabs(p4.Eta()) <= 2.5 && passes(ids, EventTuple::ElectronMedium) ) {
      // Electron fake muon? Unlikely
      if ( overlaps(goodLeptons, p4, 0.01) ) {
        if ( isNominal ) weirdStuffCounter->Fill(ElectronOverlapsMuon);
        continue;
      }
      goodLeptons.emplace_back(p4, 11*charge);
      systShiftsForMet -= Vector2D(p4) - Vector2D(electron_p4_raw[i]);
    }
  }
  if ( isNominal ) nGoodLeptons->Fill(goodLeptons.size(), weight);

  if ( isNominal ) nChadLeptons->Fill(goodLeptons.size(), weight);  


  // Find any remaining e/mu that pass looser selection
  std::vector<Lepton> extraLeptons;
  for (size_t i=0; i<muon_id.GetSize(); ++i) {
    LorentzVector p4 = muon_p4[i];
    if ( systematic == SystMuonEnUp ) applyObjectShift(p4, muon_p4Error[i], true);
    else if ( systematic == SystMuonEnDown ) applyObjectShift(p4, muon_p4Error[i], false);

    if ( overlaps(goodLeptons, p4, 0.01) ) continue;
    int ids = muon_id[i];
    float iso = muon_relIsoDBetaR04[i];
    if ( p4.Pt() > 10 && fabs(p4.Eta()) <= 2.4 && passes(ids, EventTuple::MuonLoose) && iso < 0.25 ) {
      extraLeptons.emplace_back(p4, 13*muon_q[i]);
      systShiftsForMet -= Vector2D(p4) - Vector2D(muon_p4[i]);
    }
  }
  for (size_t i=0; i<electron_id.GetSize(); ++i) {
    LorentzVector p4 = electron_p4_raw[i];
    if ( systematic == SystElectronEnUp ) applyObjectShift(p4, electron_p4Error[i], true);
    else if ( systematic == SystElectronEnDown ) applyObjectShift(p4, electron_p4Error[i], false);

    if ( overlaps(goodLeptons, p4, 0.01) ) continue;
    int ids = electron_id[i];
    if ( p4.Pt() > 10 && fabs(p4.Eta()) <= 2.5 && (passes(ids, EventTuple::ElectronMedium)||passes(ids, EventTuple::ElectronVeto)) ) {
      extraLeptons.emplace_back(p4, 11*electron_q[i]);
      systShiftsForMet -= Vector2D(p4) - Vector2D(electron_p4_raw[i]);
    }
  }
  if ( isNominal ) nExtraLeptons->Fill(extraLeptons.size(), weight);
  if ( isNominal ) nLooseTightLeptons->Fill(extraLeptons.size(), goodLeptons.size(), weight);

  if ( isNominal && printEventList ) {
    std::cout <<
      goodLeptons.size() << "," <<
      extraLeptons.size() << ",";
  }

  // Find event category
  EventCategory eventCategory{CatUnknown};
  bool eventHasTriggerSeed{false};
  LorentzVector Zcand;
  Lepton leadingLep{Zcand, 0};
  Lepton trailingLep{Zcand, 0};
  Vector2D emulatedMet;
  LorentzVector emulatedMETp4;
  if ( goodLeptons.size() < 2 ) {
    // Future fake rate region?
    return;
  }
  else if ( goodLeptons.size() == 2 && extraLeptons.size() == 0 ) {
    // Z signal region
    if ( goodLeptons[0].pdgId * goodLeptons[1].pdgId == -11*11 ) {
      eventCategory = CatEE;
      if ( seedsEEcat ) eventHasTriggerSeed = true;
    }
    else if ( goodLeptons[0].pdgId * goodLeptons[1].pdgId == -11*13 ) {
      eventCategory = CatEM;
      if ( seedsEMcat ) eventHasTriggerSeed = true;
    }
    else if ( goodLeptons[0].pdgId * goodLeptons[1].pdgId == -13*13 ) {
      eventCategory = CatMM;
      if ( seedsMMcat ) eventHasTriggerSeed = true;
    }
    else {
      // Same-sign, future control region?
      return;
    }
    Zcand = goodLeptons[0].p4 + goodLeptons[1].p4;
    leadingLep = goodLeptons[0];
    trailingLep = goodLeptons[1];
    if ( trailingLep.p4.Pt() > leadingLep.p4.Pt() ) std::swap(leadingLep, trailingLep);
  }
  else if ( goodLeptons.size() == 3 && extraLeptons.size() == 0 ) {
    // WZ control region
    // Only take tight leptons due to too many fakes from DY
    for(size_t i=0; i<goodLeptons.size(); ++i) {
      for(size_t j=i+1; j<goodLeptons.size(); ++j) {
        // Check if OSSF Pair, then see if better Z mass
        if ( goodLeptons[i].pdgId == -1 * goodLeptons[j].pdgId ) {
          LorentzVector candTemp = goodLeptons[i].p4 + goodLeptons[j].p4;
          if ( fabs(candTemp.M()-nominalZmass) < fabs(Zcand.M()-nominalZmass) ) {
            Zcand = candTemp;
            leadingLep = goodLeptons[i];
            trailingLep = goodLeptons[j];
            if ( trailingLep.p4.Pt() > leadingLep.p4.Pt() ) std::swap(leadingLep, trailingLep);

            // third lepton for fake met
            size_t k = 3-i-j;
            emulatedMet = Vector2D(goodLeptons[k].p4);
            emulatedMETp4 = goodLeptons[k].p4;

            if ( std::abs(goodLeptons[i].pdgId) == 11 ) {
              eventCategory = CatEEL;
              // TODO: need something akin to seedsEELcat but
              // want to allow third lepton to fire trigger
              // What we should do is just trigger match leptons
              eventHasTriggerSeed = true;
            }
            else if ( std::abs(goodLeptons[i].pdgId) == 13 ) {
              eventCategory = CatMML;
              eventHasTriggerSeed = true;
            }
          }
        }
      }
    }
  }

  else if ( goodLeptons.size() >=2 && goodLeptons.size() + extraLeptons.size() == 4 ) {
    // ZZ control region
    // Promote loose leptons to good ones
    for(const auto& lep : extraLeptons) goodLeptons.push_back(lep);
    extraLeptons.clear();
    for(size_t i=0; i<goodLeptons.size(); ++i) {
      for(size_t j=i+1; j<goodLeptons.size(); ++j) {
        // Check if OSSF Pair, then see if better Z mass
        if ( goodLeptons[i].pdgId == -1 * goodLeptons[j].pdgId ) {
          LorentzVector candTemp = goodLeptons[i].p4 + goodLeptons[j].p4;
          if ( fabs(candTemp.M()-nominalZmass) < fabs(Zcand.M()-nominalZmass) ) {
            // Find extra leptons for fake met
            Lepton Z2LeadingLep{LorentzVector(), 0};
            Lepton Z2TrailingLep{LorentzVector(), 0};
            for(size_t k=0; k<goodLeptons.size(); ++k) {
              if ( k != i && k != j ) {
                if ( goodLeptons[k].p4.Pt() > Z2LeadingLep.p4.Pt() ) {
                  Z2TrailingLep = Z2LeadingLep;
                  Z2LeadingLep = goodLeptons[k];
                }
                else {
                  Z2TrailingLep = goodLeptons[k];
                }
              }
            }

            // Make sure second Z is OSSF as well
            if ( Z2LeadingLep.pdgId != -1 * Z2TrailingLep.pdgId ) continue;

            Zcand = candTemp;
            leadingLep = goodLeptons[i];
            trailingLep = goodLeptons[j];
            if ( trailingLep.p4.Pt() > leadingLep.p4.Pt() ) std::swap(leadingLep, trailingLep);
            emulatedMet = Vector2D(Z2LeadingLep.p4) + Vector2D(Z2TrailingLep.p4);
            emulatedMETp4 = Z2LeadingLep.p4 + Z2TrailingLep.p4;

            if ( std::abs(goodLeptons[i].pdgId) == 11 ) {
              eventCategory = CatEELL;
              // TODO: need something akin to seedsEELcat but
              // want to allow third lepton to fire trigger
              // What we should do is just trigger match leptons
              eventHasTriggerSeed = true;
            }
            else if ( std::abs(goodLeptons[i].pdgId) == 13 ) {
              eventCategory = CatMMLL;
              eventHasTriggerSeed = true;
            }
          }
        }
      }
    }
    // In case no combination is satisfactory
    if ( Zcand.M() == 0. ) return;
  }
  else {
    // Too many leptons!
    return;
  }
  categories_systs->Fill(eventCategory, systematic, weight);

  // FIXME: To sync old framework, but will we keep it / does it matter?
  // Check after Z pt cut
  if ( eventCategory == CatEE && leadingLep.p4.Pt() < 25. ) return;


  // Find any taus, but clean against tight e/mu
  std::vector<Lepton> extraTaus;
  for (size_t i=0; i<tau_id.GetSize(); ++i) {
    LorentzVector p4 = tau_p4[i];
    int ids = tau_id[i];
    int charge = tau_q[i];
    if ( overlaps(goodLeptons, p4, 0.4) ) continue;
    if ( p4.Pt() > 18 && fabs(p4.Eta()) <= 2.3 && passes(ids, EventTuple::TauDecayModeFinding) ) {
      extraTaus.emplace_back(p4, 15*charge);
    }
  }
  if ( isNominal ) nLooseTightLeptonsWTau->Fill(extraLeptons.size()+extraTaus.size(), goodLeptons.size(), weight);


  // Jet counting
  // TODO: future hope to remove jet count vetos
  // in favor of hadronic activity cut.  See slide 28:
  // https://agenda.hep.wisc.edu/event/965/session/0/contribution/0/material/slides/0.pdf
  std::vector<LorentzVector> goodJets;
  int nBjetsCSVmedium{0};
  double bTagScaleFactor{1.};
  const char * bSystName = ( systematic == SystBtagUp ) ? "up" : ( systematic == SystBtagDown ) ? "down" : "central";
  double et_jets20{0.};
  double et_jets30{0.};
  for (size_t i=0; i<jet_id.GetSize(); ++i) {
    LorentzVector p4;
    if ( systematic == SystJetEnUp ) p4 = jet_p4_JetEnUp[i];
    if ( systematic == SystJetEnDown ) p4 = jet_p4_JetEnDown[i];
    if ( systematic == SystJetResUp ) p4 = jet_p4_JetResUp[i];
    if ( systematic == SystJetResDown ) p4 = jet_p4_JetResDown[i];
    else p4 = jet_p4[i];

    int ids = jet_id[i];
    float btagMVAvalue = jet_btagCSVv2[i];
    int flavor = ( isRealData() ) ? 0 : jet_hadronFlavor[i];

    if ( p4.Pt() <= 20 || fabs(p4.Eta()) > 5 || !passes(ids, EventTuple::JetLoose) ) continue;
    if ( overlaps(goodLeptons, p4, 0.4) ) continue;

    // CSVv2 medium for Moriond17
    // https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation80XReReco#Supported_Algorithms_and_Operati
    const float btagDiscriminatorWP = 0.8484;
    if ( fabs(p4.Eta()) <= 2.4 && btagMVAvalue > btagDiscriminatorWP ) {
      nBjetsCSVmedium++;
    }

    // Following prescription 1a in https://twiki.cern.ch/twiki/bin/view/CMS/BTagSFMethods
    // See makeScaleFactors.py for the scale factor source
    if ( !isRealData() && fabs(p4.Eta()) <= 2.4 ) {
      double SF{1.};
      double tagEff{0.};

      // Hadron flavor is 0,4,5 corresponding to the gen jet type
      // So (flavor%3) gives 0=light, 1=charm, 2=bottom
      if ( isNominal && doBtagEfficiency ) btagEffDenominator->Fill((double) (flavor%3), p4.Pt(), p4.Eta(), weight);
      if ( flavor == 5 ) {
        SF = btagReader->eval_auto_bounds(bSystName, BTagEntry::FLAV_B, p4.Eta(), p4.Pt());
        if ( btagEfficiency == nullptr ) tagEff = 0.64; // dummy value taken from quick check on TTbar
        else tagEff = btagEfficiency->Evaluate3D(flavor%3, p4.Pt(), p4.Eta());
      }
      else if ( flavor == 4 ) {
        SF = btagReader->eval_auto_bounds(bSystName, BTagEntry::FLAV_C, p4.Eta(), p4.Pt());
        if ( btagEfficiency == nullptr ) tagEff = 0.15; // dummy value taken from quick check on TTbar, DY (about the same)
        else tagEff = btagEfficiency->Evaluate3D(flavor%3, p4.Pt(), p4.Eta());
      }
      else if ( flavor == 0 ) {
        SF = btagReader->eval_auto_bounds(bSystName, BTagEntry::FLAV_UDSG, p4.Eta(), p4.Pt());
        if ( btagEfficiency == nullptr ) tagEff = 0.045; // dummy value taken from quick check: 0.054 for TTbar, 0.038 for DY
        else tagEff = btagEfficiency->Evaluate3D(flavor%3, p4.Pt(), p4.Eta());
      }

      if ( btagMVAvalue > btagDiscriminatorWP ) {
        // efficiency cancels out
        bTagScaleFactor *= SF;
        if ( isNominal && doBtagEfficiency ) btagEffNumerator->Fill((double) (flavor%3), p4.Pt(), p4.Eta(), weight);
      } else {
        // Note: singular at tagEff=1, ~ SF + (1-SF)/(1-tagEff)
        bTagScaleFactor *= (1.-SF*tagEff)/(1.-tagEff);
      }
    }

    et_jets20 += p4.Et();
    if ( p4.Pt() >= 30 ) {
      goodJets.emplace_back(p4);
      et_jets30 += p4.Et();
    }
  }

  weight *= bTagScaleFactor;
  if ( isNominal ) {
    nGoodJets->Fill(goodJets.size(), weight);
    nBtagMediumJets->Fill(nBjetsCSVmedium, weight);
    weightsMonitor->Fill(WeightTypes::BTagWeight, weight);
  }


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
    leadingLepPtPreSF->Fill(eventCategory, leadingLep.p4.Pt(), weight);
    leadingLepEtaPreSF->Fill(eventCategory, leadingLep.p4.Eta(), weight);
    trailingLepPtPreSF->Fill(eventCategory, trailingLep.p4.Pt(), weight);
    trailingLepEtaPreSF->Fill(eventCategory, trailingLep.p4.Eta(), weight);
  }


  // Apply relevant lepton scale factors
  double leptonSf{1.};
  if ( !isRealData() ) {
    if ( std::abs(leadingLep.pdgId) == 11 ) {
      auto shift = ScaleFactor::CentralValue;
      if ( systematic == SystElectronSFUp )   shift = ScaleFactor::ShiftUp;
      if ( systematic == SystElectronSFDown ) shift = ScaleFactor::ShiftDown;
      leptonSf *= electronGsfSF->Evaluate2D(leadingLep.p4.Eta(), leadingLep.p4.Pt(), shift);
      leptonSf *= electronIdSF->Evaluate2D(leadingLep.p4.Eta(), leadingLep.p4.Pt(), shift);
      if ( isNominal ) weightsMonitor->Fill(WeightTypes::LeadingLepID, weight);
    }
    else if ( std::abs(leadingLep.pdgId) == 13 ) {
      auto shift = ScaleFactor::CentralValue;
      if ( systematic == SystMuonSFUp )   shift = ScaleFactor::ShiftUp;
      if ( systematic == SystMuonSFDown ) shift = ScaleFactor::ShiftDown;
      leptonSf *= muonIdSF->Evaluate2D(fabs(leadingLep.p4.Eta()), leadingLep.p4.Pt(), shift);
      leptonSf *= muonIsoSF->Evaluate2D(fabs(leadingLep.p4.Eta()), leadingLep.p4.Pt(), shift);
      if ( isNominal ) weightsMonitor->Fill(WeightTypes::LeadingLepID, weight);
    }

    if ( std::abs(trailingLep.pdgId) == 11 ) {
      auto shift = ScaleFactor::CentralValue;
      if ( systematic == SystElectronSFUp )   shift = ScaleFactor::ShiftUp;
      if ( systematic == SystElectronSFDown ) shift = ScaleFactor::ShiftDown;
      leptonSf *= electronGsfSF->Evaluate2D(trailingLep.p4.Eta(), trailingLep.p4.Pt(), shift);
      leptonSf *= electronIdSF->Evaluate2D(trailingLep.p4.Eta(), trailingLep.p4.Pt(), shift);
      if ( isNominal ) weightsMonitor->Fill(WeightTypes::TrailingLepID, weight);
    }
    else if ( std::abs(trailingLep.pdgId) == 13 ) {
      auto shift = ScaleFactor::CentralValue;
      if ( systematic == SystMuonSFUp )   shift = ScaleFactor::ShiftUp;
      if ( systematic == SystMuonSFDown ) shift = ScaleFactor::ShiftDown;
      leptonSf *= muonIdSF->Evaluate2D(fabs(trailingLep.p4.Eta()), trailingLep.p4.Pt(), shift);
      leptonSf *= muonIsoSF->Evaluate2D(fabs(trailingLep.p4.Eta()), trailingLep.p4.Pt(), shift);
      if ( isNominal ) weightsMonitor->Fill(WeightTypes::TrailingLepID, weight);
    }

    weight *= leptonSf;
  }

  const Vector2D met = getMet(systematic) + systShiftsForMet;
  const Vector2D metForCuts = met + emulatedMet;

  //const float balance = fabs(1-metForCuts.R()/Zcand.Pt());
  //const float balance = fabs(metForCuts.R()-Zcand.Pt())/Zcand.Pt();
  const float balance = metForCuts.R()/Zcand.Pt()- 1;  
  const float deltaPhiZMet = fabs(DeltaPhi(Zcand, metForCuts));
  Vector2D recoil = - metForCuts - Vector2D(Zcand);
  recoil.Rotate(-Zcand.Phi());
  const float response = recoil.X() / Zcand.Pt();
  float jetMetMinDphi{999.};
  for(const auto& jet : goodJets) {
    if ( fabs(DeltaPhi(jet, metForCuts)) < jetMetMinDphi )
      jetMetMinDphi = fabs(DeltaPhi(jet, metForCuts));
  }
  float minMass2l{999.};
  for(size_t i=0; i<goodLeptons.size(); ++i) {
    for(size_t j=i+1; j<goodLeptons.size(); ++j) {
      float mass2l = (goodLeptons[i].p4 + goodLeptons[j].p4).M();
      if ( mass2l < minMass2l )
        minMass2l = mass2l;
    }
  }

  //working with jets information(Chad Freer)
  LorentzVector leadingJet;
  float leading{0.};
  LorentzVector subleadingJet;
  float subleading{0.};
  //finding the leading jet 
  if (goodJets.size() > 1){ 
    for(size_t i=0; i<goodJets.size(); ++i) {
        if (goodJets[i].pt() > leading) {
           leading = goodJets[i].pt();
           leadingJet = goodJets[i];
        }
     }
  //Now subleading
     for(size_t i=0; i<goodJets.size(); ++i) {
        if (goodJets[i].pt() > subleading && goodJets[i].pt() < leading) {
          subleading = goodJets[i].pt();
          subleadingJet = goodJets[i];
        }   
     }
  }  
  const float jj_mass = (leadingJet + subleadingJet).M();  
  //const float deltaRjj = DeltaR(leadingJet, subleadingJet);
  const float deltaPhijj = fabs(DeltaPhi(leadingJet, subleadingJet));
  //const float deltaEtajj = fabs(DeltaEta(leadingJet, subleadingJet));
  const float deltaEtajj = fabs(leadingJet.Eta() - subleadingJet.Eta());
  const float zeppenfeld = (Zcand.Eta()-(leadingJet.Eta()+subleadingJet.Eta())/2)/deltaEtajj;
  //end jet work

  const float deltaRll = DeltaR(leadingLep.p4, trailingLep.p4);
  const float deltaPhill = fabs(DeltaPhi(leadingLep.p4, trailingLep.p4));
  const float deltaEtall = fabs(leadingLep.p4.Eta() - trailingLep.p4.Eta());  
  // x_jet vs. x_Z (Eqn. 7,8 of https://arxiv.org/pdf/1410.4840.pdf)
  const float x_denom20 = ( et_jets20 + metForCuts.R() + Zcand.Et() );
  const float x_jet20 = et_jets20 / x_denom20;
  const float x_denom30 = ( et_jets30 + metForCuts.R() + Zcand.Et() );
  const float x_jet30 = et_jets30 / x_denom30;
  const float x_Z = Zcand.Et() / x_denom20;
  // PF vs. Calo MET for MET stability checks
  double pfToCaloMetRatio = fabs(met.R()-caloMet->R())/met.R();

  //std::vector<LorentzVector> goodJets;
  int genBjets{0};
  //double bTagScaleFactor{1.};
  //const char * bSystName = ( systematic == SystBtagUp ) ? "up" : ( systematic == SystBtagDown ) ? "down" : "central";
  for (size_t i=0; i<jet_id.GetSize(); ++i) {
    LorentzVector p4;
    //if ( systematic == SystJetEnUp ) p4 = jet_p4_JetEnUp[i];
    //if ( systematic == SystJetEnDown ) p4 = jet_p4_JetEnDown[i];
    //if ( systematic == SystJetResUp ) p4 = jet_p4_JetResUp[i];
    //if ( systematic == SystJetResDown ) p4 = jet_p4_JetResDown[i];
    //else p4 = jet_p4[i];
    int flavor2 = ( isRealData() ) ? 0 : jet_hadronFlavor[i];
  
    if ( isNominal && flavor2 ==5 ) {
       genBjets++;
    }
  }




  if ( isNominal  && !skipSystematics){// && isRealData() && !skipSystematics ) {
    eventList.prerun = *run;
    eventList.prelumi = *lumi;
    eventList.preevent = *event;
    eventList.prechannel = eventCategory;// CatEE=2, CatMM=4, CatEM=8, CatEEL=16, CatMML=32, CatEELL=64, CatMMLL=128
    eventList.prepfMet = metForCuts.R();
    eventList.preZmass = Zcand.M();
    eventList.Zpt_Reco = Zcand.Pt();
    eventList.DeltaPhiZMet = deltaPhiZMet;
    eventList.Balance = balance;
    //eventList.DeltaRll = deltaRll;
    eventList.DeltaPhill = deltaPhill;
    eventList.DeltaEtall = deltaEtall;
    eventList.ll_leadingLepPt = leadingLep.p4.Pt();
    eventList.ll_trailingLepPt = trailingLep.p4.Pt();
    eventList.ll_leadingLepEta = leadingLep.p4.Eta();
    eventList.ll_trailingLepEta = trailingLep.p4.Eta();;
    eventList.ll_Zeta = Zcand.Eta();
    eventList.ll_Zrapidity = Zcand.Rapidity();
    eventList.weight = weight;
    eventList.ngoodjets = goodJets.size();
    eventList.jj_leadingPt = leadingJet.pt();
    eventList.jj_subleadingPt = subleadingJet.pt();
    eventList.jj_leadingEta = leadingJet.Eta();
    eventList.jj_subleadingEta = subleadingJet.Eta();
    eventList.dijet_mass = jj_mass;
    //eventList.DeltaRjj =deltaRjj;
    eventList.DeltaPhijj = deltaPhijj;
    eventList.DeltaEtajj = deltaEtajj;
    eventList.Zeppenfeld = zeppenfeld;
    eventList.nBjets = nBjetsCSVmedium;
    eventList.xJet = x_jet20;
    eventList.xZ = x_Z;
    eventList.genBJets = genBjets;
    preSelection->Fill(); 

  }
 //Chad Looking at Gen level information for LLDM
   // for( size_t i = 0; i < genParticle_p4.GetSize(); i++ ) {
  //ufd.PassMET=0;
  //ufd.PassDphiZMET=0;
  //ufd.PassBalanceCut=0;
  //ufd.PassDeltaRll=0;

  int packedSelection{0};
  if ( fabs(Zcand.M() - nominalZmass) < 15. )
    packedSelection |= bit(passZmass);
  if ( nBjetsCSVmedium == 0 )
    packedSelection |= bit(passBveto);
  if ( extraTaus.size() == 0 )
    packedSelection |= bit(passTauVeto);
  if ( goodJets.size() == 2 )
    packedSelection |= bit(nJetsGood30);
  if ( ((eventCategory & CatLLL) && met.R() > 30.) || (eventCategory & ~CatLLL) )
    packedSelection |= bit(WZCRrealMETcut);
  if ( ((eventCategory & CatLLL) && (Zcand+emulatedMETp4).M() > 100.) || (eventCategory & ~CatLLL) )
    packedSelection |= bit(WZCRm3lcut);
  if ( ((eventCategory & CatLLL) && minMass2l > 4.) || (eventCategory & ~CatLLL) )
    packedSelection |= bit(WZCRm2lcut);
  if ( ((eventCategory & CatLLLL) && emulatedMETp4.M() > 60 && emulatedMETp4.M() < 120) || (eventCategory & ~CatLLLL) )
    packedSelection |= bit(ZZCRsecondZcut);
  if ( ((eventCategory & CatLLLL) && fabs(emulatedMETp4.M() - nominalZmass) < 15) || (eventCategory & ~CatLLLL) )
    packedSelection |= bit(ZZCRtightSecondZcut);
  if ( Zcand.Pt() > 0. )
    packedSelection |= bit(passZpt);
  if ( metForCuts.R() > 0. ) //ufd.PassMET = 1;
    packedSelection |= bit(passMET);
  if ( deltaPhiZMet > 0 ) //ufd.PassDphiZMET = 1;
    packedSelection |= bit(passDphiZMET);
  if ( balance < 2 ) //ufd.PassBalanceCut = 1;
    packedSelection |= bit(passBalanceCut);
  if ( jetMetMinDphi > 0.5 ) 
    packedSelection |= bit(passDphiJetMET);
  if ( deltaRll < 2.6 ) //ufd.PassDeltaRll = 1;
    packedSelection |= bit(passDeltaRll);

  // Define here "all cuts" for any given event category
  int allCutsMask{bit(passAll)-1};
  if ( eventCategory & (CatLL|CatEM) ) {
    // passAll is ok since CR cuts evaluate true outside CR
  }
  else if ( eventCategory & CatLLL ) {
    // Turn off Tau, B vetos
    allCutsMask ^= bit(passTauVeto);
    allCutsMask ^= bit(passBveto);
  }
  else if ( eventCategory & CatLLLL ) {
    // Turn off Tau, B vetos
    allCutsMask ^= bit(passTauVeto);
    allCutsMask ^= bit(passBveto);
    // Allow loose (60-120) second Z if this is uncommented
    // allCutsMask ^= bit(ZZCRtightSecondZcut);
  }

  // Function that checks all cuts but ignores cut specified
  auto passAllBut = [packedSelection, allCutsMask](const int cut) -> bool { return ((packedSelection | bit(cut)) & allCutsMask) == allCutsMask; };

  if ( isNominal ) {
    if ( passes(packedSelection, passZpt) && passes(packedSelection, passZmass) ) {
      // Trigger efficiency
      triggerEfficiencyPostZpt->Fill(eventCategory, 0, weight);
      // Lepton pt after Z boost
      leadingLepPtPostZpt->Fill(eventCategory, leadingLep.p4.Pt(), weight);
      trailingLepPtPostZpt->Fill(eventCategory, trailingLep.p4.Pt(), weight);

      if ( eventHasTriggerSeed ) {
        triggerEfficiencyPostZpt->Fill(eventCategory, 1, weight);
        leadingLepPtPostZptWtrigger->Fill(eventCategory, leadingLep.p4.Pt(), weight);
        trailingLepPtPostZptWtrigger->Fill(eventCategory, trailingLep.p4.Pt(), weight);

        metVsAlt->Fill(eventCategory, met.R(), altPfMet->R(), weight);
        if ( isRealData() ) {
          metVsUncleaned->Fill(eventCategory, met.R(), uncleanedPfMet->R(), weight);
          metAltVsUncleaned->Fill(eventCategory, altPfMet->R(), uncleanedPfMet->R(), weight);
          if ( *metFilters & bit(EventTuple::Flag_BadMuonsFromHIP2016) )
            metFiltersMuonBug->Fill(eventCategory, 0, weight);
          if ( *metFilters & bit(EventTuple::Flag_DuplicateMuonsFromHIP2016) )
            metFiltersMuonBug->Fill(eventCategory, 1, weight);
        }
      }
    }

    if ( !eventHasTriggerSeed ) goto postCutflow;

    // Cutflow, n-1, and preselection level plots
    leadingLepPt->Fill(eventCategory, leadingLep.p4.Pt(), weight);
    leadingLepEta->Fill(eventCategory, leadingLep.p4.Eta(), weight);
    trailingLepPt->Fill(eventCategory, trailingLep.p4.Pt(), weight);
    trailingLepEta->Fill(eventCategory, trailingLep.p4.Eta(), weight);

    Zmass->Fill(eventCategory, Zcand.M(), weight);
    if ( passes(packedSelection, passZmass) ) {
      nvtx->Fill(eventCategory, *nPrimaryVerticesCut, weight);
      rho->Fill(eventCategory, *fixedGridRhoAll, weight);
      Zpt->Fill(eventCategory, Zcand.Pt(), weight);
      Zeta->Fill(eventCategory, Zcand.Eta(), weight);
      Zrapidity->Fill(eventCategory, Zcand.Rapidity(), weight);
      ZdeltaRllvsPt->Fill(eventCategory, DeltaR(leadingLep.p4, trailingLep.p4), Zcand.Pt(), weight);
      pfMet_nJets->Fill(eventCategory, metForCuts.R(), goodJets.size(), weight);
      for(auto&& lep : goodLeptons) {
        if ( std::abs(lep.pdgId) == 11 ) {
          for(size_t i=0; i<electron_p4.GetSize(); ++i) {
            if ( DeltaR(electron_p4[i], lep.p4) < 0.01 ) {
              electronCorrection->Fill(electron_p4[i].Pt(), lep.p4.Pt(), weight);
            }
          }
        }
      }
    }

    // n-1 plots here
    if ( passAllBut(passZmass) ) nMinus1_Zmass->Fill(eventCategory, Zcand.M(), weight);
    if ( passAllBut(passBveto) ) nMinus1_BVeto->Fill(eventCategory, nBjetsCSVmedium, weight);
    if ( passAllBut(passTauVeto) ) nMinus1_TauVeto->Fill(eventCategory, extraTaus.size(), weight);
    if ( passAllBut(nJetsGood30) ) nMinus1_nJets->Fill(eventCategory, goodJets.size(), weight);
    if ( passAllBut(nJetsGood30) ) nMinus1_nJets_xJet20->Fill(eventCategory, x_jet20, weight);
    if ( passAllBut(nJetsGood30) ) nMinus1_nJets_xJet30->Fill(eventCategory, x_jet30, weight);
    if ( passAllBut(WZCRrealMETcut) ) nMinus1_realMETcutCR->Fill(eventCategory, met.R(), weight);
    if ( passAllBut(ZZCRsecondZcut) ) nMinus1_secondZcutCR->Fill(eventCategory, emulatedMETp4.M(), weight);
    if ( passAllBut(passZpt) ) nMinus1_Zpt->Fill(eventCategory, Zcand.Pt(), weight);
    if ( passAllBut(passMET) ) nMinus1_MET->Fill(eventCategory, metForCuts.R(), weight);
    if ( passAllBut(passDphiZMET) ) nMinus1_dPhiZMET->Fill(eventCategory, deltaPhiZMet, weight);
    if ( passAllBut(passBalanceCut) ) nMinus1_balance->Fill(eventCategory, balance, weight);
    if ( passAllBut(passDphiJetMET) ) nMinus1_dPhiJetMET->Fill(eventCategory, jetMetMinDphi, weight);
    if ( passAllBut(passDeltaRll) ) nMinus1_deltaRll->Fill(eventCategory, deltaRll, weight);

    if ( (packedSelection | bit(passBalanceCut) | bit(passDphiZMET)) == (bit(passAll)-1) ) {
      nMinus2_dPhiZMET_balance_response->Fill(eventCategory, response, weight);
    }
    if ( (packedSelection | bit(nJetsGood30) | bit(passDphiJetMET)) == (bit(passAll)-1) ) {
      nMinus2_nJets_dPhiJetMET_xJet20->Fill(eventCategory, x_jet20, weight);
    }

    // MET Sanity regions
    int mask = bit(passZmass) | bit(passZpt) | bit(nJetsGood30) | bit(passMET) | bit(passBalanceCut);
    int require = mask ^ bit(passBalanceCut);
    if ( (packedSelection & mask) == require ) {
      metCheck_dPhiJetMet_failBalance->Fill(eventCategory, jetMetMinDphi, weight);
      metCheck_pfVsCaloMet_failBalance->Fill(eventCategory, pfToCaloMetRatio, weight);
    }
    mask = bit(passZmass) | bit(passZpt) | bit(nJetsGood30) | bit(passMET) | bit(passDphiZMET);
    require = mask ^ bit(passDphiZMET);
    if ( (packedSelection & mask) == require ) {
      metCheck_dPhiJetMet_failDphi->Fill(eventCategory, jetMetMinDphi, weight);
      metCheck_pfVsCaloMet_failDphi->Fill(eventCategory, pfToCaloMetRatio, weight);
    }

    // Cut flow starting here
    cutFlow->Fill(eventCategory, 0., weight);
    cutFlowUnweighted->Fill(eventCategory, 0., 1.);

    cutFlow_Zmass->Fill(eventCategory, Zcand.M(), weight);
    if ( !passes(packedSelection, passZmass) ) goto postCutflow;
    cutFlow->Fill(eventCategory, passZmass+1., weight);
    cutFlowUnweighted->Fill(eventCategory, passZmass+1., 1.);

    cutFlow_BVeto->Fill(eventCategory, nBjetsCSVmedium, weight);
    if ( !passes(packedSelection, passBveto) ) goto postCutflow;
    cutFlow->Fill(eventCategory, passBveto+1., weight);
    cutFlowUnweighted->Fill(eventCategory, passBveto+1., 1.);

    cutFlow_TauVeto->Fill(eventCategory, extraTaus.size(), weight);
    if ( !passes(packedSelection, passTauVeto) ) goto postCutflow;
    cutFlow->Fill(eventCategory, passTauVeto+1., weight);
    cutFlowUnweighted->Fill(eventCategory, passTauVeto+1., 1.);

    xJet_xZ_jet20->Fill(eventCategory, et_jets20/x_denom20, Zcand.Et()/x_denom20);
    xJet_xZ_jet30->Fill(eventCategory, et_jets30/x_denom30, Zcand.Et()/x_denom30);

    cutFlow_nJets->Fill(eventCategory, goodJets.size(), weight);
    if ( !passes(packedSelection, nJetsGood30) ) goto postCutflow;
    cutFlow->Fill(eventCategory, nJetsGood30+1., weight);
    cutFlowUnweighted->Fill(eventCategory, nJetsGood30+1., 1.);

    cutFlow_realMETcutCR->Fill(eventCategory, met.R(), weight);
    if ( !passes(packedSelection, WZCRrealMETcut) ) goto postCutflow;
    cutFlow->Fill(eventCategory, WZCRrealMETcut+1., weight);
    cutFlowUnweighted->Fill(eventCategory, WZCRrealMETcut+1., 1.);

    cutFlow_secondZcutCR->Fill(eventCategory, emulatedMETp4.M(), weight);
    if ( !passes(packedSelection, ZZCRsecondZcut) ) goto postCutflow;
    cutFlow->Fill(eventCategory, ZZCRsecondZcut+1., weight);
    cutFlowUnweighted->Fill(eventCategory, ZZCRsecondZcut+1., 1.);

    // TODO: Investigate m3l or m4l cut to increase purity in control region
    // (remove fakes due to loose ID)
    multiLeptonMass->Fill(eventCategory, (Zcand+emulatedMETp4).M(), weight);

    cutFlow_Zpt->Fill(eventCategory, Zcand.Pt(), weight);
    if ( !passes(packedSelection, passZpt) ) goto postCutflow;
    cutFlow->Fill(eventCategory, passZpt+1., weight);
    cutFlowUnweighted->Fill(eventCategory, passZpt+1., 1.);

    metResponse->Fill(eventCategory, response, weight);

    cutFlow_MET->Fill(eventCategory, metForCuts.R(), weight);
    if ( !passes(packedSelection, passMET) ) goto postCutflow;
    cutFlow->Fill(eventCategory, passMET+1., weight);
    cutFlowUnweighted->Fill(eventCategory, passMET+1., 1.);

    cutFlow_dPhiZMET->Fill(eventCategory, fabs(DeltaPhi(Zcand, metForCuts)), weight);
    if ( !passes(packedSelection, passDphiZMET) ) goto postCutflow;
    cutFlow->Fill(eventCategory, passDphiZMET+1., weight);
    cutFlowUnweighted->Fill(eventCategory, passDphiZMET+1., 1.);

    cutFlow_balance->Fill(eventCategory, balance, weight);
    if ( !passes(packedSelection, passBalanceCut) ) goto postCutflow;
    cutFlow->Fill(eventCategory, passBalanceCut+1., weight);
    cutFlowUnweighted->Fill(eventCategory, passBalanceCut+1., 1.);

    cutFlow_dPhiJetMET->Fill(eventCategory, jetMetMinDphi, weight);
    if ( !passes(packedSelection, passDphiJetMET) ) goto postCutflow;
    cutFlow->Fill(eventCategory, passDphiJetMET+1., weight);
    cutFlowUnweighted->Fill(eventCategory, passDphiJetMET+1., 1.);

    cutFlow_deltaRll->Fill(eventCategory, deltaRll, weight);
    if ( !passes(packedSelection, passDeltaRll) ) goto postCutflow;
    cutFlow->Fill(eventCategory, passDeltaRll+1., weight);
    cutFlowUnweighted->Fill(eventCategory, passDeltaRll+1., 1.);
  }
postCutflow:

  if ( isNominal && printEventList ) {
    std::cout <<
      // *run << "," <<
      // *lumi << "," <<
      // *event << "," <<
      // *metFilters << ",";
      // goodLeptons.size() << "," <<
      // extraLeptons.size() << ",";
      pileupWeight << "," <<
      kFactorWeight << "," <<
      bTagScaleFactor << "," <<
      leptonSf << "," <<
      eventCategory << "," << // CatEE=bit(1), CatMM=bit(3), CatEM=bit(2), CatEEL=bit(4), CatMML=bit(5), CatEELL=bit(6), CatMMLL=bit(7),
      met.R() << "," <<
      metForCuts.R() << "," << // Emulated MET
      Zcand.M() << "," <<
      Zcand.Pt() << "," <<
      jetMetMinDphi << "," <<
      eventHasTriggerSeed << "," <<
      passes(packedSelection, passZmass) << "," <<
      passes(packedSelection, passBveto) << "," <<
      passes(packedSelection, passTauVeto) << "," <<
      passes(packedSelection, nJetsGood30) << "," <<
      passes(packedSelection, WZCRrealMETcut) << "," <<
      passes(packedSelection, WZCRm3lcut) << "," <<
      passes(packedSelection, WZCRm2lcut) << "," <<
      passes(packedSelection, ZZCRsecondZcut) << "," <<
      passes(packedSelection, ZZCRtightSecondZcut) << "," <<
      passes(packedSelection, passZpt) << "," <<
      passes(packedSelection, passMET) << "," <<
      passes(packedSelection, passDphiZMET) << "," <<
      passes(packedSelection, passBalanceCut) << "," <<
      passes(packedSelection, passDphiJetMET) << "," <<
      passes(packedSelection, passDeltaRll) << ",";
  }

  //
  // Full selection after this point, except MET cut since fitting sideband (at 50)
  //
  if ( !eventHasTriggerSeed ) return;
  if ( !(passAllBut(passMET) && metForCuts.R() > 50.) ) return;

  pfMet_systs->Fill(eventCategory, metForCuts.R(), systematic, weight);

  if ( isNominal ) {
    if ( ! skipSystematics ) {
      for(size_t i=0; i<lheWeights.GetSize(); ++i) {
        pfMet_lheWeights->Fill(eventCategory, metForCuts.R(), i, weight * lheWeights[i] / lheWeights[0]);
      }
    }
    realPfMet->Fill(eventCategory, met.R(), weight);
    genMetHist->Fill(eventCategory, genMet->R(), weight);

    if ( hasTrigger(EventTuple::TriggerSingleElectron) ) {
      triggerBitsFinal->Fill(eventCategory, EventTuple::TriggerSingleElectron, weight);
    }
    if ( hasTrigger(EventTuple::TriggerDoubleElectron) ) {
      triggerBitsFinal->Fill(eventCategory, EventTuple::TriggerDoubleElectron, weight);
    }
    if ( hasTrigger(EventTuple::TriggerSingleMuon) ) {
      triggerBitsFinal->Fill(eventCategory, EventTuple::TriggerSingleMuon, weight);
    }
    if ( hasTrigger(EventTuple::TriggerDoubleMuon) ) {
      triggerBitsFinal->Fill(eventCategory, EventTuple::TriggerDoubleMuon, weight);
    }
    if ( hasTrigger(EventTuple::TriggerMuonElectron) ) {
      triggerBitsFinal->Fill(eventCategory, EventTuple::TriggerMuonElectron, weight);
    }
  }
/*
  if ( isNominal  && !skipSystematics){// && isRealData() && !skipSystematics ) {
    eventList.run = *run;
    eventList.lumi = *lumi;
    eventList.event = *event;
    eventList.channel = eventCategory;
    eventList.pfMet = metForCuts.R();
    eventList.Zmass = Zcand.M();
    finalSelection->Fill();
  }
*/
  /*
  bool isLepton = (eventCategory==CatEE || eventCategory==CatMM);
  if (isLepton) {
    ufd.ptgen = -1.;
    ufd.ptrec = -1.;
    ufd.passSel = 0;

    LorentzVector p4_lep(0.,0.,0.,0.);
    int nlep = 0;
    for( size_t i = 0; i < genParticle_p4.GetSize(); i++ ) {
      if( !(abs(genParticle_id.At(i))==13 || abs(genParticle_id.At(i))==11) ) continue;
      int flags = genParticle_flags.At(i);
      if( !EventTupleUtil::passes(flags,EventTuple::GenPromptFinalState) ) continue;
      p4_lep += genParticle_p4.At(i);
      nlep += 1;
    }
    if(nlep==2) ufd.ptgen = p4_lep.Pt();
    ufd.ptrec = Zcand.Pt();

    if (isNominal && packedSelection==allCutsMask) ufd.passSel = 1;
    ufd.weight = weight;
    unfoldingtest->Fill();
  }
  */
}

