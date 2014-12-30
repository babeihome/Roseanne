
#ifndef DmpEvtRec0_H
#define DmpEvtRec0_H

//#include "DmpEvtHeader.h"
#include "DmpEvtBgoHits.h"
#include "DmpEvtBTAnc.h"


class DmpEvtRec0 : public TObject{
public:
  DmpEvtRec0();
  ~DmpEvtRec0();


public:
  //DmpEvtHeader      *Header;
  DmpEvtBgoHits     *Bgo;
  DmpEvtPsdHits     *Psd;
  float             Nud[4];
  DmpEvtBTAnc       *Anc;


  ClassDef(DmpEvtRec0,1)
};

#endif
