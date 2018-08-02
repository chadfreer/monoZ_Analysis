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

#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

class TauPrep : public edm::stream::EDProducer<> {
  public:
    explicit TauPrep(const edm::ParameterSet&);
    ~TauPrep();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    virtual void beginStream(edm::StreamID) override;
    virtual void produce(edm::Event&, const edm::EventSetup&) override;
    virtual void endStream() override;

    //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
    //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

    edm::EDGetTokenT<pat::TauCollection> tauToken_;
    edm::EDGetTokenT<reco::VertexCollection> vertexToken_;
    StringCutObjectSelector<reco::Candidate::LorentzVector, false> kinematicCutAnyVariation_;
    StringCutObjectSelector<pat::Tau> finalCut_;
};

TauPrep::TauPrep(const edm::ParameterSet& iConfig):
  tauToken_(consumes<pat::TauCollection>(iConfig.getParameter<edm::InputTag>("tauSrc"))),
  vertexToken_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexSrc"))),
  kinematicCutAnyVariation_(iConfig.getParameter<std::string>("kinematicCutAnyVariation")),
  finalCut_(iConfig.getParameter<std::string>("finalCut"))
{
  produces<pat::TauCollection>();
}


TauPrep::~TauPrep()
{
}

void
TauPrep::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  Handle<pat::TauCollection> tauHandle;
  iEvent.getByToken(tauToken_, tauHandle);

  Handle<reco::VertexCollection> vertexHandle;
  iEvent.getByToken(vertexToken_, vertexHandle);
  const auto& pv = vertexHandle->at(0);
  // TODO: vertex compatibility?
  (void) pv;

  std::unique_ptr<pat::TauCollection> taus(new pat::TauCollection());

  for(size_t iTau=0; iTau<tauHandle->size(); ++iTau) {
    pat::Tau tauNew(tauHandle->at(iTau));

    // TODO: energy scale
    tauNew.addUserFloat("p4Error", tauNew.p() * 0.03);

    bool passCut{false};
    if ( kinematicCutAnyVariation_(tauNew.p4()) ) passCut |= true;
    for(auto candName : tauNew.userCandNames()) {
      if ( kinematicCutAnyVariation_(tauNew.userCand(candName)->p4()) ) passCut |= true;
    }
    if ( !finalCut_(tauNew) ) passCut = false;

    if ( passCut ) taus->push_back(tauNew);
  }

  iEvent.put(std::move(taus));
}


void
TauPrep::beginStream(edm::StreamID)
{
}


void
TauPrep::endStream() {
}


/*
void
TauPrep::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 

/*
void
TauPrep::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 

/*
void
TauPrep::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 

/*
void
TauPrep::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 

void
TauPrep::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}


DEFINE_FWK_MODULE(TauPrep);
