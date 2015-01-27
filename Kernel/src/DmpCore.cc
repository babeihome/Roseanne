/*
 *  $Id: DmpCore.cc, 2014-09-21 14:58:08 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 22/04/2014
*/

#include <time.h>

#include "DmpCore.h"
#include "DmpRootIOSvc.h"
#include "DmpDataBuffer.h"
#include "DmpTimeConvertor.h"

//-------------------------------------------------------------------
DmpCore::DmpCore()
 :fAlgMgr(0),
  fSvcMgr(0),
  fLaunchTime(DmpTimeConvertor::Date2Second("2013-01-01 00:00:00")),
  fMaxEventNo(-1),
  fStartTime(fLaunchTime),
  fStopTime(DmpTimeConvertor::Date2Second("2053-01-01 00:00:00")),
  fInitializeDone(false),
  fTerminateRun(false),
  fCurrentEventID(0)    // must == 0
{
  std::cout<<"**************************************************"<<std::endl;
  std::cout<<"      Offline software of DAMPE (DMPSW)"<<std::endl;
  std::cout<<"      version:  1.1.0(dampeustc)"<<std::endl;
  std::cout<<"**************************************************"<<std::endl;
  fAlgMgr = DmpAlgorithmManager::GetInstance();
  fSvcMgr = DmpServiceManager::GetInstance();
  fSvcMgr->Append(DmpRootIOSvc::GetInstance());     // must use GetInstance instead of global variable, since, Mac create global variable later than DmpCore::DmpCore()
  fSvcMgr->Append(DmpDataBuffer::GetInstance());
}

//-------------------------------------------------------------------
DmpCore::~DmpCore(){
}

//-------------------------------------------------------------------
bool DmpCore::Initialize(){
  //*
  //* Important! First, initialize servises, then algorithms
  //*
  std::cout<<"\n  [DmpCore::Initialize] Initialize..."<<std::endl;
  if(not fSvcMgr->Initialize()) return false;
  if(not fAlgMgr->Initialize()) return false;
  gRootIOSvc->PrepareMetaData();
  std::cout<<"  [DmpCore::Initialize] ... initialized successfully"<<std::endl;
  fInitializeDone = true;
  return true;
}

//-------------------------------------------------------------------
bool DmpCore::Run(){
  if(not fInitializeDone){
    return false;
  }
  std::cout<<"\n  [DmpCore::Run] Running..."<<std::endl;
// *
// *  TODO: use cut of time range??
// *
  while((not fTerminateRun) && (fCurrentEventID <= fMaxEventNo)){
    if(gRootIOSvc->PrepareEvent(fCurrentEventID)){
      if(fAlgMgr->ProcessOneEvent()){
        gRootIOSvc->FillData("Event");
      }
      ++fCurrentEventID;
    }else{
      fTerminateRun = true;
      break;
    }
  }
  std::cout<<"  [DmpCore::Run] Done"<<std::endl;
  return true;
}

//-------------------------------------------------------------------
bool DmpCore::Finalize(){
  std::cout<<"\n  [DmpCore::Finalize] Finalize..."<<std::endl;
  //*
  //* Important! First, finalize algorithms, then services!
  //*
  fAlgMgr->Finalize();
  fSvcMgr->Finalize();
  std::cout<<"  [DmpCore::Finalize] ... finalized successfully"<<std::endl;
  return true;
}

//-------------------------------------------------------------------
void DmpCore::SetEventNumber(const long &n){
  fMaxEventNo = n;
  gRootIOSvc->JobOption()->SetOption("MaxEventNumber",boost::lexical_cast<std::string>(n));
}

//-------------------------------------------------------------------
void DmpCore::SetStartTime(const std::string &t0){
  fStartTime = DmpTimeConvertor::Date2Second(t0);
  gRootIOSvc->JobOption()->SetOption("StartTime",t0);
}

//-------------------------------------------------------------------
void DmpCore::SetStopTime(const std::string &t1){
  fStopTime = DmpTimeConvertor::Date2Second(t1);
  gRootIOSvc->JobOption()->SetOption("StopTime",t1);
}

//-------------------------------------------------------------------
DmpCore *gCore = DmpCore::GetInstance();


