/*
 *  $Id: DmpDataBuffer.cc, 2015-01-27 11:48:33 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 20/07/2014
*/

#include <boost/algorithm/string.hpp>
#include "DmpDataBuffer.h"

//-------------------------------------------------------------------
DmpDataBuffer::DmpDataBuffer()
 :DmpVSvc("DmpDataBuffer")
{
}

//-------------------------------------------------------------------
DmpDataBuffer::~DmpDataBuffer(){
}

//-------------------------------------------------------------------
bool DmpDataBuffer::Initialize(){
  return true;
}

//-------------------------------------------------------------------
bool DmpDataBuffer::Finalize(){
  DmpLogInfo<<"+-Deleting output data..."<<DmpLogEndl;
  for(DmpDataBufFolderMap::iterator aFolder=fDataBufPool.begin();aFolder!=fDataBufPool.end();++aFolder){
    DmpDataBufTreeMap aTreeMap = aFolder->second;
    DmpLogInfo<<"| |-"<<aFolder->first<<std::endl;
    for(DmpDataBufTreeMap::iterator aTree=aTreeMap.begin();aTree!=aTreeMap.end();++aTree){
      DmpDataBufBranchMap aBranchMap = aTree->second;
    DmpLogInfo<<"| | |-"<<aTree->first<<std::endl;
      for(DmpDataBufBranchMap::iterator it=aBranchMap.begin();it!=aBranchMap.end();++it){
    DmpLogInfo<<"| | | |-"<<it->first<<std::endl;
        delete it->second;
      }
      aBranchMap.clear();
    }
    aTreeMap.clear();
  }
  fDataBufPool.clear();
  DmpLogInfo<<"`-Done"<<DmpLogEndl;
  return true;
}

//-------------------------------------------------------------------
TObject* DmpDataBuffer::ReadObject(const std::string &path){
  std::vector<std::string> p  = PathCheck(path);
  return ReadObject(p);
}

TObject* DmpDataBuffer::ReadObject(std::vector<std::string> temp)
{
  TObject *ptr = 0;
  if(fDataBufPool.find(temp[0]) != fDataBufPool.end()){
    if(fDataBufPool[temp[0]].find(temp[1]) != fDataBufPool[temp[0]].end()){
      if(fDataBufPool[temp[0]][temp[1]].find(temp[2]) != fDataBufPool[temp[0]][temp[1]].end()){
        ptr = fDataBufPool[temp[0]][temp[1]][temp[2]];
      }
    }
  }
  return ptr;
}

//-------------------------------------------------------------------
std::vector<std::string> DmpDataBuffer::PathCheck(std::string checkMe)const
{
  std::vector<std::string> temp;
  boost::split(temp,checkMe,boost::is_any_of("/"));
  if(3 != temp.size()){
    DmpLogError<<"Wrong path: "<<checkMe<<DmpLogEndl;
    throw;
  }
  return temp;
}

//-------------------------------------------------------------------
DmpDataBuffer *gDataBuffer = DmpDataBuffer::GetInstance();


