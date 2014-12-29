#ifndef DmpEvtBTAnc_H
#define DmpEvtBTAnc_H

#include "Cluster.hh"
#include "TClonesArray.h" 

//-------------------------------------------------------------------
class DmpEvtBTAnc : public TObject{
public:
  DmpEvtBTAnc();
  virtual ~DmpEvtBTAnc();
  void LoadFrom(const DmpEvtBTAnc *&r);
  void Reset();

public:
  TClonesArray  *fAMSCls;      // AMS data
  int           fAdcC1;     // cherenkov 1
  int           fAdcC2;     // cherenkov 2
  int           fAdcPbGlass;        // behind S3. Beam off
  int           fAdcSc1;    // scintillator behind dump, PMT 1
  int           fAdcSc2;    // scintillator behind dump, PMT 2
  int           fAdcSc3;    // scintillator behind dump, PMT 3
  int           fAdcSc4;    // scintillator behind dump, PMT 4
  int           fAdcSd1;    // scintillator behind DAMPE, PMT 1
  int           fAdcSd2;    // scintillator behind DAMPE, PMT 2

public:
  ClassDef(DmpEvtBTAnc,1)
};

#endif


