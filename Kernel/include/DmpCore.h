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
  bool Finalize();              // execute all elements' Finalize() in all *Mgr

public:     // binding functions
  void SetLogLevel(const std::string &v)const{DmpLog::SetLogLevel(v);}
  void SetLogHeader(const std::string &v)const{DmpLog::SetLogHeader(v);}
  void SetEventNumber(const long &n);
  void SetStartTime(const std::string &t0);
  void SetStopTime(const std::string &t1);
  void StartFromEvent(const long &i){fCurrentEventID = i;}
  DmpAlgorithmManager*  AlgorithmManager() const {return fAlgMgr;}
  DmpServiceManager*    ServiceManager() const {return fSvcMgr;}

public:
  const long& GetMaxEventNumber() const {return fMaxEventNo;}
  const long& GetCurrentEventID() const {return fCurrentEventID;}
  bool EventInTimeWindow(const int &t) const{return ((fStartTime<=t)&&(t<=fStopTime))?true:false;} // use second of event header
  const bool& InitializeDone() const {return fInitializeDone;}
  const int&  GetStartTime() const {return fStartTime;}
  const int&  GetStopTime() const {return fStopTime;}
  long GetSeed() const;

public:
  void TerminateRun()   {fTerminateRun = true;} // call me in algorithms

private:
  DmpCore();

private:
  DmpAlgorithmManager   *fAlgMgr;       // algorithm manager, singleton
  DmpServiceManager     *fSvcMgr;       // service manager, singleton
  int                   fLaunchTime;    // lauch time, 20130101-0000
  long                  fMaxEventNo;    // run how many event
  int                   fStartTime;     // unit: second. start time of time window
  int                   fStopTime;      // unit: second. stop time of time window

private:
  bool      fInitializeDone;        // default is false
  bool      fTerminateRun;          // concrete algorithm could set this value

private:
  long      fCurrentEventID;        // the current event ID (processing)

};

//-------------------------------------------------------------------
extern DmpCore  *gCore;

#endif

