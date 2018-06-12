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

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

class MuonPrep : public edm::stream::EDProducer<> {
  public:
    explicit MuonPrep(const edm::ParameterSet&);
    ~MuonPrep();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    virtual void beginStream(edm::StreamID) override;
    virtual void produce(edm::Event&, const edm::EventSetup&) override;
    virtual void endStream() override;

    bool isICHEPmedium(const reco::Muon& recoMu);

    //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
    //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

    edm::EDGetTokenT<pat::MuonCollection> muonToken_;
    edm::EDGetTokenT<reco::VertexCollection> vertexToken_;
    StringCutObjectSelector<reco::Candidate::LorentzVector, false> kinematicCutAnyVariation_;
    StringCutObjectSelector<pat::Muon> finalCut_;
};

MuonPrep::MuonPrep(const edm::ParameterSet& iConfig):
  muonToken_(consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muonSrc"))),
  vertexToken_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexSrc"))),
  kinematicCutAnyVariation_(iConfig.getParameter<std::string>("kinematicCutAnyVariation")),
  finalCut_(iConfig.getParameter<std::string>("finalCut"))
{
  produces<pat::MuonCollection>();
}


MuonPrep::~MuonPrep()
{
}

void
MuonPrep::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  Handle<pat::MuonCollection> muonHandle;
  iEvent.getByToken(muonToken_, muonHandle);

  Handle<reco::VertexCollection> vertexHandle;
  iEvent.getByToken(vertexToken_, vertexHandle);
  const auto& pv = vertexHandle->at(0);

  std::unique_ptr<pat::MuonCollection> muons(new pat::MuonCollection());

  for(size_t iMu=0; iMu<muonHandle->size(); ++iMu) {
    pat::Muon muNew(muonHandle->at(iMu));

    // TODO: energy scale
    muNew.addUserFloat("p4Error", 0.02 * muNew.p());

    muNew.addUserInt("isTightMuon", muNew.isTightMuon(pv));
    muNew.addUserInt("isSoftMuon", muNew.isSoftMuon(pv));
    muNew.addUserInt("isHighPtMuon", muNew.isHighPtMuon(pv));
    muNew.addUserInt("isICHEPmedium", isICHEPmedium(muNew));

    const auto& isoValues = muNew.pfIsolationR04();
    muNew.addUserFloat("relIsoDBetaR04", (isoValues.sumChargedHadronPt + std::max(0., isoValues.sumNeutralHadronEt + isoValues.sumPhotonEt - 0.5*isoValues.sumPUPt))/muNew.pt());

    muNew.addUserFloat("dxy", muNew.muonBestTrack()->dxy(pv.position()));
    muNew.addUserFloat("dz", muNew.muonBestTrack()->dz(pv.position()));

    bool passCut{false};
    double relErr = 1 + muNew.userFloat("p4Error") / muNew.p();
    if ( kinematicCutAnyVariation_(muNew.p4()         ) ) passCut |= true;
    if ( kinematicCutAnyVariation_(muNew.p4() * relErr) ) passCut |= true;
    if ( kinematicCutAnyVariation_(muNew.p4() / relErr) ) passCut |= true;
    for(auto candName : muNew.userCandNames()) {
      if ( kinematicCutAnyVariation_(muNew.userCand(candName)->p4()) ) passCut |= true;
    }
    if ( !finalCut_(muNew) ) passCut = false;

    if ( passCut ) muons->push_back(muNew);
  }

  iEvent.put(std::move(muons));
}


void
MuonPrep::beginStream(edm::StreamID)
{
}


void
MuonPrep::endStream() {
}


/*
void
MuonPrep::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 

/*
void
MuonPrep::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 

/*
void
MuonPrep::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 

/*
void
MuonPrep::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 

void
MuonPrep::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}


bool
MuonPrep::isICHEPmedium(const reco::Muon & recoMu)
{
  // Fixes inefficiency due to HIP/(strip dynamic inefficiency)
  // https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideMuonIdRun2?Short_Term_Medium_Muon_Definitio=#Short_Term_Medium_Muon_Definitio
  bool goodGlob = recoMu.isGlobalMuon() &&
                  recoMu.globalTrack()->normalizedChi2() < 3 &&
                  recoMu.combinedQuality().chi2LocalPosition < 12 &&
                  recoMu.combinedQuality().trkKink < 20;
  bool isMedium = muon::isLooseMuon(recoMu) &&
                  recoMu.innerTrack()->validFraction() > 0.49 &&
                  muon::segmentCompatibility(recoMu) > (goodGlob ? 0.303 : 0.451);
  return isMedium;
}

DEFINE_FWK_MODULE(MuonPrep);
