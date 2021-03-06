#ifndef ScaleFactors_h
#define ScaleFactors_h

#include "TROOT.h"
#include "TObject.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"

// Streamable scale factor histogram object
// Add the histogram after instantiating, then you
// can pass it to proof, or save to file, or whatever
class ScaleFactor : public TNamed {
  public:
    enum Variation {
      CentralValue,
      ShiftUp,
      ShiftDown,
    };
    enum OverflowBehavior {
      AsInHist,
      NearestEntry,
      Unity
    };

    ScaleFactor() {}
    ScaleFactor(const char *name, const char *title) : TNamed(name, title) {}
    ScaleFactor(const TString &name, const TString &title) : TNamed(name, title) {}
    virtual ~ScaleFactor();

    void Set1DHist(TH1D * central, TH1D * shiftUp=0, TH1D * shiftDown=0, OverflowBehavior b=Unity);
    TH1D * Get1DHist(Variation var=CentralValue);
    void Set2DHist(TH2D * central, TH2D * shiftUp=0, TH2D * shiftDown=0, OverflowBehavior b=Unity);
    TH2D * Get2DHist(Variation var=CentralValue);
    void Set3DHist(TH3D * central, TH3D * shiftUp=0, TH3D * shiftDown=0, OverflowBehavior b=Unity);
    TH3D * Get3DHist(Variation var=CentralValue);

    double Evaluate1D(double x, Variation var=CentralValue) const;
    double Evaluate2D(double x, double y, Variation var=CentralValue) const;
    double Evaluate3D(double x, double y, double z, Variation var=CentralValue) const;

    void RegisterGlobalFunction(int dimension=1);

  private:
    void SetOverflowBins(TH1D * hist, OverflowBehavior b);
    void SetOverflowBins(TH2D * hist, OverflowBehavior b);
    void SetOverflowBins(TH3D * hist, OverflowBehavior b);

    TH1D * histCentral1D_{nullptr};
    TH1D * histShiftUp1D_{nullptr};
    TH1D * histShiftDown1D_{nullptr};
    TH2D * histCentral2D_{nullptr};
    TH2D * histShiftUp2D_{nullptr};
    TH2D * histShiftDown2D_{nullptr};
    TH3D * histCentral3D_{nullptr};
    TH3D * histShiftUp3D_{nullptr};
    TH3D * histShiftDown3D_{nullptr};

  ClassDef(ScaleFactor,2);
};

#endif
