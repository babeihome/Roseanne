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
 :fJobTime(""),
  fAlgMgr(0),
  fSvcMgr(0),
  fLaunchTime(DmpTimeConvertor::Date2Second("2013-01-01 00:00:00")),
  fStartTime(DmpTimeConvertor::Date2Second("2010-01-01 00:00:00")),
  fStopTime(DmpTimeConvertor::Date2Second("2033-01-01 00:00:00")),
  fSeed(0),
  fInitializeDone(false),
  fTerminateRun(false),
  fMaxEventNo(-1),      // default < 0, for all events
  fInTimeEvents(0)      // must == 0
{
  std::cout<<"**************************************************"<<std::endl;
  std::cout<<"      Offline software of DAMPE (DMPSW)"<<std::endl;
  std::cout<<"      version:  2.1.0(dampeustc)"<<std::endl;
  std::cout<<"**************************************************"<<std::endl;
  fJobOpt = new DmpJobOption();
  fAlgMgr = DmpAlgorithmManager::GetInstance();
  fSvcMgr = DmpServiceManager::GetInstance();
  fSvcMgr->Append(DmpRootIOSvc::GetInstance());     // must use GetInstance instead of global variable, since, Mac create global variable later than DmpCore::DmpCore()
  fSvcMgr->Append(DmpDataBuffer::GetInstance());
  fSeed = (time((time_t*)NULL));
  char tmptime[256];
  time_t mt = (int)fSeed;
  strftime(tmptime,256,"%F %T",gmtime(&mt));
  fJobTime = (std::string)tmptime;
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
  if(not fInitializeDone && gRootIOSvc->GetOutputRootFile()){
    std::string name =gRootIOSvc->GetJobOptTreeName();
    gDataBuffer->RegisterObject(name+"/option",fJobOpt);
  }
  fJobOpt->SetOption("Core/JobTime",fJobTime);
  fJobOpt->SetOption("Core/JobSeed",boost::lexical_cast<std::string>(fSeed));
  fJobOpt->SetOption("Core/TimeWindowStart",DmpTimeConvertor::Second2Date(fStartTime));
  fJobOpt->SetOption("Core/TimeWindowStop",DmpTimeConvertor::Second2Date(fStopTime));
  fJobOpt->SetOption("Core/MaxEventNumber",boost::lexical_cast<std::string>(fMaxEventNo));
  fJobOpt->SetJobName(this->GetSeedString()+"_"+fAlgMgr->AlgorithmFlow());
  std::cout<<"  [DmpCore::Initialize] ... initialized successfully"<<std::endl;
  fInitializeDone = true;
  return fInitializeDone;
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
  while((not fTerminateRun) && (fInTimeEvents < fMaxEventNo || fMaxEventNo < 0)){
    if(gRootIOSvc->PrepareEvent()){
      if(EventInTimeRange()){
        ++fInTimeEvents;
        if(fAlgMgr->ProcessOneEvent()){
          gRootIOSvc->FillData("Event");
        }
      }
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
  if(not fInitializeDone){
    return false;
  }
  std::cout<<"\n  [DmpCore::Finalize] Finalize..."<<std::endl;
  if(DmpLog::logLevel >= DmpLog::INFO){
    std::cout<<std::endl;
    fJobOpt->PrintOptions();
    std::cout<<std::endl;
  }
  //*
  //* Important! First, finalize algorithms, then services!
  //*
  fAlgMgr->Finalize();
  fSvcMgr->Finalize();
  std::cout<<"  [DmpCore::Finalize] ... finalized successfully"<<std::endl;
  return true;
}

//-------------------------------------------------------------------
void DmpCore::SetStartTime(const std::string &t0){
  fStartTime = DmpTimeConvertor::Date2Second(t0);
}

//-------------------------------------------------------------------
void DmpCore::SetStopTime(const std::string &t1){
  fStopTime = DmpTimeConvertor::Date2Second(t1);
}

void DmpCore::LoadFirstEvent()
{
  gRootIOSvc->PrepareFirstEvent();
}

std::string DmpCore::GetSeedString()const
{
  return boost::lexical_cast<std::string>(fSeed);
}

//-------------------------------------------------------------------
DmpCore *gCore = DmpCore::GetInstance();


