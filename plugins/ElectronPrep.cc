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

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

class ElectronPrep : public edm::stream::EDProducer<> {
  public:
    explicit ElectronPrep(const edm::ParameterSet&);
    ~ElectronPrep();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    virtual void beginStream(edm::StreamID) override;
    virtual void produce(edm::Event&, const edm::EventSetup&) override;
    virtual void endStream() override;

    //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
    //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

    edm::EDGetTokenT<pat::ElectronCollection> electronToken_;
    edm::EDGetTokenT<reco::CandidateView> uncalibratedToken_;
    edm::EDGetTokenT<edm::ValueMap<bool>> vetoIdToken_;
    edm::EDGetTokenT<edm::ValueMap<bool>> looseIdToken_;
    edm::EDGetTokenT<edm::ValueMap<bool>> mediumIdToken_;
    edm::EDGetTokenT<edm::ValueMap<bool>> tightIdToken_;
    edm::EDGetTokenT<edm::ValueMap<bool>> heepIdToken_;
    edm::EDGetTokenT<reco::VertexCollection> vertexToken_;
    StringCutObjectSelector<reco::Candidate::LorentzVector, false> kinematicCutAnyVariation_;
    StringCutObjectSelector<pat::Electron> finalCut_;
};

ElectronPrep::ElectronPrep(const edm::ParameterSet& iConfig):
  electronToken_(consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electronSrc"))),
  uncalibratedToken_(consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>("uncalibratedSrc"))),
  vetoIdToken_(consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("vetoId"))),
  looseIdToken_(consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("looseId"))),
  mediumIdToken_(consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("mediumId"))),
  tightIdToken_(consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("tightId"))),
  heepIdToken_(consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("heepId"))),
  vertexToken_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexSrc"))),
  kinematicCutAnyVariation_(iConfig.getParameter<std::string>("kinematicCutAnyVariation")),
  finalCut_(iConfig.getParameter<std::string>("finalCut"))
{
  produces<pat::ElectronCollection>();
}


ElectronPrep::~ElectronPrep()
{
}

void
ElectronPrep::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  Handle<pat::ElectronCollection> electronHandle;
  iEvent.getByToken(electronToken_, electronHandle);
  const auto inputId = electronHandle.id();

  Handle<reco::CandidateView> uncalibratedHandle;
  iEvent.getByToken(uncalibratedToken_, uncalibratedHandle);
  if ( uncalibratedHandle->size() != electronHandle->size() ) {
    edm::LogError("ElectronPrep") << "Uncalibrated collections not same size as input collection!";
  }

  Handle<ValueMap<bool>> vetoIdHandle;
  iEvent.getByToken(vetoIdToken_, vetoIdHandle);
  Handle<ValueMap<bool>> looseIdHandle;
  iEvent.getByToken(looseIdToken_, looseIdHandle);
  Handle<ValueMap<bool>> mediumIdHandle;
  iEvent.getByToken(mediumIdToken_, mediumIdHandle);
  Handle<ValueMap<bool>> tightIdHandle;
  iEvent.getByToken(tightIdToken_, tightIdHandle);
  Handle<ValueMap<bool>> heepIdHandle;
  iEvent.getByToken(heepIdToken_, heepIdHandle);

  Handle<reco::VertexCollection> vertexHandle;
  iEvent.getByToken(vertexToken_, vertexHandle);
  const auto& pv = vertexHandle->at(0);

  std::unique_ptr<pat::ElectronCollection> electrons(new pat::ElectronCollection());

  for(size_t iEl=0; iEl<electronHandle->size(); ++iEl) {
    pat::Electron elNew(electronHandle->at(iEl));
    auto p4kind = elNew.candidateP4Kind();

    elNew.addUserFloat("p4Error", elNew.p4Error(p4kind));

    elNew.addUserCand("raw", uncalibratedHandle->ptrAt(iEl));
    elNew.addUserData("raw", uncalibratedHandle->at(iEl).p4());

    elNew.addUserInt("VetoId", vetoIdHandle->get(inputId, iEl));
    elNew.addUserInt("LooseId", looseIdHandle->get(inputId, iEl));
    elNew.addUserInt("MediumId", mediumIdHandle->get(inputId, iEl));
    elNew.addUserInt("TightId", tightIdHandle->get(inputId, iEl));
    elNew.addUserInt("HEEPV6Id", heepIdHandle->get(inputId, iEl));

    elNew.addUserFloat("dxy", elNew.gsfTrack()->dxy(pv.position()));
    elNew.addUserFloat("dz", elNew.gsfTrack()->dz(pv.position()));

    bool passCut{false};
    double relErr = 1 + elNew.userFloat("p4Error") / elNew.p();
    if ( kinematicCutAnyVariation_(elNew.p4(p4kind)         ) ) passCut |= true;
    if ( kinematicCutAnyVariation_(elNew.p4(p4kind) * relErr) ) passCut |= true;
    if ( kinematicCutAnyVariation_(elNew.p4(p4kind) / relErr) ) passCut |= true;
    for(auto candName : elNew.userCandNames()) {
      if ( kinematicCutAnyVariation_(elNew.userCand(candName)->p4()) ) passCut |= true;
    }
    if ( !finalCut_(elNew) ) passCut = false;

    if ( passCut ) electrons->push_back(elNew);
  }

  iEvent.put(std::move(electrons));
}


void
ElectronPrep::beginStream(edm::StreamID)
{
}


void
ElectronPrep::endStream() {
}


/*
void
ElectronPrep::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 

/*
void
ElectronPrep::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 

/*
void
ElectronPrep::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 

/*
void
ElectronPrep::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 

void
ElectronPrep::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}


DEFINE_FWK_MODULE(ElectronPrep);
