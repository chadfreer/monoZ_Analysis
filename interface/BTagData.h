#ifndef BTagData_h
#define BTagData_h

#include "TROOT.h"
#include "TObject.h"
#include "CondFormats/BTauObjects/interface/BTagCalibration.h"

// Streamable wrapper for BTagCalibration
// because it takes so damn long to load from .csv files
// and if we want to keep using proof we need to be able to
// serialize and send the data.
// Then for some reason we need to instantiate a copy to force
// CMSSW's auto library loader to read in the dictionary before
// trying to compile this otherwise we get a huge indecipherable error
class BTagData : public TNamed {
  public:
    BTagData() {}
    BTagData(const char *name, const char *title) : TNamed(name, title) {}
    BTagData(const TString &name, const TString &title) : TNamed(name, title) {}
    virtual ~BTagData() {};

    BTagCalibration data;

  ClassDef(BTagData,1);
};

#endif
