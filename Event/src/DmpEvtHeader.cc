/*
 *  $Id: DmpEvtHeader.cc, 2014-10-24 00:33:41 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 09/10/2014
*/

#include <iostream>
#include "DmpEDetectorID.h"
#include "DmpEvtHeader.h"

ClassImp(DmpEvtHeader)

//-------------------------------------------------------------------
DmpEvtHeader::DmpEvtHeader(){
  Reset();
}

//-------------------------------------------------------------------
DmpEvtHeader::~DmpEvtHeader(){
}

//-------------------------------------------------------------------
DmpEvtHeader& DmpEvtHeader::operator=(const DmpEvtHeader &r){
  fTrigger = r.fTrigger;
  fDeltaTime = r.fDeltaTime;
  fSecond = r.fSecond;
  fMillisecond = r.fMillisecond;
  fTriggerStatus = r.fTriggerStatus;
  fPsdStatus = r.fPsdStatus;
  fStkStatus = r.fStkStatus;
  fBgoStatus = r.fBgoStatus;
  fNudStatus = r.fNudStatus;
}

//-------------------------------------------------------------------
void DmpEvtHeader::Reset(){
  fTrigger = -9;
  fDeltaTime = -9;
  fSecond = -9;
  fMillisecond = -9;
  fTriggerStatus.reset();
  fPsdStatus.reset();
  fStkStatus.reset();
  fBgoStatus.reset();
  fNudStatus.reset();
  fTmpDeltaTime = 0.0;
}

//-------------------------------------------------------------------
void DmpEvtHeader::LoadFrom(DmpEvtHeader *r){
  fTrigger = r->fTrigger;
  fDeltaTime = r->fDeltaTime;
  fSecond = r->fSecond;
  fMillisecond = r->fMillisecond;
  fTriggerStatus = r->fTriggerStatus;
  fPsdStatus = r->fPsdStatus;
  fStkStatus = r->fStkStatus;
  fBgoStatus = r->fBgoStatus;
  fNudStatus = r->fNudStatus;
}

//-------------------------------------------------------------------
bool DmpEvtHeader::IsGoodEvent(const short &id)const{
  bool v = true;
    // fake data is good for this event
  if(id == DmpEDetectorID::kPsd){
    v = (fPsdStatus.count()==1 && fPsdStatus.test(6))?true:false;
  }else if(id == DmpEDetectorID::kStk){
    v = (fStkStatus.count()==1 && fStkStatus.test(6))?true:false;
  }else if(id == DmpEDetectorID::kBgo){
    v = (fBgoStatus.count()==1 && fBgoStatus.test(6))?true:false;
  }else if(id == DmpEDetectorID::kNud){
    v = (fNudStatus.count()==1 && fNudStatus.test(6))?true:false;
  }else if(id == -1){
    v = (IsGoodEvent(DmpEDetectorID::kPsd) && IsGoodEvent(DmpEDetectorID::kStk) && IsGoodEvent(DmpEDetectorID::kBgo) && (IsGoodEvent(DmpEDetectorID::kNud)));
  }else{
    std::cout<<"WARNING:  parameter range: 0 ~ 3"<<std::endl;
    v = false;
  }
  return v;
}

//-------------------------------------------------------------------
bool DmpEvtHeader::IsFakeData(const short &id)const{
  bool v = false;
  if(id == DmpEDetectorID::kPsd){
    v = fPsdStatus[6];
  }else if(id == DmpEDetectorID::kStk){
    v = fStkStatus[6];
  }else if(id == DmpEDetectorID::kBgo){
    v = fBgoStatus[6];
  }else if(id == DmpEDetectorID::kNud){
    v = fNudStatus[6];
  }else{
    std::cout<<"WARNING:  parameter range: 0 ~ 3"<<std::endl;
  }
  return v;
}

//-------------------------------------------------------------------
bool DmpEvtHeader::TriggersMatch(const short &id)const{
  bool v = true;
  if(id == DmpEDetectorID::kPsd){
    v = not fPsdStatus[0];
  }else if(id == DmpEDetectorID::kStk){
    v = not fStkStatus[0];
  }else if(id == DmpEDetectorID::kBgo){
    v = not fBgoStatus[0];
  }else if(id == DmpEDetectorID::kNud){
    v = not fNudStatus[0];
  }else if(-1 == id){
    v = not (fPsdStatus[1] || fBgoStatus[1] || fNudStatus[1] || fStkStatus[1]);
  }else{
    std::cout<<"WARNING:  parameter range: 0 ~ 3"<<std::endl;
    v = false;
  }
  return v;
}

//-------------------------------------------------------------------
short DmpEvtHeader::ChoosedTriggerType(const short &group_id)const{
  short id = -1;
  if(4 == group_id){
    id = (fTriggerStatus.to_ulong()>>27) & 0x07;
  }else if(3 == group_id){
    id = (fTriggerStatus.to_ulong()>>22) & 0x1f;
  }else if(2 == group_id){
    id = (fTriggerStatus.to_ulong()>>20) & 0x03;
  }else if(1 == group_id){
    id = (fTriggerStatus.to_ulong()>>18) & 0x03;
  }else if(0 == group_id){
    id = (fTriggerStatus.to_ulong()>>16) & 0x03;
  }
  return id;
}

/*
DmpSubDetStatus DmpEvtHeader::GetSubDetectorStatus(const short &id)const
{
  DmpSubDetStatus v;
  if(id == DmpEDetectorID::kPsd){
    v = fPsdStatus;
  }else if(id == DmpEDetectorID::kStk){
    v = fStkStatus;
  }else if(id == DmpEDetectorID::kBgo){
    v = fBgoStatus;
  }else if(id == DmpEDetectorID::kNud){
    v = fNudStatus;
  }else{
    std::cout<<"WARNING:  parameter range: 0 ~ 3"<<std::endl;
  }
  return v;
}
*/
//-------------------------------------------------------------------
void DmpEvtHeader::SetTriggerChoose(const unsigned short &choosing){
  std::bitset<16> tmp = choosing;
  for(size_t i=0;i<14;++i){ // bit 31, bit 30 are not for class type
    if(tmp.test(i)){
      fTriggerStatus.set(16+i);
    }
  }
}

//-------------------------------------------------------------------
void DmpEvtHeader::SetTriggerGenerate(const unsigned char &g){
  std::bitset<8> tmp = g;
  for(size_t i=0;i<7;++i){  // bit 15 is not used
    if(tmp.test(i)){
      fTriggerStatus.set(8+i);
    }
  }
}

//-------------------------------------------------------------------
void DmpEvtHeader::SetTriggerEnable(const unsigned char &e){
  std::bitset<8> tmp = e;
  for(size_t i=0;i<7;++i){  // bit 7 is not used
    if(tmp.test(i)){
      fTriggerStatus.set(i);
    }
  }
}

//-------------------------------------------------------------------
void DmpEvtHeader::SetDeltaTime(const float &v){
  if(fTmpDeltaTime > 6){
    fDeltaTime = (fTmpDeltaTime>999999)?999:(fTmpDeltaTime/6)*6+v;
  }else{
    fDeltaTime = v;
  }
}

//-------------------------------------------------------------------
void DmpEvtHeader::SetTime(const int &s,const short &ms){
  static int last_s = 0, last_ms = 0;
  fSecond = s;
  fMillisecond = ms;
  fTmpDeltaTime = (fSecond-last_s)*1000 + fMillisecond - last_ms;
  last_s = fSecond;
  last_ms = fMillisecond;
}

//-------------------------------------------------------------------
void DmpEvtHeader::SetTime(const int &s,const short &ms,const float &v){
  static int last_s = 0, last_ms = 0;
  fSecond = s;
  fMillisecond = ms;
  int deltaTime = (fSecond-last_s)*1000 + fMillisecond - last_ms;
  if(deltaTime > 6){
    fDeltaTime = (deltaTime>999999)?-99:(deltaTime/6)*6+v;
  }else{
    fDeltaTime = v;
  }
  last_s = fSecond;
  last_ms = fMillisecond;
}

//-------------------------------------------------------------------
void DmpEvtHeader::SetTag(const short &id,TagType tagType){
  if(id == DmpEDetectorID::kPsd){
    fPsdStatus.set(tagType);
  }else if(id == DmpEDetectorID::kStk){
    fStkStatus.set(tagType);
  }else if(id == DmpEDetectorID::kBgo){
    fBgoStatus.set(tagType);
  }else if(id == DmpEDetectorID::kNud){
    fNudStatus.set(tagType);
  }else{
    std::cout<<"WARNING:  parameter range: 0 ~ 3"<<std::endl;
    throw;
  }
}

bool DmpEvtHeader::GeneratedTrigger(const short &group_id)const
{
  if(group_id >4){
    std::cout<<"WARNING:  parameter range: 0 ~ 4"<<std::endl;
    return false;
  }
  return fTriggerStatus[8+group_id];
} 

bool DmpEvtHeader::EnabledTrigger(const short &group_id)const
{
  if(group_id >4){
    std::cout<<"WARNING:  parameter range: 0 ~ 4"<<std::endl;
    return false;
  }
  return fTriggerStatus[group_id];
}
