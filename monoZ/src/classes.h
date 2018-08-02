#include "Analysis/monoZ/interface/EventTuple.h"
#include "Analysis/monoZ/interface/BTagData.h"
#include "Analysis/monoZ/interface/CategorizedHist.h"
#include "Analysis/monoZ/interface/ScaleFactor.h"
#include "Analysis/monoZ/interface/SelectorBase.h"
#include "Analysis/monoZ/interface/DibosonCorrections.h"
#include "Analysis/monoZ/interface/METResponse.h"
#include "Analysis/monoZ/interface/NVtxDistribution.h"
#include "Analysis/monoZ/interface/MonoZSelector.h"
#include "Analysis/monoZ/interface/PhotonSelector.h"

namespace{
  namespace{
    Vector2D pVec2d;
    EventTuple pEventTuple;
    BTagData pBtag;
    CategorizedHist pCatHist;
    CategorizedHist1D pCatHist1;
    CategorizedHist2D pCatHist2;
    ScaleFactor pScaleFactor;
    SelectorBase pSelectorBase;
    DibosonCorrections pDibosonCorrections;
    METResponse pMETResponse;
    NVtxDistribution pNVtxDistribution;
    MonoZSelector pMonoZSelector;
    PhotonSelector pPhotonSelector;
  }
}
