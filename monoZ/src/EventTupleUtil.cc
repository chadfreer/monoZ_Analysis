#include "Analysis/monoZ/interface/EventTupleUtil.h"

namespace EventTupleUtil {

bool overlaps(const std::vector<Lepton> &collection, const LorentzVector &test, float dR) {
  for(const auto& lep : collection) {
    if ( DeltaR(lep.p4, test) < dR ) return true;
  }
  return false;
};

void applyObjectShift(LorentzVector& p4, const float p4Error, const bool shiftUp) {
  // Convention in tuples is to have the error be in same units of p4
  // So first turn it into a relative percentage and do the logNormal-style shift
  double relErr = p4Error / p4.P();
  if ( shiftUp ) {
    p4 *= 1. + relErr;
  }
  else {
    p4 /= 1. + relErr;
  }
}

}
