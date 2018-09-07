#ifndef CategorizedHist_h
#define CategorizedHist_h

#include "TROOT.h"
#include "TNamed.h"
#include "TH1D.h"
#include "TH2D.h"

#include "EventTupleUtil.h"

class CategorizedHist : public TNamed {
  public:
    CategorizedHist() {};
    CategorizedHist(const char *name, const char *title) : TNamed(name, title) {};
    virtual ~CategorizedHist();

    void SetDirectory(TDirectory * dir);
    void Add(CategorizedHist * toAdd);
    Long64_t Merge(TCollection * coll);
    void SetXaxisBinLabel(int bin, const char * label);
    void SetYaxisBinLabel(int bin, const char * label);

  protected:
    std::vector<std::pair<const int, TH1*>> Hists;

  ClassDef(CategorizedHist,1);
};

class CategorizedHist1D : public CategorizedHist {
  public:
    CategorizedHist1D() {};
    CategorizedHist1D(const char *name, const char *title, int nBins, double xmin, double xmax);
    void Fill(EventTupleUtil::EventCategory cat, double x, double weight=1.);
    std::vector<TH1D*> GetHists();

  ClassDef(CategorizedHist1D,1);
};

class CategorizedHist2D : public CategorizedHist {
  public:
    CategorizedHist2D() {};
    CategorizedHist2D(const char *name, const char *title, int nBins, double xmin, double xmax, int nBinsY, double ymin, double ymax);
    void Fill(EventTupleUtil::EventCategory cat, double x, double y, double weight=1.);
    std::vector<TH2D*> GetHists();

  ClassDef(CategorizedHist2D,1);
};

#endif
