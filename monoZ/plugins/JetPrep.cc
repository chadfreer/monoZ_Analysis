// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

#include "PhysicsTools/SelectorUtils/interface/PFJetIDSelectionFunctor.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

class JetPrep : public edm::stream::EDProducer<> {
  public:
    explicit JetPrep(const edm::ParameterSet&);
    ~JetPrep();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    virtual void beginStream(edm::StreamID) override;
    virtual void produce(edm::Event&, const edm::EventSetup&) override;
    virtual void endStream() override;

    //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
    //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

    edm::EDGetTokenT<pat::JetCollection> jetToken_;

    edm::EDGetTokenT<reco::CandidateView> shiftedJESUpToken_;
    edm::EDGetTokenT<reco::CandidateView> shiftedJESDownToken_;
    edm::EDGetTokenT<reco::CandidateView> shiftedJERUpToken_;
    edm::EDGetTokenT<reco::CandidateView> shiftedJERDownToken_;

    StringCutObjectSelector<reco::Candidate::LorentzVector, false> kinematicCutAnyVariation_;
    StringCutObjectSelector<pat::Jet> finalCut_;

    PFJetIDSelectionFunctor looseJetIdSelector_{PFJetIDSelectionFunctor::RUNIISTARTUP, PFJetIDSelectionFunctor::LOOSE};
    PFJetIDSelectionFunctor tightJetIdSelector_{PFJetIDSelectionFunctor::RUNIISTARTUP, PFJetIDSelectionFunctor::TIGHT};
    pat::strbitset hasLooseId_;
    pat::strbitset hasTightId_;
};

JetPrep::JetPrep(const edm::ParameterSet& iConfig):
  jetToken_(consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("jetSrc"))),
  shiftedJESUpToken_(consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>("shiftedJESUp"))),
  shiftedJESDownToken_(consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>("shiftedJESDown"))),
  shiftedJERUpToken_(consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>("shiftedJERUp"))),
  shiftedJERDownToken_(consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>("shiftedJERDown"))),
  kinematicCutAnyVariation_(iConfig.getParameter<std::string>("kinematicCutAnyVariation")),
  finalCut_(iConfig.getParameter<std::string>("finalCut"))
{
  produces<pat::JetCollection>();

  hasLooseId_ = looseJetIdSelector_.getBitTemplate();
  hasTightId_ = tightJetIdSelector_.getBitTemplate();
}


JetPrep::~JetPrep()
{
}

void
JetPrep::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  Handle<pat::JetCollection> jetHandle;
  iEvent.getByToken(jetToken_, jetHandle);

  Handle<reco::CandidateView> shiftedJESUpHandle;
  iEvent.getByToken(shiftedJESUpToken_, shiftedJESUpHandle);
  Handle<reco::CandidateView> shiftedJESDownHandle;
  iEvent.getByToken(shiftedJESDownToken_, shiftedJESDownHandle);
  if (shiftedJESUpHandle->size() != jetHandle->size() || shiftedJESDownHandle->size() != jetHandle->size()) {
    edm::LogError("JetPrep") << "shiftedJES collections not same size as input collection!";
  }

  Handle<reco::CandidateView> shiftedJERUpHandle;
  Handle<reco::CandidateView> shiftedJERDownHandle;
  if ( !iEvent.isRealData() ) {
    iEvent.getByToken(shiftedJERUpToken_, shiftedJERUpHandle);
    iEvent.getByToken(shiftedJERDownToken_, shiftedJERDownHandle);
    if (shiftedJERUpHandle->size() != jetHandle->size() || shiftedJERDownHandle->size() != jetHandle->size()) {
      edm::LogError("JetPrep") << "shiftedJER collections not same size as input collection!";
    }
  }

  std::unique_ptr<pat::JetCollection> jets(new pat::JetCollection());

  for(size_t iJet=0; iJet<jetHandle->size(); ++iJet) {
    pat::Jet jetNew(jetHandle->at(iJet));

    // Using MET Uncertainty nomenclature, JES = JetEn
    jetNew.addUserCand("JetEnUp", shiftedJESUpHandle->ptrAt(iJet));
    jetNew.addUserData("JetEnUp", shiftedJESUpHandle->at(iJet).p4());
    jetNew.addUserCand("JetEnDown", shiftedJESDownHandle->ptrAt(iJet));
    jetNew.addUserData("JetEnDown", shiftedJESDownHandle->at(iJet).p4());

    // Using MET Uncertainty nomenclature, JER = JetRes
    if ( !iEvent.isRealData() ) {
      jetNew.addUserCand("JetResUp", shiftedJERUpHandle->ptrAt(iJet));
      jetNew.addUserData("JetResUp", shiftedJERUpHandle->at(iJet).p4());
      jetNew.addUserCand("JetResDown", shiftedJERDownHandle->ptrAt(iJet));
      jetNew.addUserData("JetResDown", shiftedJERDownHandle->at(iJet).p4());
    }

    jetNew.addUserInt("looseJetId", looseJetIdSelector_(jetNew, hasLooseId_));
    jetNew.addUserInt("tightJetId", tightJetIdSelector_(jetNew, hasTightId_));

    bool passCut{false};
    if ( kinematicCutAnyVariation_(jetNew.p4()) ) passCut |= true;
    for(auto candName : jetNew.userCandNames()) {
      if ( kinematicCutAnyVariation_(jetNew.userCand(candName)->p4()) ) passCut |= true;
    }
    if ( !finalCut_(jetNew) ) passCut = false;

    if ( passCut ) jets->push_back(jetNew);
  }

  iEvent.put(std::move(jets));
}


void
JetPrep::beginStream(edm::StreamID)
{
}


void
JetPrep::endStream() {
}


/*
void
JetPrep::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 

/*
void
JetPrep::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 

/*
void
JetPrep::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 

/*
void
JetPrep::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 

void
JetPrep::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}


DEFINE_FWK_MODULE(JetPrep);
