/*
 *  $Id: DmpEvtBgoHits.cc, 2014-10-06 17:06:59 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 16/12/2013
*/

#include <algorithm>

#include "DmpEvtBgoHits.h"
#include "DmpBgoBase.h"

ClassImp(DmpEvtBgoHits)

//------------------------------------------------------------------------------
DmpEvtBgoHits::DmpEvtBgoHits(){
}

//------------------------------------------------------------------------------
DmpEvtBgoHits::~DmpEvtBgoHits(){
  Reset();
}

//-------------------------------------------------------------------
DmpEvtBgoHits& DmpEvtBgoHits::operator=(const DmpEvtBgoHits &r){
  Reset();
  fGlobalBarID = r.fGlobalBarID;
  fEnergy = r.fEnergy;
  fES0 = r.fES0;
  fES1 = r.fES1;
  fPosition = r.fPosition;
}

//-------------------------------------------------------------------
void DmpEvtBgoHits::Reset(){
  fGlobalBarID.clear();
  fEnergy.clear();
  fES0.clear();
  fES1.clear();
  fPosition.clear();
}

//-------------------------------------------------------------------
void DmpEvtBgoHits::LoadFrom(DmpEvtBgoHits *r){
  Reset();
  fGlobalBarID = r->fGlobalBarID;
  fEnergy = r->fEnergy;
  fES0 = r->fES0;
  fES1 = r->fES1;
  fPosition = r->fPosition;
}

//-------------------------------------------------------------------
double DmpEvtBgoHits::TotalEnergy()const{
  double e=0.;
  for(size_t i=0;i<fGlobalBarID.size();++i){
    e += fEnergy[i];
  }
  return e;
}

//-------------------------------------------------------------------
double DmpEvtBgoHits::TotalEnergyS0()const{
  double e=0.;
  for(size_t i=0;i<fGlobalBarID.size();++i){
    e += fES0[i];
  }
  return e;
}

//-------------------------------------------------------------------
double DmpEvtBgoHits::TotalEnergyS1()const{
  double e=0.;
  for(size_t i=0;i<fGlobalBarID.size();++i){
    e += fES1[i];
  }
  return e;
}

//-------------------------------------------------------------------
double DmpEvtBgoHits::TotalEnergy(const short &lid)const{
  double e = 0.;
  for(size_t i=0;i<fGlobalBarID.size();++i){
    if(DmpBgoBase::GetLayerID(fGlobalBarID[i]) == lid){
      e += fEnergy[i];
    }
  }
  return e;
}

//-------------------------------------------------------------------
double DmpEvtBgoHits::TotalEnergyS0(const short &lid)const{
  double e = 0.;
  for(size_t i=0;i<fGlobalBarID.size();++i){
    if(DmpBgoBase::GetLayerID(fGlobalBarID[i]) == lid){
      e += fES0[i];
    }
  }
  return e;
}

//-------------------------------------------------------------------
double DmpEvtBgoHits::TotalEnergyS1(const short &lid)const{
  double e = 0.;
  for(size_t i=0;i<fGlobalBarID.size();++i){
    if(DmpBgoBase::GetLayerID(fGlobalBarID[i]) == lid){
      e += fES1[i];
    }
  }
  return e;
}

//-------------------------------------------------------------------
short DmpEvtBgoHits::FiredBarNumber(const short &lid)const{
  short nTot = fGlobalBarID.size();
  short n = 0;
  if(lid == -1){
    return nTot;
  }else{
    for(size_t i=0;i<nTot;++i){
      if(DmpBgoBase::GetLayerID(fGlobalBarID[i]) == lid){
        ++n;
      }
    }
  }
  return n;
}

//-------------------------------------------------------------------
double DmpEvtBgoHits::CentroidAddress(const short &lid)const{
  std::vector<double> e_b=EnergyArray(lid);
  double centroid = 0.;
  for(size_t i =0;i<14;++i){
    centroid += e_b[i]*(i+1);   // first bar id is 0
  }
  return (centroid/22) - 1;
}

//-------------------------------------------------------------------
TVector3 DmpEvtBgoHits::CentroidPosition(const short &lid)const{
  double cAdd = CentroidAddress(lid);
  TVector3 left,right;
  for(size_t i=0;i<fGlobalBarID.size();++i){
    if(DmpBgoBase::GetLayerID(fGlobalBarID[i]) == lid){
      if(DmpBgoBase::GetBarID(fGlobalBarID[i]) == (short)cAdd){
        left = fPosition[i];
      }else if(DmpBgoBase::GetBarID(fGlobalBarID[i]) == ((short)cAdd)+1){
        right = fPosition[i];
      }else{
        continue;
      }
    }
  }
  right -= left;
  right *= (cAdd - ((short)cAdd));
  left += right;
  return left;
}

//-------------------------------------------------------------------
double DmpEvtBgoHits::OvershootRatio(const short &lid)const{
  std::vector<double> e_b=EnergyArray(lid);
  short centroid = (short)CentroidAddress(lid);
  return (e_b[centroid-1] + e_b[centroid+1]) / (2*e_b[centroid]);
}

//-------------------------------------------------------------------
TVector3 DmpEvtBgoHits::PositionInPlane(const short &planeID)const{
  TVector3 up = CentroidPosition(planeID*2);
  TVector3 down = CentroidPosition(planeID*2+1);
  TVector3 p;
  p.SetXYZ(down.x(),up.y(),(down.z()+up.z())/2);
  return p;
}

//-------------------------------------------------------------------
double DmpEvtBgoHits::TotalEnergy(const short &l0,const short &l1)const{
  double e=0.;
  for(short i=0;i<14;++i){
    if((l0 <= i) && (i <= l1)){
      e += TotalEnergy(i);
    }
  }
  return e;
}

//-------------------------------------------------------------------
double DmpEvtBgoHits::RMS2(const short &lid)const{
  double v=0.;
  std::vector<double> e_b=EnergyArray(lid);
  double centroid = CentroidAddress(lid);
  for(short i=0;i<14;++i){
    v += e_b[i]*(centroid-i)*(centroid-i);
  }
  return v/TotalEnergy(lid);
}

//-------------------------------------------------------------------
double DmpEvtBgoHits::FValue(const short &lid)const{
  return TotalEnergy(lid)*RMS2(lid)/TotalEnergy();
}

//-------------------------------------------------------------------
std::vector<double>  DmpEvtBgoHits::EnergyArray(const short &lid)const{
  std::vector<double> e_b(22,0.);
  for(size_t i=0;i<fGlobalBarID.size();++i){
    if(DmpBgoBase::GetLayerID(fGlobalBarID[i]) == lid){
      e_b[DmpBgoBase::GetBarID(fGlobalBarID[i])] = fEnergy[i];
    }
  }
  return e_b;
}


