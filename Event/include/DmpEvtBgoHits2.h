/*
 *  $Id: DmpEvtBgoHits2.h, 2014-10-06 17:08:29 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 24/19/2014
*/

#ifndef DmpEvtBgoHits2_H
#define DmpEvtBgoHits2_H

#include "TVector3.h"
#include <map>

struct _dmpAHit
{
  _dmpAHit();
  _dmpAHit(short lid,short bid);
  ~_dmpAHit();
  _dmpAHit &operator=(const _dmpAHit &r);
  void Reset();

  double    fEnergy;        // unit Mips
  short     fLayer;
  short     fBar;
  TVector3  fPosition;        // unit mm, position x,y,z
  short     fUsedDyID_s0;
  // 2,5,8
  // -8:    should use dynode 8, but dy5-dy8 error
  // -5:    should use dynode 5, but dy2-dy5 error
  // -2:    should use dynode 2, but dy2 overflow
  // 0:     not has any signal dynode 2,5,8
  short     fUsedDyID_s1;

  ClassDef(_dmpAHit,1)
};

typedef std::vector<_dmpAHit*>  _dmpHits;

class DmpEvtBgoHits2 : public TObject
{
/*
 *  DmpEvtBgoHits2
 *  
 *  One DmpEvtBgoHits2 is a response of one Event.
*/
public:
  DmpEvtBgoHits2();
  ~DmpEvtBgoHits2();
  DmpEvtBgoHits2 &operator=(const DmpEvtBgoHits2 &r);
  void Reset();
  void LoadFrom(DmpEvtBgoHits2 *r);

public:
  double    GetTotalE()const;
  bool      GetFiredBars(short layerid,_dmpHits &r,bool useNoIsolatedBars = false)const;    // if no has any bar, return false.
  double    GetTotalE(short layerid, bool useNoIsolatedBars = false)const;
  double    GetTotalE(short l0,short l1, bool useNoIsolatedBars = false)const;
  short     GetFiredBarNumber(short layerid,bool useNoIsolatedBars = false)const;

  _dmpAHit  *GetEMaxBar(short layerid)const;         // NOTE:    return 0, if no exist
  _dmpAHit  *GetBar(short layerid,short barID)const; // NOTE:   return 0, if no exist
  short     GetEMaxBarID(short layerid)const;       // NOTE:    return -1, if no has any bar
  double    S1S3Ratio(short layerid)const;          // NOTE:    return -1 if S1 bar = {0 | 21}

  double    GetCoGBar(short layerid,bool useNoIsolatedBars = false)const;          // NOTE:    return -1, if no has any bar
  bool      GetCoG(short layerid,TVector3 &r,bool useNoIsolatedBars = false)const; // return false, if no has any bar
  bool      GetPositionInPlane(short planeID,TVector3 &r,bool useNoIsolatedBars = false)const;    // NOTE:    return false, if any layer not has bars
  double    GetRMS2(short layerid,bool useNoIsolatedBars = false)const;       // NOTE: return -1, if no has any bar in this layer
  double    GetF(short layerid, bool useNoIsolatedBars = false)const;    // NOTE:: return -1, if no has any bars in this layer
  double    GetG(short layerid, bool useNoIsolatedBars = false)const;   // NOTE:: return -1, if no has any bars in this layer

public:
  _dmpHits     fHits;     // all fired bars

  ClassDef(DmpEvtBgoHits2,1)
};

typedef DmpEvtBgoHits2 DmpEvtPsdHits;

#endif

