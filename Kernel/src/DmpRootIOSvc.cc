/*
 *  $Id: DmpRootIOSvc.cc, 2014-10-06 14:55:57 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 20/05/2014
*/

//#ifndef __CINT__
//#include <boost/lexical_cast.hpp>
//#endif

#include <boost/algorithm/string.hpp>
#include "TTree.h"
#include "TFile.h"
#include "TDirectory.h"

#include "DmpRootIOSvc.h"
#include "DmpDataBuffer.h"
#include "DmpCore.h"

//-------------------------------------------------------------------
DmpRootIOSvc::DmpRootIOSvc()
 :DmpVSvc("DmpRootIOSvc"),
  fInRootFile(0),
  fOutRootFile(0),
  fOutFileKey(""),
  fInPath("./"),
  fOutPath("./"),
  fFirstInputEntry(0),  // must == 0
  fCurrentInputEntry(-1), // must == -1
  fEntriesOfTree(-1),    // must < 0
  fWritingEvtID(0)       // must == 0
{
  fInFileName = "";
  fOutFileName = "";
}

//-------------------------------------------------------------------
DmpRootIOSvc::~DmpRootIOSvc(){
}

//-------------------------------------------------------------------
void DmpRootIOSvc::SetInputPath(const std::string &argv)
{
  if('/' == argv[argv.length()-1]){
    fInPath = argv;
  }else{
    fInPath = argv+"/";
  }
}

//-------------------------------------------------------------------
void DmpRootIOSvc::SetInputFile(const std::string &v)
{
  boost::filesystem::path temp(v);
  fInFileName = temp.filename().string();
  if(temp.parent_path().string()!=""){
    fInPath = temp.parent_path().string()+"/";
  }
}

//-------------------------------------------------------------------
void DmpRootIOSvc::SetOutputPath(const std::string &argv)
{
  if('/' == argv[argv.length()-1]){
    fOutPath = argv;
  }else{
    fOutPath = argv+"/";
  }
}

//-------------------------------------------------------------------
void DmpRootIOSvc::SetOutputFile(const std::string &v)
{
  boost::filesystem::path temp(v);
  fOutFileName = temp.filename().string();
  if((temp.parent_path().string()!="") && (fOutPath=="./")){
    fOutPath = temp.parent_path().string()+"/";
  }
}

//-------------------------------------------------------------------
void DmpRootIOSvc::SetWriteList(std::string argv)
{
  fWriteList.erase(fWriteList.begin()+2,fWriteList.end());  // first 2 were defined in DmpCore. keep them anyway
  this->AddWriteList(argv);
}

void DmpRootIOSvc::AddWriteList(std::string argv)
{
  std::vector<std::string> tmpList;
  boost::split(tmpList,argv,boost::is_any_of(";"));
  for(short i=0;i<tmpList.size();++i){
      if(find(fWriteList.begin(),fWriteList.end(),tmpList[i]) == fWriteList.end()){
        fWriteList.push_back(tmpList[i]);
      }
  }
}

//-------------------------------------------------------------------
void DmpRootIOSvc::SetFirstInputEvent(long i)
{
  if(i<0){
    DmpLogWarning<<"First event ID must >= 0..."<<DmpLogEndl;
  }else{
    fFirstInputEntry = i;
  }
}

//-------------------------------------------------------------------
bool DmpRootIOSvc::Initialize()
{
  DmpLogDebug<<"initialization... "<<DmpLogEndl;
  fCurrentInputEntry = fFirstInputEntry;
  //-------------------------------------------------------------------
  std::string wlist;
  for(int i=0;i<fWriteList.size();++i){
    std::vector<std::string>  temp;
    boost::split(temp,fWriteList.at(i),boost::is_any_of("/"));
    if(2 != temp.size()){
      DmpLogError<<"Format(folder/tree). setting writeList \":\t"<<fWriteList.at(i)<<DmpLogEndl;
      return false;
    }
    if(i!=0){
     wlist += ";";
    }
    wlist += fWriteList.at(i);
  }
  gCore->GetJobOption()->SetOption("IO/WriteList",wlist);
  gCore->GetJobOption()->SetOption("IO/InputFile",fInFileName.string());
  gCore->GetJobOption()->SetOption("IO/OutputFile",fOutFileName.string());
  gCore->GetJobOption()->SetOption("IO/FirstInputEventID",boost::lexical_cast<std::string>(gRootIOSvc->GetFirstInputEntryID()));  // SetOption in the end of Initialize, not in SetXXXX() !!!
  //-------------------------------------------------------------------
  if(fOutFileName.string() == ""){  // save output into input root file
    if(fInFileName.string() == ""){
      this->SetOutputFile("DmpData_T"+gCore->GetSeedString()+".root");
    }else{
      if(fInFileName.extension().string() == ".root"){
        if(fOutFileKey == ""){
          fInFileName = fInPath+GetInputStem()+".root";
          fInRootFile = TFile::Open(fInFileName.string().c_str(),"UPDATE");
          if(fInRootFile == 0){
            return false;
          }
          fOutRootFile = fInRootFile;     // check flag for output file
          fOutFileName = fInFileName;
        }else{
          this->SetOutputFile(GetInputStem()+".root");
        }
      }else{
        if(fInFileName.extension().string() == ".frd"){
          this->SetOutputFile(GetInputStem()+".root");
        }else{
          DmpLogError<<" input is not a root file...\t"<<fInFileName.string()<<DmpLogEndl;
          return false;
        }
      }
    }
  }
  if(fOutRootFile == 0){    // create new output, only read input
    if(fInFileName.extension().string() == ".root"){
      fInFileName = fInPath+GetInputStem()+".root";
      fInRootFile = TFile::Open(fInFileName.string().c_str(),"READ");
      if(fInRootFile == 0){
        return false;
      }
    }
    if(fWriteList.size()){
      if(not boost::filesystem::exists(fOutPath)){
        boost::filesystem::create_directories(fOutPath);
      }
      std::string splitMark = "__";
      std::string output = fOutPath+fOutFileName.stem().string();
      unsigned found = output.find_last_of(splitMark);
      if("" != fOutFileKey){
        fOutFileKey = splitMark+fOutFileKey;
      }
      if(-1 == found){    // not found
        fOutFileName = output+fOutFileKey+".root";
      }else{
        fOutFileName = output.substr(0,found)+fOutFileKey+".root";
      }
      fOutRootFile = new TFile(fOutFileName.string().c_str(),"RECREATE");
    }
  }
  //-------------------------------------------------------------------
  DmpLogDebug<<"... initialization done "<<DmpLogEndl;
  return true;
}

//-------------------------------------------------------------------
bool DmpRootIOSvc::Finalize(){
  // save trees
  if(fOutRootFile){
    DmpLogInfo<<"+-Writing "<<fOutFileName<<DmpLogEndl;
    for(DmpRootIOFolderMap::iterator aFolderMap=fOutTreeSet.begin();aFolderMap != fOutTreeSet.end();++aFolderMap){
      DmpRootIOTreeMap aTreeMap = aFolderMap->second;
      DmpLogInfo<<"| |-"<<aFolderMap->first<<DmpLogEndl;
      fOutRootFile->cd((aFolderMap->first).c_str());
      if(aFolderMap->first != "Event"){
        FillData(aFolderMap->first);
      }
      for(DmpRootIOTreeMap::iterator it= aTreeMap.begin();it!=aTreeMap.end();++it){
        DmpLogInfo<<"| | |-"<<it->first<<", entries = "<<it->second->GetEntries()<<DmpLogEndl;
        it->second->Write();
        delete it->second;
      }
    }
    DmpLogInfo<<"`-Done"<<DmpLogEndl;
  }
  // delete root files
  if(fInRootFile){
    fInRootFile->Close();
    delete fInRootFile;
  }
  if(fOutRootFile && (fOutRootFile != fInRootFile)){
    fOutRootFile->Close();
    delete fOutRootFile;
  }
  return true;
}

//-------------------------------------------------------------------
bool DmpRootIOSvc::WriteValid(const std::string &treeName){
  if(find(fWriteList.begin(),fWriteList.end(),treeName) != fWriteList.end()){return true;}
        /*
  for(short i=0;i<fWriteList.size();++i){
    if(treeName == fWriteList[i]){
      return true;
    }
  }
  */
  return false;
}

//-------------------------------------------------------------------
TTree* DmpRootIOSvc::GetOutputTree(std::string folder_treeName){
  TTree *tree = 0;
  std::vector<std::string>  temp;
  boost::split(temp,folder_treeName,boost::is_any_of("/"));
  if(2 != temp.size()){
    DmpLogError<<"Format should be folder/tree. Setting:\t"<<folder_treeName<<DmpLogEndl;
    gCore->TerminateRun();
    return tree;
  }
  std::string folderName = temp[0];
  std::string treeName = temp[1];
  if(fOutTreeSet.find(folderName) != fOutTreeSet.end()){
    if(fOutTreeSet[folderName].find(treeName) != fOutTreeSet[folderName].end()){
      tree = fOutTreeSet[folderName][treeName];
    }
  }else{
    DmpRootIOTreeMap aNewFolder;
    fOutTreeSet.insert(std::make_pair(folderName,aNewFolder));
    if(not fOutRootFile->GetDirectory(folderName.c_str())){
      fOutRootFile->mkdir(folderName.c_str());
    }
  }
  if(0 == tree){
    tree = new TTree(treeName.c_str(),treeName.c_str());
    tree->SetAutoSave(50000000);    //50MBytes
    fOutTreeSet[folderName].insert(std::make_pair(treeName,tree));
  }
  return tree;
}

//-------------------------------------------------------------------
TTree* DmpRootIOSvc::GetInputTree(std::string folder_treeName){
  TTree *theTree = 0;
  std::vector<std::string>  temp;
  boost::split(temp,folder_treeName,boost::is_any_of("/"));
  if(2 != temp.size()){
    DmpLogError<<"Format should be folder/tree. Setting:\t"<<folder_treeName<<DmpLogEndl;
    gCore->TerminateRun();
    return theTree;
  }
  std::string folderName = temp[0];
  std::string treeName = temp[1];
  if(fInTreeSet.find(folderName) != fInTreeSet.end()){
    if(fInTreeSet[folderName].find(treeName) != fInTreeSet[folderName].end()){
      theTree = fInTreeSet[folderName][treeName];
    }
  }else{
    DmpRootIOTreeMap  aNewFolder;
    fInTreeSet.insert(std::make_pair(folderName,aNewFolder));
  }
  if(0 == theTree){
    std::string foldrAndTreeName = folderName+"/"+treeName;
    theTree = dynamic_cast<TTree*>(fInRootFile->Get(foldrAndTreeName.c_str()));
    if(theTree){
      if(fEntriesOfTree != theTree->GetEntries()){
        if(fEntriesOfTree < 0){
          fEntriesOfTree = theTree->GetEntries();
          if(fFirstInputEntry >= fEntriesOfTree){
            DmpLogError<<"First event ID: "<<fFirstInputEntry<<"\t entries of "<<foldrAndTreeName<<": "<<fEntriesOfTree<<DmpLogEndl;
            gCore->TerminateRun();
            return theTree;
          }
        }else{
          DmpLogError<<"Trees entry not match\t"<<foldrAndTreeName<<": "<<theTree->GetEntries()<<"\t"<<fEntriesOfTree<<DmpLogEndl;
          gCore->TerminateRun();
          return theTree;
        }
      }
      fInTreeSet[folderName].insert(std::make_pair(treeName,theTree));
    }
  }
  return theTree;
}

//-------------------------------------------------------------------
bool DmpRootIOSvc::PrepareEvent()
{
  if(0 == fInTreeSet.size()){
    // some algorithm not use input root file, like Sim and Rdc
    return true;
  }
  if(fCurrentInputEntry >= fEntriesOfTree){    // must after fInTreeSet.size() check
    return false;
  }
  //DmpLogDebug<<"reading event ID = "<<fCurrentInputEntry<<DmpLogEndl;
  if(fCurrentInputEntry < fEntriesOfTree){
    for(DmpRootIOTreeMap::iterator it=fInTreeSet["Event"].begin();it!=fInTreeSet["Event"].end();++it){
      it->second->GetEntry(fCurrentInputEntry);
    }
    ++fCurrentInputEntry;
  }
  return true;
}

bool DmpRootIOSvc::PrepareFirstEvent()
{
  long tmp = fCurrentInputEntry;
  fCurrentInputEntry = fFirstInputEntry;
  bool ss = this->PrepareEvent();
  fCurrentInputEntry = tmp;
  return ss;
}

//-------------------------------------------------------------------
void DmpRootIOSvc::FillData(const std::string &floder){
  if(fOutTreeSet.find(floder) != fOutTreeSet.end()){
    for(DmpRootIOTreeMap::iterator it=fOutTreeSet[floder].begin();it!=fOutTreeSet[floder].end();++it){
      DmpLogDebug<<it->first<<"\tFill "<<floder<<" data "<<it->second->GetEntries()<<DmpLogEndl;
      it->second->Fill();
      ++fWritingEvtID;
    }
  }
}

long DmpRootIOSvc::GetInputEventID()const
{
  if(fCurrentInputEntry == fFirstInputEntry){
    return fCurrentInputEntry;
  }
  return fCurrentInputEntry - 1;    // must -1
}

int DmpRootIOSvc::GetTreeNumberInDir(std::string v,TFile *f)const
{
  int i=0;
  if(f == 0){f = fOutRootFile;}
  if(f != 0){
    TDirectory *dir = f->GetDirectory(v.c_str());
    if(dir){
      i = dir->GetListOfKeys()->GetEntries();
    }
  }
  return i;
}

std::string DmpRootIOSvc::GetJobOptTreeName(std::string v)
{
  static int xxx = 0;
  if(xxx){return fWriteList[1];}
  TString tmp =fWriteList[1];
  tmp.Remove(tmp.Length()-1);
  tmp += GetTreeNumberInDir(v);
  fWriteList[1] = (std::string)tmp;
  ++xxx;
  return fWriteList[1];
}

//-------------------------------------------------------------------
DmpRootIOSvc *gRootIOSvc = DmpRootIOSvc::GetInstance();

