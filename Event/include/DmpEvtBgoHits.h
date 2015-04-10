/*
 *  $Id: DmpEvtBgoHits.h, 2014-10-06 17:08:29 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 24/19/2014
*/

#ifndef DmpEvtBgoHits_H
#define DmpEvtBgoHits_H

#include "TVector3.h"
#include <map>

class DmpEvtBgoHits : public TObject{
/*
 *  DmpEvtBgoHits
 *  
 *  One DmpEvtBgoHits is a response of one Event.
*/
public:
  DmpEvtBgoHits();
  ~DmpEvtBgoHits();
  DmpEvtBgoHits &operator=(const DmpEvtBgoHits &r);
  void Reset();
  void LoadFrom(DmpEvtBgoHits *r);
  short GetHittedBarNumber()const{return fGlobalBarID.size();}

public:
  double    TotalEnergy()const;
  double    TotalEnergy(const short &layerid)const;
  double    TotalEnergy(const short &l0,const short &l1)const;
  double    TotalEnergyS0()const;
  double    TotalEnergyS0(const short &layerid)const;
  double    TotalEnergyS1()const;
  double    TotalEnergyS1(const short &layerid)const;
  short     FiredBarNumber(const short &layerid = -1)const;
  double    CentroidAddress(const short &layerid)const;
  TVector3  CentroidPosition(const short &layerid)const;
  double    OvershootRatio(const short &layerid)const;       // centroid affected by last big signal
  TVector3  PositionInPlane(const short &planeID)const;
  double    RMS2(const short &layerid)const;
  double    FValue(const short &layerid)const;
  std::vector<double>  EnergyArray(const short &layerid)const;

public:
  std::vector <short>     fGlobalBarID;     // unique sensitive detector bar ID, using DmpBgoBase to construct it
  std::vector <double>    fEnergy;          // unit MeV
  std::vector <double>    fES0;             // unit MeV, from side 0
  std::vector <double>    fES1;             // unit MeV, from side 1
  std::vector <TVector3>  fPosition;        // unit mm, position x,y,z

  ClassDef(DmpEvtBgoHits,1)
};

#endif

