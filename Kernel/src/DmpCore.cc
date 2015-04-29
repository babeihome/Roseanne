/*
 *  $Id: DmpCore.cc, 2014-09-21 14:58:08 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 22/04/2014
*/

//#include <time.h>
#include <sys/time.h>

#include "DmpCore.h"
#include "DmpRootIOSvc.h"
#include "DmpDataBuffer.h"
#include "DmpSvcDatabase.h"
#include "DmpTimeConvertor.h"

#define EVTHEADERL0  "Event/Header/Level0"

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
  fHeaderLv0(0),
  fMaxEventNo(-1),      // default < 0, for all events
  fInTimeEvents(0)      // must == 0
{
  std::cout<<"**************************************************"<<std::endl;
  std::cout<<"      Offline software of DAMPE (DMPSW)"<<std::endl;
  std::cout<<"      version:  2.2.0(dampeustc)"<<std::endl;
  std::cout<<"**************************************************"<<std::endl;
  fJobOpt = new DmpJobOption();
  fAlgMgr = DmpAlgorithmManager::GetInstance();
  fSvcMgr = DmpServiceManager::GetInstance();
  fSvcMgr->Append(DmpRootIOSvc::GetInstance());     // must use GetInstance instead of global variable, since, Mac create global variable later than DmpCore::DmpCore()
  fSvcMgr->Append(DmpDataBuffer::GetInstance());
  fSvcMgr->Append(DmpSvcDatabase::GetInstance());     // must use GetInstance instead of global variable, since, Mac create global variable later than DmpCore::DmpCore()
  gRootIOSvc->AppendWriteList("Event/Header");
  gRootIOSvc->AppendWriteList("Metadata/Job0"); // default job id is 0

  struct timeval cu;
  gettimeofday(&cu,NULL);
  fSeed = cu.tv_sec*1000000 + cu.tv_usec;
  time_t mt = time(0);
  char tmptime[256];
  strftime(tmptime,256,"%F %T",gmtime(&mt));
  fJobTime = (std::string)tmptime;
  for(int i=0;i<3;++i){
    for(int j=0;j<2;++j){
      fTimestamp[i][j] = 0;
    }
  }
}

//-------------------------------------------------------------------
DmpCore::~DmpCore(){
}

//-------------------------------------------------------------------
bool DmpCore::Initialize(){
  //*
  //* Important! First, initialize servises, then event header, then algorithms, then job options
  //*
  std::cout<<"\n  [DmpCore::Initialize] Initializing..."<<std::endl;
  if(not fSvcMgr->Initialize()) return false;
  fHeaderLv0 = dynamic_cast<DmpEvtHeader*>(gDataBuffer->ReadObject(EVTHEADERL0));
  if(fHeaderLv0 == 0){
    fHeaderLv0 = new DmpEvtHeader();
    if(gRootIOSvc->GetInputRootFile()){
      gDataBuffer->LinkRootFile(EVTHEADERL0,fHeaderLv0);
    }
  }
  gDataBuffer->RegisterObject(EVTHEADERL0,fHeaderLv0);
  if(not fAlgMgr->Initialize()) return false;
  if(not fInitializeDone && gRootIOSvc->GetOutputRootFile()){
    std::string name =gRootIOSvc->GetJobOptTreeName("Metadata");
    gDataBuffer->RegisterObject(name+"/option",fJobOpt);
  }
  fJobOpt->SetOption("Core/JobTime",fJobTime);
  fJobOpt->SetOption("Core/JobSeed",boost::lexical_cast<std::string>(fSeed));
  fJobOpt->SetOption("Core/TimeWindowStart",DmpTimeConvertor::Second2Date(fStartTime));
  fJobOpt->SetOption("Core/TimeWindowStop",DmpTimeConvertor::Second2Date(fStopTime));
  fJobOpt->SetOption("Core/MaxEventNumber",boost::lexical_cast<std::string>(fMaxEventNo));
  fJobOpt->SetJobName(fAlgMgr->Sequence());
  std::cout<<"  [DmpCore::Initialize] ... Done"<<std::endl;
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
  gRootIOSvc->PrepareFirstEvent();
  fTimestamp[0][0] = fHeaderLv0->GetSecond();
  while((not fTerminateRun) && (fInTimeEvents < fMaxEventNo || fMaxEventNo < 0)){
    if(gRootIOSvc->PrepareEvent()){
      DmpLogDebug<<"input event ID = "<<gRootIOSvc->GetInputEventID()<<"\tin time event ID = "<<fInTimeEvents<<"\t output event ID = "<<gRootIOSvc->GetOutputEventID()<<std::endl;
      if(EventInTimeRange()){
        ++fInTimeEvents;
        if(fAlgMgr->ProcessOneEvent()){
          if(fTimestamp[2][0] == 0){fTimestamp[2][0] = fHeaderLv0->GetSecond();}
          fTimestamp[2][1] = fHeaderLv0->GetSecond();
          gRootIOSvc->FillData("Event");
        }
      }
    }else{
      fTerminateRun = true;
      break;
    }
  }
  fTimestamp[0][1] = fHeaderLv0->GetSecond();
  std::cout<<"  [DmpCore::Run] Done"<<std::endl;
  return true;
}

//-------------------------------------------------------------------
bool DmpCore::Finalize(){
  if(not fInitializeDone){
    return false;
  }
  std::cout<<"\n  [DmpCore::Finalize] Finalizing..."<<std::endl;
  if(DmpLog::logLevel >= DmpLog::INFO){
    std::cout<<std::endl;
    fJobOpt->PrintOptions();
    std::cout<<std::endl;
    std::cout<<"Input event:         "<<GetTimeFirstInput()<<"\t"<<GetTimeLastInput()<<std::endl;
    std::cout<<"In time range event: "<<GetTimeFirstInTimeWindow()<<"\t"<<GetTimeLastInTimeWindow()<<std::endl;
    std::cout<<"Output event:        "<<GetTimeFirstOutput()<<"\t"<<GetTimeLastOutput()<<std::endl;
    std::cout<<std::endl;
  }
  //*
  //* Important! First, finalize algorithms, then services!
  //*
  fAlgMgr->Finalize();
  fSvcMgr->Finalize();
  fHeaderLv0 = 0;
  std::cout<<"  [DmpCore::Finalize] ... Done\n\n"<<std::endl;
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

std::string DmpCore::GetSeedString()const
{
  return boost::lexical_cast<std::string>(fSeed);
}

std::vector<DmpJobOption*> DmpCore::GetPreviousJobOpt()const
{
  static std::vector<DmpJobOption*>  allPre;
// *
// *  TODO: 
// *
  /*
  if(allPre.size()) return allPre;
  if(fInRootFile != 0){
    return allPre;
  }else{    // get previous options
    //    check size,
    int nd = gRootIOSvc->GetJobOptSize();
    for(int i=0;i<nd;++i){
      TString treeName = "Metadata/Job";
      treeName += nd;
      TString path = treeName + "/Option";
      DmpJobOption *tmp = new DmpJobOption();
      gDataBuffer->LinkRootFile((std::string)path,tmp);
      gRootIOSvc->GetInputTree((std::string)treeName)->GetEntry(0);
      allPre.push_back(tmp);
    }
  }
  */
  return allPre;
}

bool DmpCore::EventInTimeRange()
{
  int tc = fHeaderLv0->GetSecond();
  if((fStartTime <= tc && tc <= fStopTime) || tc <= 0)
  {
    if(fTimestamp[1][0] == 0) {fTimestamp[1][0] = fHeaderLv0->GetSecond();}
    fTimestamp[1][1] = fHeaderLv0->GetSecond();
    return true;
  }
  return false;
}

std::string DmpCore::GetTimeFirstInput()const
{
  return DmpTimeConvertor::Second2Date(fTimestamp[0][0]);
}

std::string DmpCore::GetTimeLastInput()const
{
  return DmpTimeConvertor::Second2Date(fTimestamp[0][1]);
}

std::string DmpCore::GetTimeFirstInTimeWindow()const
{
  return DmpTimeConvertor::Second2Date(fTimestamp[1][0]);
}

std::string DmpCore::GetTimeLastInTimeWindow()const
{
  return DmpTimeConvertor::Second2Date(fTimestamp[1][1]);
}

std::string DmpCore::GetTimeFirstOutput()const
{
  return DmpTimeConvertor::Second2Date(fTimestamp[2][0]);
}

std::string DmpCore::GetTimeLastOutput()const
{
  return DmpTimeConvertor::Second2Date(fTimestamp[2][1]);
}

//-------------------------------------------------------------------
DmpCore *gCore = DmpCore::GetInstance();


