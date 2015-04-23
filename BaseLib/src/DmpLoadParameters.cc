/*
 *  $Id: DmpLoadParameters.cc, 2015-04-23 11:10:32 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 01/03/2015
*/

#include <fstream>
#include <iostream>
#include <sstream>

#include "DmpLoadParameters.h"
//#include "DmpBgoBase.h"
//#include "DmpPsdBase.h"

#ifndef LoadPedestal_CXX
#define LoadPedestal_CXX

namespace DAMPE{

bool __loadSteering(std::ifstream &f,DmpParameterSteering &s)
{
   s.clear();
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

bool LoadParameters(std::string inFName, DmpParameterHolder &pedPar, DmpParameterSteering &steering)
 {
   pedPar.clear();
   steering.clear();
   std::ifstream inPed_f(inFName.c_str());
   if(! inPed_f.good()){
     std::cout<<"ERROR:\tOpen file failed...\t"<<inFName<<std::endl;
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
   std::string bufs;
   TString bufTs;
   std::vector<double> pars;
   while(! inPed_f.eof()){
     getline(inPed_f,tmp);
     if(__isNextMarker(tmp)){
       break;
     }
     if(tmp != ""){
       std::istringstream stream(tmp);
       stream>>gid;
       pars.clear();
       while(stream >> bufs){
         bufTs = bufs;
         if(bufTs == "inf"){
           bufTs = "999";
         }else if(bufTs == "-nan"){
           bufTs = "-999";
         }
         pars.push_back(bufTs.Atof());
       }
       pedPar.insert(std::make_pair(gid,pars));
     }
   }

   inPed_f.close();
   return true;
 }

 void Check2(std::string naem)
 {
   std::map<int, std::vector<double> >  mypedPar;
   DmpParameterSteering StePar;
   std::string t0,t1;
   bool x = LoadParameters(naem,mypedPar,StePar);
   if(x){
     for(std::map<int,std::vector<double> >::iterator it = mypedPar.begin();it != mypedPar.end();++it){
             std::cout<<it->first;//<<it->second.at(0)<<"\t"<<it->second.at(1)<<std::endl;
             for(int i=0;i<it->second.size();++i){
                     std::cout<<"\t\t"<<it->second.at(i);
             }
             std::cout<<std::endl;
     }
     for(DmpParameterSteering::iterator it = StePar.begin();it != StePar.end();++it){
             std::cout<<it->first<<"\t"<<it->second<<std::endl;
     }
     std::cout<<t0<<"\t"<<t1<<std::endl;
   }
 }

/*
 void Check(std::string naem)
 {
   std::map<int, std::vector<double> >  mypedPar;
   DmpParameterSteering StePar;
   std::string t0,t1;
   bool x = false;
   if(naem.find(".ped") != std::string::npos){
     if(naem.find("Bgo") != std::string::npos){
             x = Bgo::LoadPedestal(naem,mypedPar,StePar);
     }else if(naem.find("Psd") != std::string::npos){
             x = Psd::LoadPedestal(naem,mypedPar,StePar);
     }else if(naem.find("Nud") != std::string::npos){
             x = Nud::LoadPedestal(naem,mypedPar,StePar);
     }
   }else if(naem.find("rel") != std::string::npos){
     if(naem.find("Bgo") != std::string::npos){
             x = Bgo::LoadRelation(naem,mypedPar,StePar);
     }else if(naem.find("Psd") !=  std::string::npos){
             x = Psd::LoadRelation(naem,mypedPar,StePar);
     }
   }else if(naem.find("mip") != std::string::npos){
     if(naem.find("Bgo") != std::string::npos){
             x = Bgo::LoadMip(naem,mypedPar,StePar);
     }else if(naem.find("Psd") !=  std::string::npos){
             //x = Psd::LoadMip(naem,mypedPar,StePar);
     }
   }
   if(x){
     for(std::map<int,std::vector<double> >::iterator it = mypedPar.begin();it != mypedPar.end();++it){
             std::cout<<it->first;//<<it->second.at(0)<<"\t"<<it->second.at(1)<<std::endl;
             for(int i=0;i<it->second.size();++i){
                     std::cout<<"\t\t"<<it->second.at(i);
             }
             std::cout<<std::endl;
     }
     for(DmpParameterSteering::iterator it = StePar.begin();it != StePar.end();++it){
             std::cout<<it->first<<"\t"<<it->second<<std::endl;
     }
     std::cout<<t0<<"\t"<<t1<<std::endl;
   }
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
   pedPar.clear();

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

  bool LoadRelation(std::string inFName, DmpParameterHolder &relPar, DmpParameterSteering &steering)
 {
   std::ifstream inRel_f(inFName.c_str());
   if(! inRel_f.good()){
     std::cout<<"ERROR:\tOpen calibration file failed...\t"<<inFName<<std::endl;
     inRel_f.close();
     return false;
   }
   if(not __loadSteering(inRel_f,steering)) {return false;}
   relPar.clear();

   std::string  tmp;
   while(tmp != Mark_D){
    getline(inRel_f,tmp);
    if(inRel_f.eof()){
        std::cout<<"Not find "<<Mark_D<<std::endl;
        return false;
    }
   }
   int gid=0;
   std::vector<double> p01(9,0);
   while(! inRel_f.eof()){
     getline(inRel_f,tmp);
     if(__isNextMarker(tmp)){
       break;
     }
     if(tmp != ""){
       std::istringstream stream(tmp);
       stream>>gid>>p01[0]>>p01[1]>>p01[2]>>p01[3]>>p01[4]>>p01[5]>>p01[6]>>p01[7]>>p01[8];
       relPar.insert(std::make_pair(gid,p01));
     }
   }

   inRel_f.close();
   return true;
 }
 
  bool LoadMip(std::string inFName, DmpParameterHolder &mipsPar, DmpParameterSteering &steering)
 {
   std::ifstream inRel_f(inFName.c_str());
   if(! inRel_f.good()){
     std::cout<<"ERROR:\tOpen calibration file failed...\t"<<inFName<<std::endl;
     inRel_f.close();
     return false;
   }
   if(not __loadSteering(inRel_f,steering)) {return false;}
   mipsPar.clear();

   std::string  tmp;
   while(tmp != Mark_D){
    getline(inRel_f,tmp);
    if(inRel_f.eof()){
        std::cout<<"Not find "<<Mark_D<<std::endl;
        return false;
    }
   }
   int gid=0;
   std::vector<double> pm(10,0);
   while(! inRel_f.eof()){
     // bar combined
     getline(inRel_f,tmp);
     if(__isNextMarker(tmp)){
       break;
     }
     if(tmp != ""){
       std::istringstream stream(tmp);
       stream>>gid>>pm[0]>>pm[1]>>pm[3]>>pm[4]>>pm[5]>>pm[6]>>pm[7]>>pm[8]>>pm[9];
       pm[2] = -1;
       mipsPar.insert(std::make_pair(gid,pm));
     }
     // bar side0
     getline(inRel_f,tmp);
     if(__isNextMarker(tmp)){
       break;
     }
     if(tmp != ""){
       std::istringstream stream(tmp);
       stream>>gid>>pm[0]>>pm[1]>>pm[2]>>pm[3]>>pm[4]>>pm[5]>>pm[6]>>pm[7]>>pm[8]>>pm[9];
       mipsPar.insert(std::make_pair(gid,pm));
     }
     // bar side1
     getline(inRel_f,tmp);
     if(__isNextMarker(tmp)){
       break;
     }
     if(tmp != ""){
       std::istringstream stream(tmp);
       stream>>gid>>pm[0]>>pm[1]>>pm[2]>>pm[3]>>pm[4]>>pm[5]>>pm[6]>>pm[7]>>pm[8]>>pm[9];
       mipsPar.insert(std::make_pair(gid,pm));
     }
   }

   inRel_f.close();
   return true;
 }
};

namespace Psd{
  bool LoadPedestal(std::string inFName, DmpParameterHolder &pedPar, DmpParameterSteering &steering)
  {
    return DAMPE::Bgo::LoadPedestal(inFName,pedPar,steering);
  }
  bool LoadRelation(std::string inFName, DmpParameterHolder &relPar, DmpParameterSteering &steering)
  {
    return DAMPE::Bgo::LoadRelation(inFName,relPar,steering);
  }
  bool LoadMip(std::string inFName, DmpParameterHolder &mipsPar, DmpParameterSteering &steering)
  {
    return DAMPE::Bgo::LoadMip(inFName,mipsPar,steering);
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
   pedPar.clear();

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

*/

};

#endif

