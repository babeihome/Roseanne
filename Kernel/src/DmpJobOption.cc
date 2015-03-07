/*
 *  $Id: DmpJobOption.cc, 2015-03-05 19:16:49 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 01/10/2014
*/

#include <iostream>
#include "DmpJobOption.h"
#include "DmpLog.h"

ClassImp(DmpJobOption)

//-------------------------------------------------------------------
DmpJobOption::DmpJobOption():JobName("No Alg")
{
}

//-------------------------------------------------------------------
DmpJobOption::~DmpJobOption(){
}

//-------------------------------------------------------------------
DmpJobOption& DmpJobOption::operator=(const DmpJobOption &r){
  Reset();
  Option =r.Option;
  CmdList = r.CmdList;
}

//-------------------------------------------------------------------
void DmpJobOption::LoadFrom(DmpJobOption *r){
  Reset();
  Option = r->Option;
  CmdList = r->CmdList;
}

//-------------------------------------------------------------------
void DmpJobOption::SetOption(std::string tmp,const std::string &v){
  tmp = (tmp[0]!='/')?tmp:tmp.substr(1);
  if(HasCommand(tmp)){
    if(Option[tmp] != v){
      DmpLogInfo<<"Resetting "<<tmp<<":\t\""<<Option[tmp]<<"\" ---> \""<<v<<"\""<<DmpLogEndl;
      Option[tmp] = v;
    }
  }else{
    DmpLogInfo<<"Setting "<<tmp<<":\t"<<v<<DmpLogEndl;
    Option.insert(std::make_pair(tmp,v));
    CmdList.push_back(tmp);
  }
}

//-------------------------------------------------------------------
void DmpJobOption::PrintOptions()const{
  DmpLogCout<<"Job Name:\t"<<JobName<<DmpLogEndl;
  for(std::map<std::string,std::string>::const_iterator it=Option.begin();it!=Option.end();it++){
    DmpLogCout<<"\t"<<it->first<<":\t"<<it->second<<DmpLogEndl;
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

