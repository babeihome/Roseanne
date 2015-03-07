/*
 *  $Id: DmpAlgorithmManager.cc, 2014-06-27 10:51:39 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 25/03/2014
*/

#include "DmpAlgorithmManager.h"

//-------------------------------------------------------------------
DmpAlgorithmManager::DmpAlgorithmManager()
 :DmpElementManager<DmpVAlg>("Algorithm manager")
{
}

//-------------------------------------------------------------------
DmpAlgorithmManager::~DmpAlgorithmManager(){
}

//-------------------------------------------------------------------
bool DmpAlgorithmManager::ProcessOneEvent(){
  for(std::list<DmpVAlg*>::iterator it = fElements.begin();it != fElements.end();++it){
    if(not (*it)->ProcessThisEvent()){
      return false;
    }
  }
  return true;
}

std::string DmpAlgorithmManager::AlgorithmFlow()
{
  std::string algList;
  for(std::list<DmpVAlg*>::iterator it = fElements.begin();it != fElements.end();++it){
    if(it != fElements.begin()){
      algList +="-->";
    }
    algList += (*it)->GetName();
  }
  if(algList == "") algList = "NoAlg";
  return algList;
}


