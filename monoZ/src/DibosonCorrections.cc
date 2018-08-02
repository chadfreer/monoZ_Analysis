#include "Analysis/monoZ/interface/DibosonCorrections.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "TLorentzVector.h"
#include "Math/VectorUtil.h"

ClassImp(DibosonCorrections)

bool DibosonCorrections::loadEwkTables(const char * fileWZ, const char * fileZZ) {
  if ( ewkTablesFilled_ ) {
    Error("loadEwkTables", "Tables already loaded!");
    return false;
  }
  std::ifstream inputfile;
  int acnt(0);
  std::string acorr;

  // Read WZ
  inputfile.open(fileWZ);
  if(!inputfile.is_open()) {
    Error("loadEwkTables", "WZ Input file %s not found. Try a relative path from $PWD", fileWZ);
    return false;
  }
  ewkTableWZ_.reserve(40000*5);
  while(!inputfile.eof()) {
    inputfile >> acorr;
    ewkTableWZ_.emplace_back(atof(acorr.c_str()));
    acnt++;
  }
  inputfile.close();
  acnt = 0;

  // Read ZZ
  inputfile.open(fileZZ);
  if(!inputfile.is_open()) {
    Error("loadEwkTables", "ZZ Input file %s not found. Try a relative path from $PWD", fileZZ);
    return false;
  }
  ewkTableZZ_.reserve(40000*5);
  while(!inputfile.eof()) {
    inputfile >> acorr;
    ewkTableZZ_.emplace_back(atof(acorr.c_str()));
    acnt++;
  }
  inputfile.close();

  ewkTablesFilled_ = true;
  return true;
}


float DibosonCorrections::getVVcorrection(SelectorBase *eventPtr, TheoryUncertainty shift) {
  return getEWcorrection(eventPtr, shift, true);
}


float DibosonCorrections::getEWcorrection(SelectorBase *eventPtr, TheoryUncertainty shift, bool addNnloQcd) {
  // Has the table been filled?
  if ( ! ewkTablesFilled_ ) {
    Error("loadEwkTables", "No data in EWK tables! Must load the tables into object before use!");
    return 1.;
  }
  int process = 0;
  auto * table = &ewkTableZZ_;
  if(eventPtr->isDataset(SelectorBase::Dataset::MC_qqZZ_powheg)) {
    process = 1;
    table = &ewkTableZZ_;
  }
  else if(eventPtr->isDataset(SelectorBase::Dataset::MC_WZ_powheg)) {
    process = 2;
    table = &ewkTableWZ_;
  }
  else {
    // Called with some dataset we don't want to correct!
    return 1.;
  }

  // Objects to be read from EventTuple
  auto& gpId = eventPtr->genParticle_id;
  //auto& gpSt = eventPtr->genParticle_flags;
  auto& gpP4 = eventPtr->genParticle_p4;

  // Identify initial-state partons (both quarks and gluons) and Z/W bosons
  // (Are Z's and W's always included in the GenParticle collection??
  //  Unsafe!!! But OK for now...)
  // For reference, check arXiv:1401.3964, page 14
  TLorentzVector q1, q2, v1, v2;
  unsigned int fq1 = std::abs(gpId[0]);
  unsigned int fq2 = std::abs(gpId[1]);

  if( (fq1>=1 && fq1<=6) || fq1==21 )
    q1 = TLorentzVector( gpP4[0].X(), gpP4[0].Y(), gpP4[0].Z(), gpP4[0].T() );
  else
    throw cms::Exception("monoZ/getEWcorrection")
      << "GenParticle element 0 is neither a quark nor a gluon.\n";

  if( (fq2>=1 && fq2<=6) || fq2==21 )
    q2 = TLorentzVector( gpP4[1].X(), gpP4[1].Y(), gpP4[1].Z(), gpP4[1].T() );
  else
    throw cms::Exception("monoZ/getEWcorrection")
      << "GenParticle element 1 is neither a quark nor a gluon.\n";

  if( gpId[2]==23 || std::abs(gpId[2])==24 )
    v1 = TLorentzVector( gpP4[2].X(), gpP4[2].Y(), gpP4[2].Z(), gpP4[2].T() );
  else
    throw cms::Exception("monoZ/getEWcorrection")
      << "GenParticle element 2 is neither a Z nor a W boson.\n";

  if( gpId[3]==23 || std::abs(gpId[3])==24 )
    v2 = TLorentzVector( gpP4[3].X(), gpP4[3].Y(), gpP4[3].Z(), gpP4[3].T() );
  else
    throw cms::Exception("monoZ/getEWcorrection")
      << "GenParticle element 3 is neither a Z nor a W boson.\n";

  // Diboson center of mass
  TLorentzVector vv = v1 + v2;

  // Mandelstam variable s (square of center of mass energy)
  float sqrshat = vv.Mag();
  float    shat = vv.Mag2();

  // Boost to the VV center of mass frame
  q1.Boost( -vv.BoostVector() );
  q2.Boost( -vv.BoostVector() );
  v1.Boost( -vv.BoostVector() );

  // Unitary vectors
  TVector3 uq1 = q1.Vect() * (1./q1.P());
  TVector3 uq2 = q2.Vect() * (1./q2.P());
  TVector3 uv1 = v1.Vect() * (1./v1.P());

  // Effective beam axis
  TVector3 dq = uq1 - uq2;
  TVector3 udq = dq * (1./dq.Mag()); // effective beam direction
  float costheta = udq.Dot(uv1);

  float mz = 91.1876; // Z boson mass, assumed to be on-shell
  float mw = 80.385;  // W boson mass, assumed to be on-shell
  float that = 0.;

  if(process==1)
    that = mz*mz - 0.5*shat + costheta * std::sqrt(0.25*shat*shat - mz*mz*shat);
  else if(process==2) {
    double b = 0.5/sqrshat * std::sqrt(std::pow(shat - mz*mz - mw*mw, 2) - 4*mw*mw*mz*mz);
    double a = std::sqrt(b*b + mz*mz);
    // Bad calculation! But needed to boost to the WZ c.m. frame (different masses)
    that = mz*mz - sqrshat * (a - b*costheta);
  }

  //
  // Select table row, based on s_hat and t_hat
  //
  unsigned int itab = 40000;
  float sqrshatmax = 0.8E+04; // highest value of sqrt(s_hat) in the table
  // In case sqrshat exceeds sqrshatmax
  // (the table is for 8 TeV, we run at 13 TeV)
  if(sqrshat>sqrshatmax) itab = 39800;
  else {
    for(unsigned int itmp=0; itmp<40000; itmp+=200) {
      if(std::abs(sqrshat - (*table)[itmp*5+0])<sqrshatmax) {
        sqrshatmax = std::abs(sqrshat - (*table)[itmp*5+0]);
        itab = itmp;
      }
      else break;
    }
  }

  sqrshatmax = (*table)[(itab+199)*5+1];
  // In case sqrshat exceeds sqrshatmax
  // (the table is for 8 TeV, we run at 13 TeV)
  if(that>sqrshatmax) itab += 199;
  else {
    sqrshatmax = 0.1E+09;
    for(unsigned int itmp=itab; itmp<itab+200; ++itmp) {
      if(std::abs(that - (*table)[itmp*5+1])<sqrshatmax) {
        sqrshatmax = std::abs(that - (*table)[itmp*5+1]);
        itab = itmp;
      }
      else break;
    }
  }


  //
  // Select table column, based on quark flavor (for ZZ)
  //
  unsigned int jtab = 2; // always for WZ
  if(process==1) { // ZZ
    // Flavor of incident quark (std::min in case one is a gluon)
    int qtype = std::min(fq1, fq2);
    if     (qtype==1 || qtype==3) jtab = 3; // d, s
    else if(qtype==2 || qtype==4) jtab = 2; // u, c
    else if(qtype==5            ) jtab = 4; // b
    else
      throw cms::Exception("monoZ/getEWcorrection")
        << "Unknown quark type.\n"; // no top, right?
  }

  float weight = 1. + (*table)[itab*5+jtab];

  if(shift==kEWK_up || shift==kEWK_down) {
    size_t nlept = 0;
    float sumptl = 0.;
    for(size_t h=4; h<gpP4.GetSize(); ++h) {
      if(std::abs(gpId[h])<11 || std::abs(gpId[h])>16) continue;
      sumptl += gpP4[h].Pt();
      nlept++;
    }
    if(nlept!=4) {
      // TEMPORARY: if we don't know how to handle it,
      // let's take the conservative approach...
      // We should use generator flags to make sure
      // we don't double leptons in case of bremsstrahlung
      sumptl = 1E-9;
    }

    double rhozz = vv.Pt()/sumptl;

    // Average QCD NLO k factors from arXiv:1105.0020
    double dkfactor_qcd = 0.;
    if     (process==1)       dkfactor_qcd = 15.99/ 9.89 - 1.; // ZZ
    else if(process==2) {
      if(gpId[2]*gpId[3]>0) dkfactor_qcd = 28.55/15.51 - 1.; // W+Z
      else                  dkfactor_qcd = 18.19/ 9.53 - 1.; // W-Z
    }
    double ewkUncert = 1. + (rhozz<0.3 ? std::abs(dkfactor_qcd*(*table)[itab*5+jtab]) : std::abs((*table)[itab*5+jtab]));
    if     (shift==kEWK_up  ) weight *= ewkUncert;
    else if(shift==kEWK_down) weight /= ewkUncert;
  }

  // If you want, add the NNLO QCD k factor
  // (used by function getVVcorrection(...))
  if(addNnloQcd && process==1)
    weight *= getQCDNNLOcorrection( std::abs(ROOT::Math::VectorUtil::DeltaPhi(v1, v2)) ); // DeltaPhi must be in [-Pi,Pi[

  return weight;
}

float DibosonCorrections::getQCDNNLOcorrection(SelectorBase *eventPtr) {
  // Objects to be read from EventTuple
  auto& gpId = eventPtr->genParticle_id;
  //auto& gpSt = eventPtr->genParticle_flags;
  auto& gpP4 = eventPtr->genParticle_p4;

  // Identify Z/W bosons
  // (Are Z's and W's always included in the GenParticle collection??
  //  Unsafe!!! But OK for now...)
  LorentzVector v1, v2;

  if( gpId[2]==23 || std::abs(gpId[2])==24 )
    v1 = gpP4[2];
  else
    throw cms::Exception("monoZ/getQCDNNLOcorrection")
      << "GenParticle element 2 is neither a Z nor a W boson.\n";

  if( gpId[3]==23 || std::abs(gpId[3])==24 )
    v2 = gpP4[3];
  else
    throw cms::Exception("monoZ/getQCDNNLOcorrection")
      << "GenParticle element 3 is neither a Z nor a W boson.\n";

  return getQCDNNLOcorrection( std::abs(ROOT::Math::VectorUtil::DeltaPhi(v1, v2)) ); // DeltaPhi must be in [-Pi,Pi]
}

namespace {
  constexpr double nnloQcdKfactor_[32] { 
    1.513834489150, //  0 
    1.541738780180, //  1 
    1.497829632510, //  2 
    1.534956782920, //  3 
    1.478217033060, //  4 
    1.504330859290, //  5 
    1.520626246850, //  6 
    1.507013090030, //  7 
    1.494243156250, //  8 
    1.450536096150, //  9 
    1.460812521660, // 10 
    1.471603622200, // 11 
    1.467700038200, // 12 
    1.422408690640, // 13 
    1.397184022730, // 14 
    1.375593447520, // 15 
    1.391901318370, // 16 
    1.368564350560, // 17 
    1.317884804290, // 18 
    1.314019950800, // 19 
    1.274641749910, // 20 
    1.242346606820, // 21 
    1.244727403840, // 22 
    1.146259351670, // 23 
    1.107804993520, // 24 
    1.042053646740, // 25 
    0.973608545141, // 26 
    0.872169942668, // 27 
    0.734505279177, // 28 
    1.163152837230, // 29 
    1.163152837230, // 30 
    1.163152837230  // 31 
  }; 
}

float DibosonCorrections::getQCDNNLOcorrection(float dphi) {
  // Find index in array of QCD NNLO k factors
  // E.g. dphi = 1.4578 --> dphi*10 = 14.578 --> rintl(dphi*10) = 14
  // Only difference w.r.t. the original function below:
  // upper edge of each bin is included in the next bin
  // e.g. 0.1 -> idx = 1 (instead of 0)
  size_t idx = rintl(dphi * 10.);
  if(idx>31) return 1.1;
  return nnloQcdKfactor_[idx];
}

