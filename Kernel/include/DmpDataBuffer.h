/*
 *  $Id: DmpDataBuffer.h, 2015-01-27 11:48:31 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 20/07/2014
*/

#ifndef DmpDataBuffer_H
#define DmpDataBuffer_H

#include "TTree.h"
#include "DmpRootIOSvc.h"

class TObject;

class DmpDataBuffer : public DmpVSvc{
/*
 *  DmpDataBuffer
 *
 *      it's a singleton for eanch job
 *      it manages all data pointers.
 */
public:
  static DmpDataBuffer* GetInstance(){
    static DmpDataBuffer instance;
    return &instance;
  }
  ~DmpDataBuffer();
  bool Initialize();
  bool Finalize();
  TObject* ReadObject(const std::string &path);  // if path/dataPtr not in any data buffer, return 0
  template<typename T> void RegisterObject(const std::string &path,T *&dataPtr);   // path = Folder/Tree/Branch
  template<typename T> void LinkRootFile(const std::string &path,T *&dataPtr);  // use dataPtr to read a branch from input rootfile, if not find branch, break job;
  TObject* ReadObject(std::vector<std::string> p);

private:
  DmpDataBuffer();
  std::vector<std::string> PathCheck(std::string checkMe)const;

private:
typedef std::map<std::string, TObject*>                 DmpDataBufBranchMap;    // key is "Branch"
typedef std::map<std::string, DmpDataBufBranchMap>      DmpDataBufTreeMap;      // key is "Tree"
typedef std::map<std::string, DmpDataBufTreeMap>        DmpDataBufFolderMap;    // key is "Folder"

  DmpDataBufFolderMap    fDataBufPool;           // new created data pointers in pool. 3-level path: Folder, Tree, Branch

private:
  std::vector<TObject*>     fDataList;          // for delete
  void __pushIntoList(TObject *r);

};

//-------------------------------------------------------------------
template<typename T> void DmpDataBuffer::RegisterObject(const std::string &path,T *&dataPtr){
  // path check
  std::vector<std::string> pp = PathCheck(path);
  if(not ReadObject(pp)){
    fDataBufPool[pp[0]][pp[1]][pp[2]] = dataPtr;
  }
  // input arguments into root IOSvc, to create a branch (IOSvc check whether need to create branch)
  if(gRootIOSvc->WriteValid(pp[0]+"/"+pp[1])){
    TTree *tree = gRootIOSvc->GetOutputTree(pp[0]+"/"+pp[1]);
    if(0 == tree->GetListOfBranches()->FindObject(pp[2].c_str())){
      if("TClonesArray" == dataPtr->GetName()){
        tree->Branch(pp[2].c_str(),dataPtr,32000,2);
      }else{
        tree->Branch(pp[2].c_str(),dataPtr->GetName(),&dataPtr,32000,2);
      }
    }else{
      DmpLogError<<path<<" is existes in output tree"<<DmpLogEndl;
      throw;
    }
  }

  __pushIntoList(dataPtr);
}

//-------------------------------------------------------------------
template<typename T> void DmpDataBuffer::LinkRootFile(const std::string &path,T *&dataPtr){
  // check path
  std::vector<std::string> p = PathCheck(path);
  if(ReadObject(p)){
    DmpLogError<<path<<" is existing in data buffer"<<DmpLogEndl;
    throw;
  }
  TTree *findTree = gRootIOSvc->GetInputTree(p[0]+"/"+p[1]);
  if(findTree){
    findTree->SetBranchAddress(p[2].c_str(),&dataPtr);
    fDataBufPool[p[0]][p[1]].insert(std::make_pair(p[2],dataPtr));
  }else{
    DmpLogError<<"[DmpDataBuffer::LinkRootFile] not find "<<path<<" in "<<gRootIOSvc->GetInputFileName()<<DmpLogEndl;
    throw;
  }

  __pushIntoList(dataPtr);
}

//-------------------------------------------------------------------
extern DmpDataBuffer  *gDataBuffer;

#endif


