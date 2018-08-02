#ifndef ANALYSIS_Electron_h
#define ANALYSIS_Electron_h

#include "DataFormats/PatCandidates/interface/Electron.h"
//#include "flashgg/DataFormats/interface/DiPhotonCandidate.h"
#include "Analysis/DataFormats/interface/WeightedObject.h"
//#include "DataFormats/Math/interface/Point3D.h"

namespace flashgg {

    class Electron : public pat::Electron, public WeightedObject
    {

    public:
        Electron();
        Electron( const pat::Electron & );
        ~Electron();

        Electron *clone() const { return ( new Electron( *this ) ); }

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

