#ifndef DibosonCorrections_h
#define DibosonCorrections_h

#include "SelectorBase.h"

class DibosonCorrections : public TNamed {
public:
  enum TheoryUncertainty {
    CentralValue,
    kEWK_up,
    kEWK_down,
    kQCD_up,
    kWCD_down,
  };
  DibosonCorrections() {}
  DibosonCorrections(const char *name, const char *title) : TNamed(name, title) {}
  DibosonCorrections(const TString &name, const TString &title) : TNamed(name, title) {}
  virtual ~DibosonCorrections() {}

  // Returns true on success
  bool loadEwkTables(const char * fileWZ, const char * fileZZ);

  float getVVcorrection(SelectorBase *eventPtr, TheoryUncertainty shift=CentralValue);
  float getEWcorrection(SelectorBase *eventPtr, TheoryUncertainty shift=CentralValue, bool addNnloQcd=false);
  float getQCDNNLOcorrection(SelectorBase *eventPtr);
  float getQCDNNLOcorrection(float dphizz);

private:
  std::vector<float> ewkTableWZ_;
  std::vector<float> ewkTableZZ_;
  bool ewkTablesFilled_{false};

  ClassDef(DibosonCorrections, 1)
};

#endif

