/*
 *  $Id: DmpJobOptLogger.cc, 2014-10-03 20:08:34 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 01/10/2014
*/

#include <iostream>
#include <time.h>   // time_t
#include "DmpJobOptLogger.h"
#include "DmpLog.h"
#include "DmpTimeConvertor.h"

ClassImp(DmpJobOptLogger)

//-------------------------------------------------------------------
DmpJobOptLogger::DmpJobOptLogger()
 :Time(time((time_t*)NULL))
{
}

//-------------------------------------------------------------------
DmpJobOptLogger::~DmpJobOptLogger(){
}

//-------------------------------------------------------------------
DmpJobOptLogger& DmpJobOptLogger::operator=(const DmpJobOptLogger &r){
  Reset();
  Time = r.Time;
  Option =r.Option;
  CmdList = r.CmdList;
}

//-------------------------------------------------------------------
void DmpJobOptLogger::LoadFrom(DmpJobOptLogger *r){
  Reset();
  Time = r->Time;
  Option = r->Option;
  CmdList = r->CmdList;
}

//-------------------------------------------------------------------
void DmpJobOptLogger::PrintJobTime(const short &level)const{
// *
// *  TODO: 
// *
  DmpLogCout<<"Time: "<<Time<<"\t"<<DmpTimeConvertor::Second2Date(Time)<<DmpLogEndl;
}

//-------------------------------------------------------------------
void DmpJobOptLogger::SetOption(std::string tmp,const std::string &v){
  tmp = (tmp[0]!='/')?tmp:tmp.substr(1);
  if(HasCommand(tmp)){
    DmpLogWarning<<"Resetting "<<tmp<<":\t\""<<Option[tmp]<<"\" ---> \""<<v<<"\""<<DmpLogEndl;
    Option[tmp] = v;
  }else{
    DmpLogInfo<<"Setting "<<tmp<<":\t"<<v<<DmpLogEndl;
    Option.insert(std::make_pair(tmp,v));
    CmdList.push_back(tmp);
  }
}

//-------------------------------------------------------------------
void DmpJobOptLogger::ListOptions()const{
  for(std::map<std::string,std::string>::const_iterator it=Option.begin();it!=Option.end();it++){
    DmpLogCout<<it->first<<":\t"<<it->second<<DmpLogEndl;
  }
}

//-------------------------------------------------------------------
bool DmpJobOptLogger::HasCommand(std::string o)const{
  o = (o[0]!='/') ? o : o.substr(1);
  bool status = (Option.find(o) != Option.end()) ? true : false;
  if(not status){
    for(size_t i=0;i<CmdList.size();++i){
      if(CmdList[i].find(o) != std::string::npos){
        return true;
      }
    }
  }
  return status;
}

//-------------------------------------------------------------------
std::string DmpJobOptLogger::GetValue(const std::string &tmp)const{
  if(HasCommand(tmp)){
    return Option.at(tmp);
  }
  return "";
}

//-------------------------------------------------------------------
std::string DmpJobOptLogger::GetValue(const short &i)const{
  return Option.at(CmdList.at(i));
}

//-------------------------------------------------------------------
void DmpJobOptLogger::Reset(){
  Option.clear();
  CmdList.clear();
}

