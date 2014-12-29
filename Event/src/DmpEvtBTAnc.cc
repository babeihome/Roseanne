#include "DmpEvtBTAnc.h"

ClassImp(DmpEvtBTAnc);

//-------------------------------------------------------------------
DmpEvtBTAnc::DmpEvtBTAnc():fAMSCls(0),fAdcC1(0),fAdcC2(0),fAdcPbGlass(0),
        fAdcSc1(0),
        fAdcSc2(0),
        fAdcSc3(0),
        fAdcSc4(0),
        fAdcSd1(0),
        fAdcSd2(0)
{
  fAMSCls = new TClonesArray("Cluster",3);
  fAMSCls->SetOwner();
}

//-------------------------------------------------------------------
DmpEvtBTAnc::~DmpEvtBTAnc(){
  fAMSCls->Delete();
  delete fAMSCls;
}

//-------------------------------------------------------------------
void DmpEvtBTAnc::LoadFrom(const DmpEvtBTAnc *&r){
  fAMSCls = (TClonesArray*)r->fAMSCls->Clone();
  fAdcC1 = r->fAdcC1;
  fAdcC2 = r->fAdcC2;
  fAdcPbGlass = r->fAdcPbGlass;
  fAdcSc1 = r->fAdcSc1;
  fAdcSc2 = r->fAdcSc2;
  fAdcSc3 = r->fAdcSc3;
  fAdcSc4 = r->fAdcSc4;
  fAdcSd1 = r->fAdcSd1;
  fAdcSd2 = r->fAdcSd2;
}

//-------------------------------------------------------------------
void DmpEvtBTAnc::Reset(){
  fAMSCls->Delete();
  fAdcC1 = 0;
  fAdcC2 = 0;
  fAdcPbGlass = 0;
  fAdcSc1 = 0;
  fAdcSc2 = 0;
  fAdcSc3 = 0;
  fAdcSc4 = 0;
  fAdcSd1 = 0;
  fAdcSd2 = 0;
}

