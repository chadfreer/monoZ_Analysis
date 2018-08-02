#ifndef ANALYSIS_Muon_h
#define ANALYSIS_Muon_h

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "Analysis/DataFormats/interface/WeightedObject.h"

namespace Analysis {

    class Muon : public pat::Muon, public WeightedObject
    {

    public:
        Muon();
        Muon( const pat::Muon & );
        ~Muon();

        Muon *clone() const { return ( new Muon( *this ) ); }

    private:
    };
}

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

