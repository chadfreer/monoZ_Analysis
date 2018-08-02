// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/METReco/interface/MET.h"
#include "DataFormats/METReco/interface/METFwd.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

class SubMETProducer : public edm::stream::EDProducer<> {
  public:
    explicit SubMETProducer(const edm::ParameterSet&);
    ~SubMETProducer();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    virtual void beginStream(edm::StreamID) override;
    virtual void produce(edm::Event&, const edm::EventSetup&) override;
    virtual void endStream() override;

    //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
    //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

    edm::EDGetTokenT<pat::PackedCandidateCollection> packedToken_;
    StringCutObjectSelector<pat::PackedCandidate, false> selection_;
    edm::EDGetTokenT<reco::VertexCollection> vertexToken_;
    double dzToPrimaryVertex_;
};

SubMETProducer::SubMETProducer(const edm::ParameterSet& iConfig):
  packedToken_(consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("packedCandidatesSrc"))),
  selection_(iConfig.getParameter<std::string>("selection")),
  vertexToken_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexSrc"))),
  dzToPrimaryVertex_(iConfig.getParameter<double>("dzToPrimaryVertex"))
{
  produces<reco::METCollection>();
}


SubMETProducer::~SubMETProducer()
{
}

void
SubMETProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  edm::Handle<pat::PackedCandidateCollection> packedHandle;
  iEvent.getByToken(packedToken_, packedHandle);

  edm::Handle<reco::VertexCollection> vertexHandle;
  iEvent.getByToken(vertexToken_, vertexHandle);
  const auto& pv = vertexHandle->at(0);

  std::unique_ptr<reco::METCollection> mets(new reco::METCollection());

  double sumET{0.};
  reco::Candidate::LorentzVector sumVect;
  size_t nConstituents{0};
  for(const auto& pfCand : *packedHandle) {
    if ( !selection_(pfCand) ) continue;
    if ( fabs(pfCand.dz(pv.position())) > dzToPrimaryVertex_ ) continue;

    sumVect += pfCand.p4();
    sumET += pfCand.et();
    nConstituents++;
  }
  mets->emplace_back(sumET, -sumVect, pv.position());
  // Hacky way to store PF constituents in MET object
  mets->back().setPdgId(nConstituents);

  iEvent.put(std::move(mets));
}


void
SubMETProducer::beginStream(edm::StreamID)
{
}


void
SubMETProducer::endStream() {
}


/*
void
SubMETProducer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 

/*
void
SubMETProducer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 

/*
void
SubMETProducer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 

/*
void
SubMETProducer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 

void
SubMETProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}


DEFINE_FWK_MODULE(SubMETProducer);
