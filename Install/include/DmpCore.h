/*
 *  $Id: DmpCore.h, 2014-11-05 21:45:14 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 22/04/2014
*/

#ifndef DmpCore_H
#define DmpCore_H

#ifndef __CINT__
#include <boost/lexical_cast.hpp>
#endif

#include "DmpAlgorithmManager.h"
#include "DmpServiceManager.h"
#include "DmpJobOption.h"

#include "DmpEvtHeader.h"

class DmpCore{
/*
 *  DmpCore
 *
 *      Core of DAMPE software. It's a singleton in one job.
 *
 */
public:
  static DmpCore* GetInstance(){
    static DmpCore instance;
    return &instance;
  }
  ~DmpCore();

public:     // binding functions
  bool Initialize();            // execute all elements' Initialize() in all *Mgr
  bool Run();                   // run one job
  void TerminateRun(){fTerminateRun = true;} // call me in algorithms
  bool Finalize();              // execute all elements' Finalize() in all *Mgr

public:     // binding functions
  void SetStartTime(const std::string &t0); // YYYY-MM-DD HH:MM:SS  default value: "2013-01-01 00:00:00"
  void SetStopTime(const std::string &t1);  // YYYY-MM-DD HH:MM:SS  default value: "2053-01-01 00:00:00"
  void SetEventNumber(const long &n){fMaxEventNo = n;}
  void SetLogLevel(const std::string &v)const{DmpLog::SetLogLevel(v);}
  void SetLogHeader(const std::string &v)const{DmpLog::SetLogHeader(v);}
  DmpAlgorithmManager*  AlgorithmManager() const {return fAlgMgr;}
  DmpServiceManager*    ServiceManager() const {return fSvcMgr;}

public:
  DmpJobOption *GetJobOption(){return fJobOpt;}
  std::vector<DmpJobOption*> GetPreviousJobOpt()const;
  long GetInTimeEventID() const {return fInTimeEvents-1;}      // only events which in time
  int  GetStartTime() const {return fStartTime;}
  int  GetStopTime() const {return fStopTime;}
  long GetMaxEventNumber() const {return fMaxEventNo;}
  long GetSeed() const{return fSeed;}
  DmpEvtHeader *GetEventHeader()const{return fHeaderLv0;}
  std::string GetSeedString()const;
  std::string GetJobTime()const{return fJobTime;}
  //bool EventInTimeWindow(const int &t) const{return ((fStartTime<=t)&&(t<=fStopTime))?true:false;} // use second of event header
  bool EventInTimeRange();
  bool InitializeDone()const{return fInitializeDone;}
  DmpEvtHeader *EventHeader(){return fHeaderLv0;}    // for RDC output

public:
  int  TimeFirstInput()const{return fTimestamp[0][0];}
  int  TimeLastInput()const{return fTimestamp[0][1];}
  int  TimeFirstInTimeWindow()const{return fTimestamp[1][0];}
  int  TimeLastInTimeWindow()const{return fTimestamp[1][1];}
  int  TimeFirstOutput()const{return fTimestamp[2][0];}
  int  TimeLastOutput()const{return fTimestamp[2][1];}
  std::string GetTimeFirstInput()const;
  std::string GetTimeLastInput()const;
  std::string GetTimeFirstInTimeWindow()const;
  std::string GetTimeLastInTimeWindow()const;
  std::string GetTimeFirstOutput()const;
  std::string GetTimeLastOutput()const;

private:
  DmpCore();

private:
  std::string           fJobTime;       // running time of job
  DmpAlgorithmManager   *fAlgMgr;       // algorithm manager, singleton
  DmpServiceManager     *fSvcMgr;       // service manager, singleton
  int                   fLaunchTime;    // lauch time, 20130101-0000
  int                   fStartTime;     // unit: second. start time of time window
  int                   fStopTime;      // unit: second. stop time of time window
  long                  fSeed;          // random seed for whole job. Default value: current time
  DmpJobOption          *fJobOpt;        // all options for this job
  DmpEvtHeader          *fHeaderLv0;     //  event header level 0, has time

private:
  bool      fInitializeDone;        // default is false
  bool      fTerminateRun;          // concrete algorithm could set this value

private:
  long      fMaxEventNo;            // run how many event (in time range)
  long      fInTimeEvents;          // counter of events in time range (fStartTime ~ fStopTime)

private:
  int       fTimestamp[3][2];        // timestamp of first in time event
  /*
   *    fTimestamp[0]
   *        0:  first input event 
   *        1:  last input event
   *    fTimestamp[1]
   *        0:  first event >fStartTime
   *        1:  last event < fStopTime
   *    fTimestamp[2]
   *        0:  first output event
   *        1:  last output event
   */

};

//-------------------------------------------------------------------
extern DmpCore  *gCore;

#endif

