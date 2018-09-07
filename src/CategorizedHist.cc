#include "Analysis/monoZ/interface/CategorizedHist.h"

ClassImp(CategorizedHist)

CategorizedHist::~CategorizedHist()
{
  // Because the class is serializable,
  // ROOT is aware of this->Hists and will
  // deallocate the TH1* for us
}

void
CategorizedHist::SetDirectory(TDirectory * dir)
{
  for (auto hist : Hists) {
    hist.second->SetDirectory(dir);
  }
}

void
CategorizedHist::Add(CategorizedHist * toAdd)
{
  for (auto hist : Hists) {
    for (auto other : toAdd->Hists) {
      if (other.first == hist.first) {
        hist.second->Add(other.second);
      }
    }
  }
}

Long64_t
CategorizedHist::Merge(TCollection * coll)
{
  if (coll) {
    CategorizedHist * toAdd = 0;
    TIter next(coll);
    while ((toAdd = (CategorizedHist *) next())) {
      Add(toAdd);
    }
  }
  // Value should be ignored, not very meaningful here
  return 42;
}

void
CategorizedHist::SetXaxisBinLabel(int bin, const char * label)
{
  for (auto hist : Hists) {
    hist.second->GetXaxis()->SetBinLabel(bin, label);
  }
}

void
CategorizedHist::SetYaxisBinLabel(int bin, const char * label)
{
  for (auto hist : Hists) {
    hist.second->GetYaxis()->SetBinLabel(bin, label);
  }
}


CategorizedHist1D::CategorizedHist1D(const char *name, const char *title, int nBins, double xmin, double xmax) :
  CategorizedHist(name, title)
{
  for (const auto& cat : EventTupleUtil::CategoryNames) {
    auto hist = new TH1D(Form("%s_%s", cat.second, name), title, nBins, xmin, xmax);
    Hists.emplace_back(cat.first, hist);
  }
}

void
CategorizedHist1D::Fill(EventTupleUtil::EventCategory cat, double x, double weight)
{
  for (auto hist : Hists) {
    if ( hist.first & cat ) {
      hist.second->Fill(x, weight);
    }
  }
}

std::vector<TH1D*>
CategorizedHist1D::GetHists()
{
  std::vector<TH1D*> histList;
  for (auto hist : Hists) {
    histList.emplace_back(static_cast<TH1D*>(hist.second));
  }
  return histList;
}


CategorizedHist2D::CategorizedHist2D(const char *name, const char *title, int nBinsX, double xmin, double xmax, int nBinsY, double ymin, double ymax) :
  CategorizedHist(name, title)
{
  for (const auto& cat : EventTupleUtil::CategoryNames) {
    auto hist = new TH2D(Form("%s_%s", cat.second, name), title, nBinsX, xmin, xmax, nBinsY, ymin, ymax);
    Hists.emplace_back(cat.first, hist);
  }
}

void
CategorizedHist2D::Fill(EventTupleUtil::EventCategory cat, double x, double y, double weight)
{
  for (auto hist : Hists) {
    if ( hist.first & cat ) {
      static_cast<TH2D*>(hist.second)->Fill(x, y, weight);
    }
  }
}

std::vector<TH2D*>
CategorizedHist2D::GetHists()
{
  std::vector<TH2D*> histList;
  for (auto hist : Hists) {
    histList.emplace_back(static_cast<TH2D*>(hist.second));
  }
  return histList;
}

