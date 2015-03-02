/*
 *  $Id: DmpJobOption.cc, 2015-01-27 10:56:00 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 01/10/2014
*/

#include <iostream>
#include <time.h>   // time_t
#include "DmpJobOption.h"
#include "DmpLog.h"
#include "DmpTimeConvertor.h"

ClassImp(DmpJobOption)

//-------------------------------------------------------------------
DmpJobOption::DmpJobOption()
 :Time(time((time_t*)NULL))
{
}

//-------------------------------------------------------------------
DmpJobOption::~DmpJobOption(){
}

//-------------------------------------------------------------------
DmpJobOption& DmpJobOption::operator=(const DmpJobOption &r){
  Reset();
  Time = r.Time;
  Option =r.Option;
  CmdList = r.CmdList;
}

//-------------------------------------------------------------------
void DmpJobOption::LoadFrom(DmpJobOption *r){
  Reset();
  Time = r->Time;
  Option = r->Option;
  CmdList = r->CmdList;
}

//-------------------------------------------------------------------
std::string DmpJobOption::PrintJobTime(const short &level)const{
// *
// *  TODO: 
// *
  return DmpTimeConvertor::Second2Date(Time);
  //DmpLogCout<<"Time: "<<Time<<"\t"<<DmpTimeConvertor::Second2Date(Time)<<DmpLogEndl;
}

//-------------------------------------------------------------------
void DmpJobOption::SetOption(std::string tmp,const std::string &v){
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
void DmpJobOption::ListOptions()const{
  for(std::map<std::string,std::string>::const_iterator it=Option.begin();it!=Option.end();it++){
    DmpLogCout<<it->first<<":\t"<<it->second<<DmpLogEndl;
  }
}

//-------------------------------------------------------------------
bool DmpJobOption::HasCommand(std::string o)const{
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
std::string DmpJobOption::GetValue(const std::string &tmp)const{
  if(HasCommand(tmp)){
    return Option.at(tmp);
  }
  return "";
}

//-------------------------------------------------------------------
std::string DmpJobOption::GetValue(const short &i)const{
  return Option.at(CmdList.at(i));
}

//-------------------------------------------------------------------
void DmpJobOption::Reset(){
  Option.clear();
  CmdList.clear();
}

