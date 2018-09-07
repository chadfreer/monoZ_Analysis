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

#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"

class PhotonPrep : public edm::stream::EDProducer<> {
  public:
    explicit PhotonPrep(const edm::ParameterSet&);
    ~PhotonPrep();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    virtual void beginStream(edm::StreamID) override;
    virtual void produce(edm::Event&, const edm::EventSetup&) override;
    virtual void endStream() override;

    //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
    //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

    edm::EDGetTokenT<pat::PhotonCollection> photonToken_;
    edm::EDGetTokenT<edm::ValueMap<bool>> looseIdToken_;
    edm::EDGetTokenT<edm::ValueMap<bool>> mediumIdToken_;
    edm::EDGetTokenT<edm::ValueMap<bool>> tightIdToken_;
    edm::EDGetTokenT<pat::TriggerObjectStandAloneCollection> triggerObjectsToken_;
    edm::EDGetTokenT<pat::PackedTriggerPrescales> triggerPrescalesToken_;
    std::vector<std::string> triggerPathsForMinPrescale_;
    StringCutObjectSelector<reco::Candidate::LorentzVector, false> kinematicCutAnyVariation_;
    StringCutObjectSelector<pat::Photon> finalCut_;
};

PhotonPrep::PhotonPrep(const edm::ParameterSet& iConfig):
  photonToken_(consumes<pat::PhotonCollection>(iConfig.getParameter<edm::InputTag>("photonSrc"))),
  looseIdToken_(consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("looseId"))),
  mediumIdToken_(consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("mediumId"))),
  tightIdToken_(consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("tightId"))),
  triggerObjectsToken_(consumes<pat::TriggerObjectStandAloneCollection>(iConfig.getParameter<edm::InputTag>("triggerObjects"))),
  triggerPrescalesToken_(consumes<pat::PackedTriggerPrescales>(iConfig.getParameter<edm::InputTag>("triggerPrescales"))),
  triggerPathsForMinPrescale_(iConfig.getParameter<std::vector<std::string>>("triggerPathsForMinPrescale")),
  kinematicCutAnyVariation_(iConfig.getParameter<std::string>("kinematicCutAnyVariation")),
  finalCut_(iConfig.getParameter<std::string>("finalCut"))
{
  produces<pat::PhotonCollection>();
}


PhotonPrep::~PhotonPrep()
{
}

void
PhotonPrep::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  Handle<pat::PhotonCollection> photonHandle;
  iEvent.getByToken(photonToken_, photonHandle);
  const auto inputId = photonHandle.id();

  Handle<ValueMap<bool>> looseIdHandle;
  iEvent.getByToken(looseIdToken_, looseIdHandle);
  Handle<ValueMap<bool>> mediumIdHandle;
  iEvent.getByToken(mediumIdToken_, mediumIdHandle);
  Handle<ValueMap<bool>> tightIdHandle;
  iEvent.getByToken(tightIdToken_, tightIdHandle);

  Handle<pat::TriggerObjectStandAloneCollection> triggerObjectsHandle;
  iEvent.getByToken(triggerObjectsToken_, triggerObjectsHandle);
  Handle<pat::PackedTriggerPrescales> triggerPrescalesHandle;
  iEvent.getByToken(triggerPrescalesToken_, triggerPrescalesHandle);

  std::unique_ptr<pat::PhotonCollection> photons(new pat::PhotonCollection());

  for(size_t iPho=0; iPho<photonHandle->size(); ++iPho) {
    pat::Photon phoNew(photonHandle->at(iPho));

    phoNew.addUserFloat("p4Error", phoNew.getCorrectedEnergyError(reco::Photon::P4type::regression2));
    // TODO: phoNew.addUserCand("raw", ...

    phoNew.addUserInt("LooseId", looseIdHandle->get(inputId, iPho));
    phoNew.addUserInt("MediumId", mediumIdHandle->get(inputId, iPho));
    phoNew.addUserInt("TightId", tightIdHandle->get(inputId, iPho));

    pat::TriggerObjectStandAlone matchedTriggerObject;
    double minDeltaR{999.};
    for(const auto& obj : *triggerObjectsHandle) {
      if ( obj.hasTriggerObjectType(trigger::TriggerPhoton) ) {
        double dR = reco::deltaR(phoNew.p4(), obj.p4());
        if ( dR < minDeltaR ) {
          minDeltaR = dR;
          matchedTriggerObject = obj;
        }
      }
    }

    int minPrescale{0};
    if ( minDeltaR < 999. && finalCut_(phoNew) ) {
      const edm::TriggerNames& triggerNames = iEvent.triggerNames(triggerPrescalesHandle->triggerResults());
      matchedTriggerObject.unpackPathNames(triggerNames);
      // Copy necessary due to const-ness disallowing unpacking of trigger names :<
      pat::PackedTriggerPrescales triggerPrescales = *triggerPrescalesHandle;
      triggerPrescales.setTriggerNames(triggerNames);
      bool isVolunteer{true};
      auto names = matchedTriggerObject.pathNames(true);
      // if ( names.size() > 0 ) std::cout << "Matched photon with trigger object, trigger pt = " << matchedTriggerObject.pt() << std::endl;
      // for (auto name : names) std::cout << "    Path: " << name << std::endl;
      for(const auto& path : triggerPathsForMinPrescale_) {
        // To check that at least one path actually fired
        if ( matchedTriggerObject.hasPathName(path+"*", true) ) {
          // std::cout << "  Path fired: " << path << std::endl;
          isVolunteer &= false;
        }
        // If trigger would have fired, but was not due to prescale,
        // the event weight should correspond to this trigger's prescale
        if ( matchedTriggerObject.hasPathName(path+"*", false) ) {
          int prescale = triggerPrescales.getPrescaleForName(path, true);
          // std::cout << "  Path L3 accept: " << path << ", prescale = " << prescale << std::endl;
          if ( minPrescale == 0 || prescale < minPrescale ) {
            minPrescale = prescale;
          }
        }
      }
      if ( isVolunteer ) {
        // if ( names.size() > 0 ) std::cout << " -- photon is volunteer" << std::endl;
        minPrescale = 0;
      }
    }
    phoNew.addUserInt("minPrescale", minPrescale);
    phoNew.addUserFloat("matchedTriggerDeltaR", minDeltaR);
    phoNew.addUserFloat("matchedTriggerEt", matchedTriggerObject.et());

    bool passCut{false};
    double relErr = 1 + phoNew.userFloat("p4Error") / phoNew.p();
    if ( kinematicCutAnyVariation_(phoNew.p4()         ) ) passCut |= true;
    if ( kinematicCutAnyVariation_(phoNew.p4() * relErr) ) passCut |= true;
    if ( kinematicCutAnyVariation_(phoNew.p4() / relErr) ) passCut |= true;
    for(auto candName : phoNew.userCandNames()) {
      if ( kinematicCutAnyVariation_(phoNew.userCand(candName)->p4()) ) passCut |= true;
    }
    if ( !finalCut_(phoNew) ) passCut = false;

    if ( passCut ) photons->push_back(phoNew);
  }

  iEvent.put(std::move(photons));
}


void
PhotonPrep::beginStream(edm::StreamID)
{
}


void
PhotonPrep::endStream() {
}


/*
void
PhotonPrep::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 

/*
void
PhotonPrep::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 

/*
void
PhotonPrep::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 

/*
void
PhotonPrep::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 

void
PhotonPrep::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}


DEFINE_FWK_MODULE(PhotonPrep);
