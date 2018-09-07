// -*- C++ -*-
//
// Package:   Analysis/EventCounters
// Class:    EventCounters
// 
/**\class EventCounters EventCounters.cc Analysis/monoZ/plugins/EventCounters.cc

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

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"

// ROOT include files
#include "TH1D.h"
#include "TDirectory.h"
#include "TObjString.h"

class EventCounters : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
  public:
    explicit EventCounters(const edm::ParameterSet&);
    ~EventCounters();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


  private:
    virtual void beginJob() override;
    virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
    virtual void endJob() override;

    // ----------member data ---------------------------
    edm::EDGetTokenT<GenEventInfoProduct> genEventInfoToken_;
    edm::EDGetTokenT<LHEEventProduct> lheEventToken_;
    unsigned int nLHEWeights_;

    TH1D * counters_;
    enum Counters {
      UnderflowBin=0,
      NEvents=1,
      NNegEvents=2,
      NPosEvents=3,
      GenWeight=4,
      LHEWeight=5,
      NTypes=6,
    };

    TH1D * sumLHEWeights_;
    TH1D * muRmuFCounts_;
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
EventCounters::EventCounters(const edm::ParameterSet& iConfig):
  genEventInfoToken_(consumes<GenEventInfoProduct>(iConfig.getParameter<edm::InputTag>("genEventInfoSrc"))),
  lheEventToken_(consumes<LHEEventProduct>(iConfig.getParameter<edm::InputTag>("lheEventSrc"))),
  nLHEWeights_(iConfig.getParameter<unsigned int>("nLHEWeights"))
{
  //now do what ever initialization is needed
  usesResource("TFileService");
  edm::Service<TFileService> fs;

  counters_ = fs->make<TH1D>("counters", "Event Counters;Type;Count", NTypes, 1, NTypes+1);
  counters_->GetXaxis()->SetBinLabel(NEvents, "NEvents");
  counters_->GetXaxis()->SetBinLabel(NNegEvents, "NNegEvents");
  counters_->GetXaxis()->SetBinLabel(NPosEvents, "NPosEvents");
  counters_->GetXaxis()->SetBinLabel(GenWeight, "GenWeight");
  counters_->GetXaxis()->SetBinLabel(LHEWeight, "LHEWeight");

  sumLHEWeights_ = fs->make<TH1D>("sumLHEWeights", "Sum of weights from LHE;Weight index;Sum", nLHEWeights_, 0, nLHEWeights_);
  muRmuFCounts_ = fs->make<TH1D>("muRmuFCounts", "Sum of max/min weights for muRmuF shift;;Sum", 2, 0, 2);
  muRmuFCounts_->GetXaxis()->SetBinLabel(1, "#Sigma max(#mu_{R/F} shifts)");
  muRmuFCounts_->GetXaxis()->SetBinLabel(2, "#Sigma min(#mu_{R/F} shifts)");

  // e.g. metaData = cms.PSet( asdf = cms.string("blah"), listOfCrap = cms.vstring(['blah', 'blah']) )
  const edm::ParameterSet metaData = iConfig.getParameterSet("metaData");
  for(const auto name : metaData.getParameterNamesForType<std::string>()) {
    auto dir = fs->mkdir(name);
    auto value = metaData.getParameter<std::string>(name);
    dir.make<TObjString>(value.c_str());
  }
  for(const auto name : metaData.getParameterNamesForType<std::vector<std::string>>()) {
    auto dir = fs->mkdir(name);
    auto values = metaData.getParameter<std::vector<std::string>>(name);
    for(const auto value : values) {
      dir.make<TObjString>(value.c_str());
    }
  }
}


EventCounters::~EventCounters()
{
}


//
// member functions
//

// ------------ method called for each event  ------------
void
EventCounters::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  counters_->Fill(NEvents);

  if ( !iEvent.isRealData() ) {
    edm::Handle<GenEventInfoProduct> genEventInfoHandle;
    iEvent.getByToken(genEventInfoToken_, genEventInfoHandle);
    const GenEventInfoProduct &genEvent = *genEventInfoHandle.product();

    counters_->Fill(GenWeight, genEvent.weight());
    if ( genEvent.weight() > 0 ) {
      counters_->Fill(NPosEvents);
    } else {
      counters_->Fill(NNegEvents);
    }

    edm::Handle<LHEEventProduct> lheEventHandle;
    iEvent.getByToken(lheEventToken_, lheEventHandle);
    if ( lheEventHandle.isValid() ) {
      const LHEEventProduct &lheEvent = *lheEventHandle.product();
      counters_->Fill(LHEWeight, lheEvent.originalXWGTUP());

      const auto& lheWeights = lheEvent.weights();
      for(size_t iw=0; iw<std::min(lheWeights.size(), (size_t) nLHEWeights_); ++iw) {
        sumLHEWeights_->Fill(iw, lheWeights[iw].wgt);
      }

      // Some processes did not have any weights?
      if ( lheWeights.size() == 0 ) return;

      // Calculate muR, muF shift max and min
      // (for factorizing QCD scale uncertainty on xs and acceptance)
      double maxW = lheWeights[0].wgt;
      double minW = lheWeights[0].wgt;
      for(size_t i=1; i<std::min(lheWeights.size(), 9ul); ++i) {
        if ( lheWeights[i].wgt > maxW ) maxW = lheWeights[i].wgt;
        if ( lheWeights[i].wgt < minW ) minW = lheWeights[i].wgt;
      }

      muRmuFCounts_->Fill(0., maxW / lheWeights[0].wgt);
      muRmuFCounts_->Fill(1., minW / lheWeights[0].wgt);
    }
  }
}


// ------------ method called once each job just before starting event loop  ------------
void 
EventCounters::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
EventCounters::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
EventCounters::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(EventCounters);
