#include "Analysis/monoZ/interface/EventTupleUtil.h"

void
EventTuple::clear() {
  genParticle_p4.clear();
  genParticle_id.clear();
  genParticle_flags.clear();
  lheWeights.clear();
  subMets.clear();
  subMetSumEts.clear();
  electron_p4.clear();
  electron_p4Error.clear();
  electron_p4_raw.clear();
  electron_q.clear();
  electron_id.clear();
  electron_dxy.clear();
  electron_dz.clear();
  muon_p4.clear();
  muon_p4Error.clear();
  muon_q.clear();
  muon_id.clear();
  muon_relIsoDBetaR04.clear();
  muon_trackIso.clear();
  muon_dxy.clear();
  muon_dz.clear();
  photon_p4.clear();
  photon_p4Error.clear();
  photon_id.clear();
  photon_matchedTriggerEt.clear();
  photon_matchedTriggerDeltaR.clear();
  photon_minPrescale.clear();
  tau_p4.clear();
  tau_p4Error.clear();
  tau_q.clear();
  tau_id.clear();
  jet_p4.clear();
  jet_p4_JetResUp.clear();
  jet_p4_JetResDown.clear();
  jet_p4_JetEnUp.clear();
  jet_p4_JetEnDown.clear();
  jet_id.clear();
  jet_btagCSVv2.clear();
  jet_hadronFlavor.clear();
  // Mark as c (mc) then
  // :g/^ *std::vector/norm $byw`cO^R".clear();
}

