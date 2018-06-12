#ifndef EventTupleUtil_h
#define EventTupleUtil_h

#include <cmath>
#include <map>

#include "TString.h"

// Things like DeltaR()
// https://root.cern.ch/doc/master/namespaceROOT_1_1Math_1_1VectorUtil.html
#include "Math/VectorUtil.h"

// EventTuple class definition
#include "EventTuple.h"

namespace EventTupleUtil {
  using namespace ROOT::Math::VectorUtil;

  constexpr int bit(int pos) { return (1<<pos); };

  inline bool passes(int idPacked, int pos) {
    return (idPacked & bit(pos));
  };

  // Types A and B must implement X() and Y()
  template<typename A, typename B>
  float masslessMT(const A& a, const B& b) {
    float magA = std::sqrt(a.X()*a.X()+a.Y()*a.Y());
    float magB = std::sqrt(b.X()*b.X()+b.Y()*b.Y());
    return std::sqrt(2*(magA*magB-a.X()*b.X()-a.Y()*b.Y()));
  };

  struct Lepton {
    LorentzVector p4;
    int pdgId;
    Lepton(const LorentzVector& _p4, const int id) : p4(_p4), pdgId(id) {};
  };

  bool overlaps(const std::vector<Lepton> &collection, const LorentzVector &test, float dR);

  // Modifies p4 as appropriate
  void applyObjectShift(LorentzVector& p4, const float p4Error, const bool shiftUp);

  // Categories for signal: EE, MM
  // Categories for background estimation: everybody else
  enum EventCategory {
    // Exclusive categories
    CatUnknown = bit(0),
    CatEE      = bit(1),
    CatMM      = bit(3),
    CatEM      = bit(2),
    CatEEL     = bit(4),
    CatMML     = bit(5),
    CatEELL    = bit(6),
    CatMMLL    = bit(7),
    // Inclusive categories
    CatLL      = CatEE | CatMM,
    CatLLL     = CatEEL | CatMML,
    CatLLLL    = CatEELL | CatMMLL,
    CatAll     = bit(8)-1,
  };

  // Categories to be saved in CategorizedHists
  const std::map<EventCategory, const char *> CategoryNames {
    // {CatUnknown, "noCat"},
    {CatEE, "ee"},
    {CatEM, "em"},
    {CatMM, "mm"},
    // {CatEEL, "eel"},
    // {CatMML, "mml"},
    // {CatEELL, "eell"},
    // {CatMMLL, "mmll"},
    {CatLL, "ll"},
    {CatLLL, "lll"},
    {CatLLLL, "llll"},
    // {CatAll, "all"},
  };

  enum Systematic {
    SystNominal,
    SystJetResUp,
    SystJetResDown,
    SystJetEnUp,
    SystJetEnDown,
    SystMuonEnUp,
    SystMuonEnDown,
    SystElectronEnUp,
    SystElectronEnDown,
    SystUnclusteredEnUp,
    SystUnclusteredEnDown,
    SystPileupUp,
    SystPileupDown,
    SystKFactorsUp,
    SystKFactorsDown,
    SystElectronSFUp,
    SystElectronSFDown,
    SystMuonSFUp,
    SystMuonSFDown,
    SystBtagUp,
    SystBtagDown,
    NSystematics,
    // Place systematics to ignore after this
    SystPhotonEnUp,
    SystPhotonEnDown,
    SystTauEnUp,
    SystTauEnDown,
  };

  const std::map<Systematic, const char *> SystematicNames {
    {SystNominal,           "Nominal"},
    {SystJetResUp,          "JetResUp"},
    {SystJetResDown,        "JetResDown"},
    {SystJetEnUp,           "JetEnUp"},
    {SystJetEnDown,         "JetEnDown"},
    {SystMuonEnUp,          "MuonEnUp"},
    {SystMuonEnDown,        "MuonEnDown"},
    {SystElectronEnUp,      "ElectronEnUp"},
    {SystElectronEnDown,    "ElectronEnDown"},
    {SystTauEnUp,           "TauEnUp"},
    {SystTauEnDown,         "TauEnDown"},
    {SystUnclusteredEnUp,   "UnclusteredEnUp"},
    {SystUnclusteredEnDown, "UnclusteredEnDown"},
    {SystPhotonEnUp,        "PhotonEnUp"},
    {SystPhotonEnDown,      "PhotonEnDown"},
    {SystPileupUp,          "PileupUp"},
    {SystPileupDown,        "PileupDown"},
    {SystKFactorsUp,        "KFactorsUp"},
    {SystKFactorsDown,      "KFactorsDown"},
    {SystElectronSFUp,      "ElectronSFUp"},
    {SystElectronSFDown,    "ElectronSFDown"},
    {SystMuonSFUp,          "MuonSFUp"},
    {SystMuonSFDown,        "MuonSFDown"},
    {SystBtagUp,            "BtagUp"},
    {SystBtagDown,          "BtagDown"},
  };
}

#endif
