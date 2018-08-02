#ifndef ANALYSIS_DiPhotonTagBase_h
#define ANALYSIS_DiPhotonTagBase_h

#include "Analysis/DataFormats/interface/WeightedObject.h"

namespace Analysis {

    class TaggerBase : public WeightedObject
    {
    public:

        TaggerBase();
        virtual ~TaggerBase(); 
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

