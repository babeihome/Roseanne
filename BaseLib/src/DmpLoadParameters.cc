/*
 *  $Id: DmpLoadParameters.cc, 2015-03-02 09:13:07 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 01/03/2015
*/

#include <fstream>
#include <iostream>
#include <sstream>

#include "DmpLoadParameters.h"

#ifndef LoadPedestal_CXX
#define LoadPedestal_CXX

namespace DAMPE{

bool __loadSteering(std::ifstream &f,DmpParameterSteering &s)
{
   f.seekg(0,f.beg);
   std::string  tmp;
   while(tmp != Mark_S){
    getline(f,tmp);
    if(f.eof()){
        std::cout<<"Not find "<<Mark_S<<std::endl;
        return false;
    }
   }
   while(not f.eof()){
     getline(f,tmp);
     if(__isNextMarker(tmp)){
       break;
     }
     std::size_t found = tmp.find("=");
     if(found != std::string::npos){
       s.insert(std::make_pair(tmp.substr(0,found),tmp.substr(found+1)));
     }
   }
   f.seekg(0,f.beg);
   return true;
}

bool __isNextMarker(std::string v)
{
  if((v.find("[") != std::string::npos) && (v.find("]") != std::string::npos)){
    return true;
  }
  return false;
}

namespace Bgo{
  bool LoadPedestal(std::string inFName, DmpParameterHolder &pedPar, DmpParameterSteering &steering)
 {
   std::ifstream inPed_f(inFName.c_str());
   if(! inPed_f.good()){
     std::cout<<"ERROR:\tOpen calibration file failed...\t"<<inFName<<std::endl;
     inPed_f.close();
     return false;
   }
   if(not __loadSteering(inPed_f,steering)) {return false;}

   std::string  tmp;
   while(tmp != Mark_D){
    getline(inPed_f,tmp);
    if(inPed_f.eof()){
        std::cout<<"Not find "<<Mark_D<<std::endl;
        return false;
    }
   }
   int gid=0;
   std::vector<double> mean_sig(2,0);
   while(! inPed_f.eof()){
     getline(inPed_f,tmp);
     if(__isNextMarker(tmp)){
       break;
     }
     if(tmp != ""){
       std::istringstream stream(tmp);
       stream>>gid;
       stream>>mean_sig[0]>>mean_sig[0]>>mean_sig[0]>>mean_sig[0]>>mean_sig[0];
       stream>>mean_sig[1];
       pedPar.insert(std::make_pair(gid,mean_sig));
     }
   }

   inPed_f.close();
   return true;
 }
};

namespace Psd{
  bool LoadPedestal(std::string inFName, DmpParameterHolder &pedPar, DmpParameterSteering &steering)
  {
    return DAMPE::Bgo::LoadPedestal(inFName,pedPar,steering);
  }
}

namespace Nud{
  bool LoadPedestal(std::string inFName, DmpParameterHolder &pedPar, DmpParameterSteering &steering)
 {
   std::ifstream inPed_f(inFName.c_str());
   if(! inPed_f.good()){
     std::cout<<"ERROR:\tOpen calibration file failed...\t"<<inFName<<std::endl;
     inPed_f.close();
     return false;
   }
   if(not __loadSteering(inPed_f,steering)) {return false;}

   std::string  tmp;
   while(tmp != Mark_D){
    getline(inPed_f,tmp);
    if(inPed_f.eof()){
        std::cout<<"Not find "<<Mark_D<<std::endl;
        return false;
    }
   }
   int gid=0;
   std::vector<double> mean_sig(2,0);
   while(! inPed_f.eof()){
     getline(inPed_f,tmp);
     if(__isNextMarker(tmp)){
       break;
     }
     if(tmp != ""){
       std::istringstream stream(tmp);
       stream>>gid;
       stream>>mean_sig[0]>>mean_sig[1];
       pedPar.insert(std::make_pair(gid,mean_sig));
     }
   }

   inPed_f.close();
   return true;
 }
}

namespace Stk{
  bool LoadPedestal(std::string inFName, DmpParameterHolder &pedPar, DmpParameterSteering &steering)
 {
   return true;
 }
}

 void CheckPed(std::string naem, std::string ID)
 {
   std::map<int, std::vector<double> >  mypedPar;
   DmpParameterSteering StePar;
   std::string t0,t1;
   bool x = false;
   if(ID == "Bgo"){
           x = Bgo::LoadPedestal(naem,mypedPar,StePar);
   }else if(ID == "Psd"){
           x = Psd::LoadPedestal(naem,mypedPar,StePar);
   }else if(ID == "Nud"){
           x = Nud::LoadPedestal(naem,mypedPar,StePar);
   }
   if(x){
     for(std::map<int,std::vector<double> >::iterator it = mypedPar.begin();it != mypedPar.end();++it){
             std::cout<<it->first<<"\t"<<it->second.at(0)<<"\t"<<it->second.at(1)<<std::endl;
     }
     for(DmpParameterSteering::iterator it = StePar.begin();it != StePar.end();++it){
             std::cout<<it->first<<"\t"<<it->second<<std::endl;
     }
     std::cout<<t0<<"\t"<<t1<<std::endl;
   }
 }

};

#endif

