// -*- C++ -*-
//
// Package:   Analysis/monoZ
// Class:    EventTupler
// 
/**\class EventTupler EventTupler.cc Analysis/monoZ/plugins/EventTupler.cc

 Description: [one line class summary]

 Implementation:
    [Notes on implementation]
*/
//
// Original Author:  Nicholas Charles Smith
//      Created:  Mon, 15 Aug 2016 13:43:49 GMT
//
//


// system include files
#include <memory>

// CMSSW include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/transform.h"

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/METReco/interface/MET.h"
#include "DataFormats/METReco/interface/METFwd.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"

// 
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

// ROOT include files
#include "TTree.h"
#include "Analysis/monoZ/interface/EventTuple.h"

namespace {
  // 'half-precision' floats, i.e. what MINIAOD does
  // Acutally they reduce the exponent and pack into 16 bits,
  // here we just rely on compression to remove the 13 bits since
  // we removed the entropy.  It ends up getting a factor ~1.7
  inline float half(float x) {
    union { float f; uint32_t u; };
    f = x;
    u &= 0xffffe000u; // 23 - 10 = 13 bits to throw away
    return f;
  }

  LorentzVector lorentz(const math::XYZTLorentzVector& vec) {
    return LorentzVector(vec.px(), vec.py(), vec.pz(), vec.energy());
    // return LorentzVector(half(vec.px()), half(vec.py()), half(vec.pz()), half(vec.energy()));
  }

  template<typename T>
  Vector3D vector3(const T& vec) {
    return Vector3D(vec.x(), vec.y(), vec.z());
    // return Vector3D(half(vec.x()), half(vec.y()), half(vec.z()));
  }

  Vector2D vector2(const math::XYZTLorentzVector& vec) {
    return Vector2D(vec.px(), vec.py());
    // return Vector2D(half(vec.px()), half(vec.py()));
  }
}

class EventTupler : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
  public:
    explicit EventTupler(const edm::ParameterSet&);
    ~EventTupler();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


  private:
    virtual void beginJob() override;
    virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
    virtual void endJob() override;

    void readMCOnlyProducts(const edm::Event& iEvent);
    void readLHEEvent(const LHEEventProduct& lheProduct);
    void readVertexInfo(const edm::Event& iEvent);
    void readEventQuantities(const edm::Event& iEvent);
    void readMETs(const edm::Event& iEvent);
    void readElectrons(const edm::Event& iEvent);
    void readMuons(const edm::Event& iEvent);
    void readPhotons(const edm::Event& iEvent);
    void readTaus(const edm::Event& iEvent);
    void readJets(const edm::Event& iEvent);
    void readMETFilters(const edm::Event& iEvent);
    void readTriggerBits(const edm::Event& iEvent);

    // ----------member data ---------------------------
    edm::ParameterSet config_;
    edm::EDGetTokenT<pat::METCollection> metToken_;
    edm::EDGetTokenT<pat::METCollection> altMetToken_;
    edm::EDGetTokenT<pat::METCollection> uncleanedMetToken_;
    edm::EDGetTokenT<pat::PackedCandidateCollection> packedToken_;
    edm::EDGetTokenT<reco::METCollection> trackMetToken_;
    std::vector<edm::EDGetTokenT<reco::METCollection>> subMetTokens_;
    edm::EDGetTokenT<pat::ElectronCollection> electronToken_;
    edm::EDGetTokenT<pat::MuonCollection> muonToken_;
    edm::EDGetTokenT<pat::PhotonCollection> photonToken_;
    edm::EDGetTokenT<pat::TauCollection> tauToken_;
    edm::EDGetTokenT<pat::JetCollection> jetToken_;
    edm::EDGetTokenT<reco::VertexCollection> vertexToken_;
    edm::EDGetTokenT<double> fixedGridRhoAllToken_;
    edm::EDGetTokenT<double> fixedGridRhoFastjetCentralCaloToken_;

    edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;
    StringCutObjectSelector<reco::GenParticle> genParticleCut_;
    edm::EDGetTokenT<GenEventInfoProduct> genEventInfoToken_;
    edm::EDGetTokenT<LHEEventProduct> lheEventToken_;
    unsigned int nLHEWeights_;
    edm::EDGetTokenT<std::vector<PileupSummaryInfo>> pileupSummaryInfoToken_;

    edm::EDGetTokenT<edm::TriggerResults> metFilterFlagsToken_;
    edm::EDGetTokenT<bool> badChCandFilterToken_;
    edm::EDGetTokenT<bool> badPFMuonFilterToken_;

    edm::EDGetTokenT<edm::TriggerResults> triggerBitsToken_;

    TTree * tree_;
    EventTuple event_;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
EventTupler::EventTupler(const edm::ParameterSet& iConfig):
  config_(iConfig),
  metToken_(consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("metSrc"))),
  altMetToken_(consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("altMetSrc"))),
  uncleanedMetToken_(consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("uncleanedMetSrc"))),
  packedToken_(consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("packedCandidatesSrc"))),
  trackMetToken_(consumes<reco::METCollection>(iConfig.getParameter<edm::InputTag>("trackMetSrc"))),
  subMetTokens_(edm::vector_transform(
        iConfig.getParameter<std::vector<edm::InputTag>>("subMetSrcVect"),
        [this](const edm::InputTag& tag){return consumes<reco::METCollection>(tag);}
      )),
  electronToken_(consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electronSrc"))),
  muonToken_(consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muonSrc"))),
  photonToken_(consumes<pat::PhotonCollection>(iConfig.getParameter<edm::InputTag>("photonSrc"))),
  tauToken_(consumes<pat::TauCollection>(iConfig.getParameter<edm::InputTag>("tauSrc"))),
  jetToken_(consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("jetSrc"))),
  vertexToken_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexSrc"))),
  fixedGridRhoAllToken_(consumes<double>(iConfig.getParameter<edm::InputTag>("fixedGridRhoAll"))),
  fixedGridRhoFastjetCentralCaloToken_(consumes<double>(iConfig.getParameter<edm::InputTag>("fixedGridRhoFastjetCentralCalo"))),
  genParticleToken_(consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("genParticleSrc"))),
  genParticleCut_(iConfig.getParameter<std::string>("genParticleCut")),
  genEventInfoToken_(consumes<GenEventInfoProduct>(iConfig.getParameter<edm::InputTag>("genEventInfoSrc"))),
  lheEventToken_(consumes<LHEEventProduct>(iConfig.getParameter<edm::InputTag>("lheEventSrc"))),
  nLHEWeights_(iConfig.getParameter<unsigned int>("nLHEWeights")),
  pileupSummaryInfoToken_(consumes<std::vector<PileupSummaryInfo>>(iConfig.getParameter<edm::InputTag>("pileupSrc"))),
  metFilterFlagsToken_(consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("metFilterFlagsSrc"))),
  badChCandFilterToken_(consumes<bool>(iConfig.getParameter<edm::InputTag>("BadChargedCandidateFilter"))),
  badPFMuonFilterToken_(consumes<bool>(iConfig.getParameter<edm::InputTag>("BadPFMuonFilter"))),
  triggerBitsToken_(consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("triggerBits")))
{
  usesResource("TFileService");
  edm::Service<TFileService> fs;

  tree_ = fs->make<TTree>("tree","Event Summary");

  // Alternative compression: LZMA level 4
  // Doesn't seem to work any better though
  // In fact, for some branches ZLIB level 1 works better
  // Usually the int flag branches work better with ZLIB
  // tree_->GetDirectory()->GetFile()->SetCompressionSettings(104);

  // Prevents "tree;2", etc. from happening
  // Some issue with TFileService and TFile key handling
  // causes this, but we probably won't want autoSave anyway
  tree_->SetAutoSave(-10000000000L); // 10GB

  // Want something short since tree_->Draw() will need full path
  tree_->Branch("e", &event_);

  // Tune basket sizes
  // In principle, one prefers the number of baskets
  // for each branch to be ~same, so that a given event
  // is likely to lie in the same basket for each branch
  // and thus the baskets for a group of events are close
  // to each other in the file.  In practice, idk
  // Haven't measured it yet
  tree_->SetBasketSize("genParticle*", 128000);
  tree_->SetBasketSize("lheWeights", 64000);

  // Check subMet list is consistent with EventTuple.h
  if ( subMetTokens_.size() != (size_t) EventTuple::NSubMetTypes ) {
    edm::LogError("EventTupler") << "subMetSrcVect is not the same length as EventTuple::SubMetType enum!";
  }
}


EventTupler::~EventTupler()
{
}


//
// member functions
//

// ------------ method called for each event  ------------
void
EventTupler::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  event_.run = iEvent.run();
  event_.lumi = iEvent.luminosityBlock();
  event_.event = iEvent.id().event();

  if ( !iEvent.isRealData() ) {
    readMCOnlyProducts(iEvent);
  }

  readVertexInfo(iEvent);

  readEventQuantities(iEvent);

  readMETs(iEvent);

  readElectrons(iEvent);

  readMuons(iEvent);

  readPhotons(iEvent);

  readTaus(iEvent);

  readJets(iEvent);

  readMETFilters(iEvent);

  readTriggerBits(iEvent);

  tree_->Fill();
  event_.clear();
}


// ------------ method called once each job just before starting event loop  ------------
void 
EventTupler::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
EventTupler::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
EventTupler::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

void
EventTupler::readMCOnlyProducts(const edm::Event& iEvent) {
  using namespace edm;

  Handle<reco::GenParticleCollection> genParticleHandle;
  iEvent.getByToken(genParticleToken_, genParticleHandle);
  const reco::GenParticleCollection &genParticles = *genParticleHandle.product();

  for (const auto& genParticle : genParticles) {
    if ( genParticleCut_(genParticle) )
    {
      event_.genParticle_p4.push_back(lorentz(genParticle.p4()));
      event_.genParticle_id.push_back(genParticle.pdgId());
      // There is some documentation in "DataFormats/HepMCCandidate/interface/GenParticle.h"
      event_.genParticle_flags.push_back(0
        | (genParticle.isPromptFinalState()<<EventTuple::GenPromptFinalState)
        | (genParticle.isHardProcess()<<EventTuple::GenHardProcess)
        | ((genParticle.status()==1)<<EventTuple::GenStatusOne)
        | (genParticle.isDirectPromptTauDecayProductFinalState()<<EventTuple::GenPromptTauDecay)
      );
    }
  }


  Handle<GenEventInfoProduct> genEventInfoHandle;
  iEvent.getByToken(genEventInfoToken_, genEventInfoHandle);
  const GenEventInfoProduct &genEvent = *genEventInfoHandle.product();

  event_.genWeight = genEvent.weight();


  Handle<LHEEventProduct> lheEventHandle;
  iEvent.getByToken(lheEventToken_, lheEventHandle);
  // In the event that we are processing a gen sample
  // made entirely in pythia (or sherpa/herwig), there
  // won't be an LHE product
  if ( lheEventHandle.isValid() ) {
    const LHEEventProduct &lheEvent = *lheEventHandle.product();
    readLHEEvent(lheEvent);
  }


  Handle<std::vector<PileupSummaryInfo>> pileupSummaryInfoHandle;
  iEvent.getByToken(pileupSummaryInfoToken_, pileupSummaryInfoHandle);
  const std::vector<PileupSummaryInfo> &pileupSummaryInfo = *pileupSummaryInfoHandle.product();

  event_.nOutOfTimePileup = 0.;
  event_.nAheadOfTimePileup = 0.;
  for(const auto& puInfo : pileupSummaryInfo) {
    if (puInfo.getBunchCrossing() == 0) {
      event_.nTruePileup = puInfo.getTrueNumInteractions();
      event_.nPoissonPileup = puInfo.getPU_NumInteractions();
    } else {
      event_.nOutOfTimePileup += puInfo.getPU_NumInteractions();
      if (puInfo.getBunchCrossing() < 0)
        event_.nAheadOfTimePileup += puInfo.getPU_NumInteractions();
    }
  }
}

void
EventTupler::readLHEEvent(const LHEEventProduct& lheEvent) {
  // lheWeight is (usually?) the same as genWeight
  // and always the same as lheWeights[0]
  // event_.lheWeight = lheEvent.originalXWGTUP();
  if ( nLHEWeights_ > 0 ) {
    for(const auto& w : lheEvent.weights()) {
      event_.lheWeights.push_back(w.wgt);
      // event_.lheWeights.push_back(half(w.wgt));
      if ( event_.lheWeights.size() >= (size_t) nLHEWeights_ ) break;
    }
  }

  // Useful guide: http://home.fnal.gov/~mrenna/041207_pythia_tutorial/05_041207_pythia_tutorial_TS_2_leshouches2.pdf
  const lhef::HEPEUP& lheeventinfo = lheEvent.hepeup();
  float sumPartonHT{0.};
  int nPartons{0};
  std::vector<LorentzVector> outgoingLeptons;
  for (int i=0; i<lheeventinfo.NUP; ++i) {
    const double px = lheeventinfo.PUP.at(i)[0];
    const double py = lheeventinfo.PUP.at(i)[1];
    const double pz = lheeventinfo.PUP.at(i)[2];
    const double pE = lheeventinfo.PUP.at(i)[3];
    const LorentzVector p4(px, py, pz, pE);
    const int status = lheeventinfo.ISTUP[i];
    const int pdgId = lheeventinfo.IDUP[i];

    if ( status == 1 && ( abs(pdgId) == 11 || abs(pdgId) == 13 || abs(pdgId) == 15) ) {
      outgoingLeptons.push_back(p4);
    }

    // final partons, quarks (except top) and gluon
    if ( status == 1 && (abs(pdgId) <= 5 || pdgId == 21) ) {
      nPartons++;
      sumPartonHT += p4.pt();
    }
  }
  event_.lheSumPartonHT = sumPartonHT;
  event_.lheNpartons = nPartons;
  if ( outgoingLeptons.size() == 2 ) {
    // This is used for DY stitching, probably will be meaningless for most other samples
    event_.lheZpt = (outgoingLeptons[0]+outgoingLeptons[1]).pt();
  } else {
    event_.lheZpt = 0.;
  }
}

void
EventTupler::readVertexInfo(const edm::Event& iEvent) {
  edm::Handle<reco::VertexCollection> vertexHandle;
  iEvent.getByToken(vertexToken_, vertexHandle);

  // Primary vertex choice: first in MINIAOD collection
  // See note in test/eventTuples.py
  const reco::Vertex &vertex = vertexHandle->at(0);
  event_.vertex = vector3(vertex);

  int npv{0};
  int npvCut{0};
  for(const auto& vertex : *vertexHandle) {
    // This appears to never happen, presumably MINIAOD filters these
    if ( !vertex.isValid() || vertex.isFake() ) continue;
    npv++;

    // TODO: necessary? find some documentation about this?
    if( vertex.ndof()>4. && abs(vertex.z()) <= 24. && vertex.position().Rho() <= 2.) {
      npvCut++;
    }
  }
  if ( npv != (int) vertexHandle->size() ) {
    // In case isFake() ever does happen
    // Just mark npvCut so we know (no sense adding more stuff)
    npvCut = -npvCut;
  }

  event_.nPrimaryVertices = npv;
  event_.nPrimaryVerticesCut = npvCut;
}

void
EventTupler::readEventQuantities(const edm::Event& iEvent) {
  edm::Handle<double> fixedGridRhoAllHandle;
  iEvent.getByToken(fixedGridRhoAllToken_, fixedGridRhoAllHandle);
  event_.fixedGridRhoAll = *fixedGridRhoAllHandle;

  edm::Handle<double> fixedGridRhoFastjetCentralCaloHandle;
  iEvent.getByToken(fixedGridRhoFastjetCentralCaloToken_, fixedGridRhoFastjetCentralCaloHandle);
  event_.fixedGridRhoFastjetCentralCalo = *fixedGridRhoFastjetCentralCaloHandle;
}

void
EventTupler::readMETs(const edm::Event& iEvent) {
  edm::Handle<pat::METCollection> metHandle;
  iEvent.getByToken(metToken_, metHandle);
  const pat::MET &met = metHandle.product()->at(0);

  event_.metSignificance = met.metSignificance();
  auto inverted = met.getSignificanceMatrix();
  if ( !inverted.Invert() ) {
    edm::LogWarning("EventTupler") << "Failed to invert MET Significance matrix!";
  }
  event_.metSigMatrixInverse = inverted;

  event_.caloMet = Vector2D(met.caloMETP2().px, met.caloMETP2().py);

  event_.rawPfMet = vector2(met.corP4(pat::MET::Raw));

  event_.type1PfMet = vector2(met.p4());
  event_.type1PfSumEt = met.sumEt();

  event_.type1PfMet_JetResUp = vector2(met.shiftedP4(pat::MET::JetResUp)) - event_.type1PfMet;
  event_.type1PfMet_JetResDown = vector2(met.shiftedP4(pat::MET::JetResDown)) - event_.type1PfMet;
  event_.type1PfMet_JetEnUp = vector2(met.shiftedP4(pat::MET::JetEnUp)) - event_.type1PfMet;
  event_.type1PfMet_JetEnDown = vector2(met.shiftedP4(pat::MET::JetEnDown)) - event_.type1PfMet;
  event_.type1PfMet_UnclusteredEnUp = vector2(met.shiftedP4(pat::MET::UnclusteredEnUp)) - event_.type1PfMet;
  event_.type1PfMet_UnclusteredEnDown = vector2(met.shiftedP4(pat::MET::UnclusteredEnDown)) - event_.type1PfMet;

  if ( !iEvent.isRealData() ) {
    // Of course this segfaults if not MC
    event_.genMet = vector2(met.genMET()->p4());
  }

  edm::Handle<pat::METCollection> altMetHandle;
  iEvent.getByToken(altMetToken_, altMetHandle);
  event_.altPfMet = vector2(altMetHandle->at(0).p4());
  event_.altRawPfMet = vector2(altMetHandle->at(0).corP4(pat::MET::Raw));

  if ( iEvent.isRealData() ) {
    edm::Handle<pat::METCollection> uncleanedMetHandle;
    iEvent.getByToken(uncleanedMetToken_, uncleanedMetHandle);
    event_.uncleanedPfMet = vector2(uncleanedMetHandle->at(0).p4());
  }

  edm::Handle<reco::METCollection> trackMetHandle;
  iEvent.getByToken(trackMetToken_, trackMetHandle);
  const reco::MET &trackMet = trackMetHandle.product()->at(0);
  event_.trackMet = vector2(trackMet.p4());

  for(const auto& token : subMetTokens_) {
    edm::Handle<reco::METCollection> subMetH;
    iEvent.getByToken(token, subMetH);
    const reco::MET &subMet = subMetH.product()->at(0);
    event_.subMets.push_back(vector2(subMet.p4()));
    event_.subMetSumEts.push_back(subMet.sumEt());
  }

  edm::Handle<pat::PackedCandidateCollection> packedHandle;
  iEvent.getByToken(packedToken_, packedHandle);
  event_.nPFconstituents = packedHandle->size();
}

void
EventTupler::readElectrons(const edm::Event& iEvent) {
  edm::Handle<pat::ElectronCollection> electronHandle;
  iEvent.getByToken(electronToken_, electronHandle);
  const pat::ElectronCollection &electrons = *electronHandle.product();
  for(const auto& electron : electrons) {
    auto p4kind = electron.candidateP4Kind();
    event_.electron_p4.push_back(lorentz(electron.p4(p4kind)));
    event_.electron_p4Error.push_back(electron.userFloat("p4Error"));
    event_.electron_p4_raw.push_back(lorentz(electron.userCand("raw")->p4()));
    event_.electron_q.push_back(electron.charge());
    
    event_.electron_id.push_back(0
      | (electron.userInt("VetoId")<<EventTuple::ElectronVeto)
      | (electron.userInt("LooseId")<<EventTuple::ElectronLoose)
      | (electron.userInt("MediumId")<<EventTuple::ElectronMedium)
      | (electron.userInt("TightId")<<EventTuple::ElectronTight)
      | (electron.userInt("HEEPV6Id")<<EventTuple::ElectronHEEPV6)
    );

    event_.electron_dxy.push_back(electron.userFloat("dxy"));
    event_.electron_dz.push_back(electron.userFloat("dz"));
  }
}

void
EventTupler::readMuons(const edm::Event& iEvent) {
  edm::Handle<pat::MuonCollection> muonHandle;
  iEvent.getByToken(muonToken_, muonHandle);
  const pat::MuonCollection &muons = *muonHandle.product();
  for(const auto& muon : muons) {
    event_.muon_p4.push_back(lorentz(muon.p4()));
    event_.muon_p4Error.push_back(muon.userFloat("p4Error"));
    event_.muon_q.push_back(muon.charge());

    event_.muon_id.push_back(0
      | (muon.isLooseMuon()<<EventTuple::MuonLoose)
      | (muon.isMediumMuon()<<EventTuple::MuonMedium)
      | (muon.userInt("isTightMuon")<<EventTuple::MuonTight)
      | (muon.userInt("isSoftMuon")<<EventTuple::MuonSoft)
      | (muon.userInt("isHighPtMuon")<<EventTuple::MuonHighPt)
      | (muon.userInt("isICHEPmedium")<<EventTuple::MuonICHEPmedium)
    );

    event_.muon_relIsoDBetaR04.push_back(muon.userFloat("relIsoDBetaR04"));
    event_.muon_trackIso.push_back(muon.trackIso());
    event_.muon_dxy.push_back(muon.userFloat("dxy"));
    event_.muon_dz.push_back(muon.userFloat("dz"));
  }
}

void
EventTupler::readPhotons(const edm::Event& iEvent) {
  edm::Handle<pat::PhotonCollection> photonHandle;
  iEvent.getByToken(photonToken_, photonHandle);
  const pat::PhotonCollection &photons = *photonHandle.product();
  for(const auto& photon : photons) {
    event_.photon_p4.push_back(lorentz(photon.p4()));
    event_.photon_p4Error.push_back(photon.userFloat("p4Error"));

    event_.photon_id.push_back(0
      | (photon.userInt("LooseId")<<EventTuple::PhotonLoose)
      | (photon.userInt("MediumId")<<EventTuple::PhotonMedium)
      | (photon.userInt("TightId")<<EventTuple::PhotonTight)
    );

    event_.photon_matchedTriggerEt.push_back(photon.userFloat("matchedTriggerEt"));
    event_.photon_matchedTriggerDeltaR.push_back(photon.userFloat("matchedTriggerDeltaR"));
    event_.photon_minPrescale.push_back(photon.userInt("minPrescale"));
  }
}

void
EventTupler::readTaus(const edm::Event& iEvent) {
  edm::Handle<pat::TauCollection> tauHandle;
  iEvent.getByToken(tauToken_, tauHandle);
  const pat::TauCollection &taus = *tauHandle.product();
  for(const auto& tau : taus) {
    event_.tau_p4.push_back(lorentz(tau.p4()));
    event_.tau_p4Error.push_back(tau.userFloat("p4Error"));
    event_.tau_q.push_back(tau.charge());

    event_.tau_id.push_back(0
      | ((bool) tau.tauID("decayModeFinding")<<EventTuple::TauDecayModeFinding)
      | ((bool) tau.tauID("byLooseCombinedIsolationDeltaBetaCorr3Hits")<<EventTuple::TauLooseCombinedIso3Hits)
      | ((bool) tau.tauID("byMediumCombinedIsolationDeltaBetaCorr3Hits")<<EventTuple::TauMediumCombinedIso3Hits)
      | ((bool) tau.tauID("byTightCombinedIsolationDeltaBetaCorr3Hits")<<EventTuple::TauTightCombinedIso3Hits)
    );
  }
}

void
EventTupler::readJets(const edm::Event& iEvent) {
  edm::Handle<pat::JetCollection> jetHandle;
  iEvent.getByToken(jetToken_, jetHandle);
  const pat::JetCollection &jets = *jetHandle.product();
  for(const auto& jet : jets) {
    event_.jet_p4.push_back(lorentz(jet.p4()));
    if ( !iEvent.isRealData() ) {
      event_.jet_p4_JetResUp.push_back(lorentz(jet.userCand("JetResUp")->p4()));
      event_.jet_p4_JetResDown.push_back(lorentz(jet.userCand("JetResDown")->p4()));
    } else {
      event_.jet_p4_JetResUp.push_back(LorentzVector());
      event_.jet_p4_JetResDown.push_back(LorentzVector());
    }
    event_.jet_p4_JetEnUp.push_back(lorentz(jet.userCand("JetEnUp")->p4()));
    event_.jet_p4_JetEnDown.push_back(lorentz(jet.userCand("JetEnDown")->p4()));
    
    event_.jet_id.push_back(0
      | (jet.userInt("looseJetId")<<EventTuple::JetLoose)
      | (jet.userInt("tightJetId")<<EventTuple::JetTight)
    );

    event_.jet_btagCSVv2.push_back(jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"));
    if ( !iEvent.isRealData() ) {
      event_.jet_hadronFlavor.push_back(jet.hadronFlavour());
    }
  }
}

void
EventTupler::readMETFilters(const edm::Event& iEvent) {
  edm::Handle<edm::TriggerResults> bitsH;
  iEvent.getByToken(metFilterFlagsToken_, bitsH);
  const edm::TriggerNames &names = iEvent.triggerNames(*bitsH);
  // for(auto name : names.triggerNames()) std::cout << name << std::endl;

  edm::Handle<bool> ifilterbadChCand;
  iEvent.getByToken(badChCandFilterToken_, ifilterbadChCand);
  edm::Handle<bool> ifilterbadPFMuon;
  iEvent.getByToken(badPFMuonFilterToken_, ifilterbadPFMuon);

  int filters = 0;
  filters |= (bitsH->accept(names.triggerIndex("Flag_HBHENoiseFilter"))<<EventTuple::Flag_HBHENoiseFilter);
  filters |= (bitsH->accept(names.triggerIndex("Flag_HBHENoiseIsoFilter"))<<EventTuple::Flag_HBHENoiseIsoFilter);
  filters |= (bitsH->accept(names.triggerIndex("Flag_EcalDeadCellTriggerPrimitiveFilter"))<<EventTuple::Flag_EcalDeadCellTriggerPrimitiveFilter);
  filters |= (bitsH->accept(names.triggerIndex("Flag_goodVertices"))<<EventTuple::Flag_goodVertices);
  filters |= (bitsH->accept(names.triggerIndex("Flag_eeBadScFilter"))<<EventTuple::Flag_eeBadScFilter);
  filters |= (bitsH->accept(names.triggerIndex("Flag_globalTightHalo2016Filter"))<<EventTuple::Flag_globalTightHalo2016Filter);
  filters |= (bitsH->accept(names.triggerIndex("Flag_globalSuperTightHalo2016Filter"))<<EventTuple::Flag_globalSuperTightHalo2016Filter);
  filters |= (bitsH->accept(names.triggerIndex("Flag_CSCTightHalo2015Filter"))<<EventTuple::Flag_CSCTightHalo2015Filter);
  if ( iEvent.isRealData() ) {
    filters |= (bitsH->accept(names.triggerIndex("Flag_badMuons"))<<EventTuple::Flag_BadMuonsFromHIP2016);
    filters |= (bitsH->accept(names.triggerIndex("Flag_duplicateMuons"))<<EventTuple::Flag_DuplicateMuonsFromHIP2016);
  }
  filters |= (*ifilterbadChCand<<EventTuple::Flag_BadChargedCandidateFilter);
  filters |= (*ifilterbadPFMuon<<EventTuple::Flag_BadPFMuonFilter);

  event_.metFilters = filters;
}

void
EventTupler::readTriggerBits(const edm::Event& iEvent) {
  edm::Handle<edm::TriggerResults> bitsH;
  iEvent.getByToken(triggerBitsToken_, bitsH);
  const edm::TriggerNames &names = iEvent.triggerNames(*bitsH);

  int triggers = 0;
  for(size_t bit=0; bit<bitsH->size(); ++bit) {
    if ( bitsH->accept(bit) ) {
      const std::string name = names.triggerName(bit);

      if ( false
         || (name.find("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v") != std::string::npos)
         || (name.find("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v") != std::string::npos)
         )
      {
        triggers |= 1<<EventTuple::TriggerDoubleMuon;
      }

      if ( false
         || (name.find("HLT_IsoMu24_v") != std::string::npos)
         || (name.find("HLT_IsoTkMu24_v") != std::string::npos)
         || (name.find("HLT_Mu45_eta2p1_v") != std::string::npos)
         || (name.find("HLT_Mu50_v") != std::string::npos)
         )
      {
        triggers |= 1<<EventTuple::TriggerSingleMuon;
      }

      if ( false
         || (name.find("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v") != std::string::npos)
         )
      {
        triggers |= 1<<EventTuple::TriggerDoubleElectron;
      }

      if ( false
         || (name.find("HLT_Ele25_eta2p1_WPTight_Gsf_v") != std::string::npos)
         || (name.find("HLT_Ele27_WPTight_Gsf_v") != std::string::npos)
         || (name.find("HLT_Ele115_CaloIdVT_GsfTrkIdT_v") != std::string::npos)
         )
      {
        triggers |= 1<<EventTuple::TriggerSingleElectron;
      }

      if ( false
         || (name.find("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v") != std::string::npos && name.find("_v9") == std::string::npos)
         || (name.find("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v") != std::string::npos)
         || (name.find("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v") != std::string::npos)
         || (name.find("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v") != std::string::npos)
         )
      {
        triggers |= 1<<EventTuple::TriggerMuonElectron;
      }

      if ( false
         || (name.find("HLT_Photon30_R9Id90_HE10_IsoM_v") != std::string::npos)
         || (name.find("HLT_Photon50_R9Id90_HE10_IsoM_v") != std::string::npos)
         || (name.find("HLT_Photon75_R9Id90_HE10_IsoM_v") != std::string::npos)
         || (name.find("HLT_Photon90_R9Id90_HE10_IsoM_v") != std::string::npos)
         || (name.find("HLT_Photon120_R9Id90_HE10_IsoM_v") != std::string::npos)
         || (name.find("HLT_Photon165_HE10_v") != std::string::npos)
         )
      {
        triggers |= 1<<EventTuple::TriggerSinglePhoton;
      }

      if ( false
         || (name.find("HLT_Mu17_v") != std::string::npos)
         )
      {
        triggers |= 1<<EventTuple::TriggerReferenceMuon;
      }

      if ( false
         || (name.find("HLT_Ele23_CaloIdL_TrackIdL_IsoVL_v") != std::string::npos)
         )
      {
        triggers |= 1<<EventTuple::TriggerReferenceElectron;
      }
    }
  }

  event_.triggers = triggers;
}

//define this as a plug-in
DEFINE_FWK_MODULE(EventTupler);
