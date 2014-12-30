#include "DmpEvtRec0.h"

ClassImp(DmpEvtRec0)

DmpEvtRec0::DmpEvtRec0():Bgo(0),Psd(0),Anc(0){
  //Header = new DmpEvtHeader();
  Bgo = new DmpEvtBgoHits();
  Psd = new DmpEvtPsdHits();
  Anc = new DmpEvtBTAnc();
  Nud[0] = -99.0;
  Nud[1] = -99.0;
  Nud[2] = -99.0;
  Nud[3] = -99.0;
}


//-------------------------------------------------------------------
DmpEvtRec0::~DmpEvtRec0(){
  //delete Header;
  delete Bgo;
  delete Psd;
  delete Anc;
}

