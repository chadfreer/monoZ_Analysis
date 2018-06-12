#ifndef EventTuple_h
#define EventTuple_h

#include <vector>
#include <cstdint>

// Generic vector containers that actually only use the size they should
// (unlike TLorentzVector, where sizeof(TLorentzVector) = 60 !!)
// For more info, see: https://root.cern.ch/doc/master/Vector.html
#include "Math/LorentzVector.h"
typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float>> LorentzVector;
#include "Math/PositionVector3D.h"
typedef ROOT::Math::PositionVector3D<ROOT::Math::Cartesian3D<float>> Vector3D;
#include "Math/DisplacementVector2D.h"
typedef ROOT::Math::DisplacementVector2D<ROOT::Math::Cartesian2D<float>> Vector2D;
#include "Math/SMatrix.h"
typedef ROOT::Math::SMatrix<float, 2, 2, ROOT::Math::MatRepStd<float, 2>> Matrix2D;

class EventTuple {
  public:
    EventTuple() {};
    ~EventTuple() {};
    void clear();

    uint64_t run;
    uint64_t lumi;
    uint64_t event;

    std::vector<LorentzVector> genParticle_p4;
    std::vector<int> genParticle_id;
    std::vector<int> genParticle_flags;
    enum GenFlags {
      GenPromptFinalState=0,
      GenHardProcess=1,
      GenStatusOne=2,
      GenPromptTauDecay=3,
    };

    // Generator weights
    float genWeight;
    // EventTupler has config option saveLHEWeights
    // since this vector can be large for some MC
    // and might not be needed (e.g. DD background)
    std::vector<float> lheWeights;
    // Some LHE-level quantities
    float lheSumPartonHT;
    int lheNpartons;
    // For stitching Drell-Yan
    float lheZpt;

    // Mixing module info
    float nTruePileup;
    float nPoissonPileup;
    float nOutOfTimePileup;
    float nAheadOfTimePileup;

    // Triggers
    int triggers;
    enum TriggerBits {
      TriggerDoubleMuon=0,
      TriggerSingleMuon=1,
      TriggerDoubleElectron=2,
      TriggerSingleElectron=3,
      TriggerMuonElectron=4,
      TriggerSinglePhoton=5,
      TriggerReferenceMuon=6,
      TriggerReferenceElectron=7,
    };

    // Vertex
    Vector3D vertex;
    int nPrimaryVertices;
    int nPrimaryVerticesCut;

    // Some event global quantities
    float fixedGridRhoAll;
    float fixedGridRhoFastjetCentralCalo;

    // MET info
    int metFilters;
    enum MetFilterFlags {
      Flag_HBHENoiseFilter=0,
      Flag_HBHENoiseIsoFilter=1,
      Flag_EcalDeadCellTriggerPrimitiveFilter=2,
      Flag_goodVertices=3,
      Flag_eeBadScFilter=4,
      Flag_globalTightHalo2016Filter=5,
      Flag_BadChargedCandidateFilter=6,
      Flag_BadPFMuonFilter=7,
      Flag_CSCTightHalo2015Filter=8,
      Flag_BadMuonsFromHIP2016=9,
      Flag_DuplicateMuonsFromHIP2016=10,
      Flag_globalSuperTightHalo2016Filter=11,
    };
    float metSignificance;
    Matrix2D metSigMatrixInverse;
    Vector2D genMet;
    Vector2D caloMet;
    Vector2D trackMet;
    enum SubMetType {
      SubMETchargedBarrel,
      SubMETneutralBarrel,
      SubMETphotonBarrel,
      SubMETchargedEndcap,
      SubMETneutralEndcap,
      SubMETphotonEndcap,
      SubMETneutralEndcapNoTracker,
      SubMETphotonEndcapNoTracker,
      SubMEThadronicHF,
      SubMETemHF,
      SubMETmissingHt,
      NSubMetTypes
    };
    std::vector<Vector2D> subMets;
    std::vector<float> subMetSumEts;
    Vector2D rawPfMet;
    Vector2D type1PfMet;
    float type1PfSumEt;
    Vector2D type1PfMet_JetResUp;
    Vector2D type1PfMet_JetResDown;
    Vector2D type1PfMet_JetEnUp;
    Vector2D type1PfMet_JetEnDown;
    Vector2D type1PfMet_UnclusteredEnUp;
    Vector2D type1PfMet_UnclusteredEnDown;
    // Alternate MET src for check or sync
    Vector2D altPfMet;
    Vector2D altRawPfMet;
    // Uncleaned (only applicable to data)
    Vector2D uncleanedPfMet;
    int nPFconstituents;

    // Electrons
    std::vector<LorentzVector> electron_p4;
    std::vector<float> electron_p4Error;
    std::vector<LorentzVector> electron_p4_raw;
    std::vector<int> electron_q;
    std::vector<int> electron_id;
    enum ElectronID {
      ElectronVeto=0,
      ElectronLoose=1,
      ElectronMedium=2,
      ElectronTight=3,
      ElectronHEEPV6=4,
    };
    std::vector<float> electron_dxy;
    std::vector<float> electron_dz;

    // Muons
    std::vector<LorentzVector> muon_p4;
    std::vector<float> muon_p4Error;
    std::vector<int> muon_q;
    std::vector<int> muon_id;
    enum MuonID {
      MuonLoose=0,
      MuonMedium=1,
      MuonTight=2,
      MuonSoft=3,
      MuonHighPt=4,
      MuonICHEPmedium=5,
    };
    std::vector<float> muon_relIsoDBetaR04;
    std::vector<float> muon_trackIso;
    std::vector<float> muon_dxy;
    std::vector<float> muon_dz;

    // Photons
    std::vector<LorentzVector> photon_p4;
    std::vector<float> photon_p4Error;
    std::vector<int> photon_id;
    enum PhotonID {
      PhotonLoose=0,
      PhotonMedium=1,
      PhotonTight=2,
    };
    std::vector<float> photon_matchedTriggerEt;
    std::vector<float> photon_matchedTriggerDeltaR;
    std::vector<int> photon_minPrescale;

    // Taus
    std::vector<LorentzVector> tau_p4;
    std::vector<float> tau_p4Error;
    std::vector<int> tau_q;
    std::vector<int> tau_id;
    enum TauID {
      TauDecayModeFinding=0,
      TauLooseCombinedIso3Hits=1,
      TauMediumCombinedIso3Hits=2,
      TauTightCombinedIso3Hits=3,
    };

    // Jets
    std::vector<LorentzVector> jet_p4;
    std::vector<LorentzVector> jet_p4_JetResUp;
    std::vector<LorentzVector> jet_p4_JetResDown;
    std::vector<LorentzVector> jet_p4_JetEnUp;
    std::vector<LorentzVector> jet_p4_JetEnDown;
    std::vector<int> jet_id;
    enum JetID {
      JetLoose=0,
      JetTight=1,
    };
    std::vector<float> jet_btagCSVv2; // pfCombinedInclusiveSecondaryVertexV2BJetTags
    std::vector<int> jet_hadronFlavor; // MC-only
};

#endif
