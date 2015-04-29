/*
 *  $Id: DmpEvtBgoHits2.cc, 2014-10-06 17:06:59 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 16/12/2013
*/

#include <algorithm>

#include "DmpEvtBgoHits2.h"

ClassImp(_dmpAHit)

_dmpAHit::_dmpAHit(){
  Reset();
}

_dmpAHit::_dmpAHit(short l,short b){
  Reset();
  fLayer = l;
  fBar =b ;
}

_dmpAHit::~_dmpAHit(){
}

_dmpAHit& _dmpAHit::operator=(const _dmpAHit &r){
  fEnergy = r.fEnergy;
  fLayer = r.fLayer;
  fBar = r.fBar;
  fPosition = r.fPosition;
  fUsedDyID_s0 = r.fUsedDyID_s0;
  fUsedDyID_s1 = r.fUsedDyID_s1;
}

void _dmpAHit::Reset(){
  fEnergy = 0;
  fLayer = -1;
  fBar = -1;
  fPosition.SetXYZ(0,0,0);
  fUsedDyID_s0 = -1;
  fUsedDyID_s1 = -1;
}

ClassImp(DmpEvtBgoHits2)

//------------------------------------------------------------------------------
DmpEvtBgoHits2::DmpEvtBgoHits2(){
}

//------------------------------------------------------------------------------
DmpEvtBgoHits2::~DmpEvtBgoHits2(){
  Reset();
}

//-------------------------------------------------------------------
DmpEvtBgoHits2& DmpEvtBgoHits2::operator=(const DmpEvtBgoHits2 &r){
  Reset();
  fHits = r.fHits;
}

//-------------------------------------------------------------------
void DmpEvtBgoHits2::Reset(){
  for(short i=0;i<fHits.size();++i){
    delete fHits.at(i);
  }
  fHits.clear();
}

//-------------------------------------------------------------------
void DmpEvtBgoHits2::LoadFrom(DmpEvtBgoHits2 *r)
{
  Reset();
  fHits = r->fHits;
}

//-------------------------------------------------------------------
bool DmpEvtBgoHits2::GetFiredBars(short l,_dmpHits &back, bool useNoIsolatedBars)const
{
  back.clear();
  for(int i=0;i<fHits.size();++i){
    if(fHits[i]->fLayer == l){
      back.push_back(fHits[i]);
    }
  }
  if(useNoIsolatedBars){
    _dmpHits no_is;
    short bid = 0;
    for(int i=0;i<back.size();++i){
      bid = back[i]->fBar;
      for(int j=0;j<back.size();++j){
        if(back[j]->fBar == bid +1 || back[j]->fBar == bid -1){
          no_is.push_back(back[i]);
          break;
        }
      }
    }
    back.clear();
    back = no_is;
  }
  return back.size();
}

//-------------------------------------------------------------------
double DmpEvtBgoHits2::GetTotalE()const
{
  double e=0.;
  for(int i=0;i<fHits.size();++i){
    e += fHits[i]->fEnergy;
  }
  return e;
}

//-------------------------------------------------------------------
double DmpEvtBgoHits2::GetTotalE(short lid,bool useNoIsolatedBars)const{
  double e = 0.;
  _dmpHits bars;
  this->GetFiredBars(lid,bars,useNoIsolatedBars);
  for(int i=0;i<bars.size();++i){
    e += bars[i]->fEnergy;
  }
  return e;
}

//-------------------------------------------------------------------
double DmpEvtBgoHits2::GetTotalE(short l0,short l1,bool useNoIsolatedBars)const{
  double e=0.;
  for(short i=0;i<14;++i){
    if((l0 <= i) && (i <= l1)){
      e += this->GetTotalE(i,useNoIsolatedBars);
    }
  }
  return e;
}

short DmpEvtBgoHits2::GetFiredBarNumber(short l,bool useNoIsolatedBars)const
{
  _dmpHits bars;
  this->GetFiredBars(l,bars,useNoIsolatedBars);
  return bars.size();
}

_dmpAHit* DmpEvtBgoHits2::GetEMaxBar(short l)const
{
  _dmpHits bars;
  _dmpAHit *aBar = 0;
  this->GetFiredBars(l,bars,false);
  double e=0;
  for(int i=0;i<bars.size();++i){
    if(bars[i]->fEnergy > e){
      aBar = bars[i];
    }
  }
  return aBar;
}

_dmpAHit* DmpEvtBgoHits2::GetBar(short l,short b)const
{
 for(int i=0;i<fHits.size();++i){
   if(l == fHits[i]->fLayer && b == fHits[i]->fBar){
     return fHits[i];
   }
 }
 return 0;
}

short DmpEvtBgoHits2::GetEMaxBarID(short layerid)const
{
  _dmpAHit *aBar = this->GetEMaxBar(layerid);
  if(aBar){
    return aBar->fBar;
  }
  return -1;
}

//-------------------------------------------------------------------
double DmpEvtBgoHits2::S1S3Ratio(short lid)const
{
  _dmpAHit *bar0 = this->GetEMaxBar(lid);
  if(bar0 == 0){return -1;}

  _dmpHits bars;
  this->GetFiredBars(lid,bars,false);
  double e_1= 0,e_2 =0;
  for(int i=0;i<bars.size();++i){
    if(bars[i]->fBar == bar0->fBar+1){
      e_1 = bars[i]->fEnergy;
    }
    if(bars[i]->fBar == bar0->fBar-1){
      e_2 = bars[i]->fEnergy;
    }
  }
  double e0 = bar0->fEnergy;
  return e0 / (e_1+e_2+e0);
}

//-------------------------------------------------------------------
double DmpEvtBgoHits2::GetCoGBar(short lid,bool useNoIsolatedBars)const
{
  _dmpHits bars;
  if(not this->GetFiredBars(lid,bars,useNoIsolatedBars)){
    return -1;
  }
  double centroid = 0.;
  double totE=0;
  for(int i=0;i < bars.size();++i){
    centroid += (bars[i]->fBar+1) * bars[i]->fEnergy;   // first bar id is 0
    totE += bars[i]->fEnergy;
  }
  return centroid / totE - 1;
}

//-------------------------------------------------------------------
bool DmpEvtBgoHits2::GetCoG(short lid,TVector3 &r,bool useNoIsolatedBars)const{
  double  bid_left = this->GetCoGBar(lid,useNoIsolatedBars);
  if(bid_left<0) return false;
  _dmpAHit *left_b = this->GetBar(lid,(short)bid_left);
  _dmpAHit *right_b = this->GetBar(lid,(short)bid_left+1);
  TVector3 left,right;
  if(left_b){ left = left_b->fPosition;}
  if(right_b){ right = right_b->fPosition;}
  r = left + (right - left)*(bid_left - (short)bid_left);
  return true;
}

//-------------------------------------------------------------------
bool DmpEvtBgoHits2::GetPositionInPlane(short planeID,TVector3 &r,bool useNoIsolatedBars)const{
  TVector3 up;
  if(not GetCoG(planeID*2,up,useNoIsolatedBars)){
    return false;
  }
  TVector3 down;
  if(not GetCoG(planeID*2+1,up,useNoIsolatedBars)){
    return false;
  }
  r.SetXYZ(down.x(),up.y(),(down.z()+up.z())/2);
  return true;
}

//-------------------------------------------------------------------
double DmpEvtBgoHits2::GetRMS2(short lid,bool useNoIsolatedBars)const
{
  double b0 = GetCoGBar(lid,useNoIsolatedBars);
  if(b0 < 0) return -1;
  double v=0.;
  double eT = 0;
  _dmpHits   bars;
  this->GetFiredBars(lid,bars,useNoIsolatedBars);
  short bid =0;
  for(int i=0;i < bars.size();++i){
    bid = bars[i]->fBar;
    v += bars[i]->fEnergy * (b0-bid) * (b0-bid);
    eT += bars[i]->fEnergy;
  }
  return v/eT;
}

//-------------------------------------------------------------------
double DmpEvtBgoHits2::GetF(short lid,bool useNoIsolatedBars)const
{
  double rms2 = this->GetRMS2(lid,useNoIsolatedBars);
  if(rms2 == -1) return -1;
  return GetTotalE(lid,useNoIsolatedBars)/GetTotalE() * rms2;
}

//-------------------------------------------------------------------
double DmpEvtBgoHits2::GetG(short lid,bool useNoIsolatedBars)const
{
  double rms2 = this->GetRMS2(lid,useNoIsolatedBars);
  if(rms2 == -1) return -1;
  return GetTotalE()/GetTotalE(lid,useNoIsolatedBars) * rms2;
}

