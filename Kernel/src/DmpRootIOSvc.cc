/*
 *  $Id: DmpRootIOSvc.cc, 2014-10-06 14:55:57 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 20/05/2014
*/

#include <boost/algorithm/string.hpp>
#include "TTree.h"
#include "TFile.h"

#include "DmpRootIOSvc.h"

//-------------------------------------------------------------------
DmpRootIOSvc::DmpRootIOSvc()
 :DmpVSvc("DmpRootIOSvc"),
  fInRootFile(0),
  fOutRootFile(0),
  fOutFileKey(""),
  fInPath("./"),
  fOutPath("./"),
  fJobOpt(0)
{
  fInFileName = "";
  fOutFileName = "";
  fJobOpt = new DmpJobOption();
}

//-------------------------------------------------------------------
DmpRootIOSvc::~DmpRootIOSvc(){
  delete fJobOpt;
}

//-------------------------------------------------------------------
void DmpRootIOSvc::SetInputPath(const std::string &argv)
{
  if('/' == argv[argv.length()-1]){
    fInPath = argv;
  }else{
    fInPath = argv+"/";
  }
  fJobOpt->SetOption("IO/Input/Path",argv);
}

//-------------------------------------------------------------------
void DmpRootIOSvc::SetInputFile(const std::string &v)
{
  boost::filesystem::path temp(v);
  fInFileName = temp.filename().string();
  if(temp.parent_path().string()!=""){
    fInPath = temp.parent_path().string()+"/";
  }
  fJobOpt->SetOption("IO/Input/File",v);
}

//-------------------------------------------------------------------
void DmpRootIOSvc::SetOutputPath(const std::string &argv)
{
  if('/' == argv[argv.length()-1]){
    fOutPath = argv;
  }else{
    fOutPath = argv+"/";
  }
  fJobOpt->SetOption("IO/Output/Path",argv);
}

//-------------------------------------------------------------------
void DmpRootIOSvc::SetOutputFile(const std::string &v)
{
  boost::filesystem::path temp(v);
  fOutFileName = temp.filename().string();
  if((temp.parent_path().string()!="") && (fOutPath=="./")){
    fOutPath = temp.parent_path().string()+"/";
  }
  fJobOpt->SetOption("IO/Output/File",v);
}

//-------------------------------------------------------------------
bool DmpRootIOSvc::SetWriteList(const std::string &argv)
{
  boost::split(fWriteList,argv,boost::is_any_of(";"));
  for(short i=0;i<fWriteList.size();++i){
    std::vector<std::string>  temp;
    boost::split(temp,fWriteList[i],boost::is_any_of("/"));
    if(2 != temp.size()){
      DmpLogError<<"Setting WriteList \":\t"<<fWriteList[i]<<DmpLogEndl;
      throw;
    }
  }
  fJobOpt->SetOption("IO/Output/WriteList",argv);
}

//-------------------------------------------------------------------
bool DmpRootIOSvc::Initialize(){
  DmpLogDebug<<"initialization... "<<DmpLogEndl;
  //-------------------------------------------------------------------
  fInFileName = fInPath+GetInputStem()+GetInputExtension();
  if(fInFileName.extension().string() == ".root"){
    fInRootFile = TFile::Open(fInFileName.string().c_str(),"READ");
    if(fInRootFile == 0){
      return false;
    }
  }else if(fInFileName.extension().string() == ".txt"){
// *
// *  TODO: 
// *
  }else if(fInFileName.extension().string() == ".frd"){
  }else if(fInFileName.extension().string() == "./"){
  }else{
    DmpLogError<<" input is not a root file...\t"<<fInFileName.string()<<DmpLogEndl;
    //return false;
  }
  //-------------------------------------------------------------------
  CreateOutRootFile();
  DmpLogDebug<<"... initialization done "<<DmpLogEndl;
  return true;
}

//-------------------------------------------------------------------
void DmpRootIOSvc::CreateOutRootFile(){
  if(0 != fWriteList.size()){
    if(not boost::filesystem::exists(fOutPath)){
      boost::filesystem::create_directories(fOutPath);
    }
    if(fOutFileName.string() == ""){
      if(this->GetInputExtension() == ""){
        this->SetOutputFile("DmpData.root");
      }else{
        this->SetOutputFile(this->GetInputStem()+".root");
      }
    }
    std::string splitMark = "-";
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
bool DmpRootIOSvc::Finalize(){
  // save trees
  if(fOutRootFile){
    DmpLogInfo<<"+-Writing "<<fOutFileName<<DmpLogEndl;
    for(DmpRootIOFolderMap::iterator aFolderMap=fOutTreeSet.begin();aFolderMap != fOutTreeSet.end();++aFolderMap){
      if(aFolderMap->first != "Event"){
        FillData(aFolderMap->first);
      }
      DmpRootIOTreeMap aTreeMap = aFolderMap->second;
      DmpLogInfo<<"| |-"<<aFolderMap->first<<DmpLogEndl;
      fOutRootFile->mkdir((aFolderMap->first).c_str());
      fOutRootFile->cd((aFolderMap->first).c_str());
      for(DmpRootIOTreeMap::iterator it= aTreeMap.begin();it!=aTreeMap.end();++it){
        DmpLogInfo<<"| | |-"<<it->first<<", entries = "<<it->second->GetEntries()<<DmpLogEndl;
        it->second->Write();
        delete it->second;
      }
    }
    DmpLogInfo<<"| |-JobOption"<<DmpLogEndl;
    fOutRootFile->mkdir("Metadata");
    fOutRootFile->cd("Metadata");
    TTree *outputOptTree = new TTree("JobOption","JobOption");
    outputOptTree->Branch("JobOption",fJobOpt->GetName(),&fJobOpt);
    outputOptTree->Fill();
    outputOptTree->Write();
    delete outputOptTree;
    DmpLogInfo<<"`-Done"<<DmpLogEndl;
  }
  // delete root files
  if(fInRootFile){
    fInRootFile->Close();
    delete fInRootFile;
  }
  if(fOutRootFile){
    fOutRootFile->Close();
    delete fOutRootFile;
  }
  return true;
}

//-------------------------------------------------------------------
bool DmpRootIOSvc::WriteValid(const std::string &treeName){
  for(short i=0;i<fWriteList.size();++i){
    if(treeName == fWriteList[i]){
      return true;
    }
  }
  return false;
}

//-------------------------------------------------------------------
TTree* DmpRootIOSvc::GetOutputTree(const std::string &folderName,const std::string &treeName){
  TTree *tree = 0;
  if(fOutTreeSet.find(folderName) != fOutTreeSet.end()){
    if(fOutTreeSet[folderName].find(treeName) != fOutTreeSet[folderName].end()){
      tree = fOutTreeSet[folderName][treeName];
    }
  }else{
    DmpRootIOTreeMap aNewFolder;
    fOutTreeSet.insert(std::make_pair(folderName,aNewFolder));
  }
  if(0 == tree){
    tree = new TTree(treeName.c_str(),treeName.c_str());
    tree->SetAutoSave(50000000);    //50MBytes
    fOutTreeSet[folderName].insert(std::make_pair(treeName,tree));
  }
  return tree;
}

//-------------------------------------------------------------------
TTree* DmpRootIOSvc::GetInputTree(const std::string &folderName,const std::string &treeName){
  TTree *theTree = 0;
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
      fInTreeSet[folderName].insert(std::make_pair(treeName,theTree));
      fEntriesOfTree.insert(std::make_pair(foldrAndTreeName,theTree->GetEntries()));
    }
  }
  return theTree;
}

//-------------------------------------------------------------------
void DmpRootIOSvc::PrepareMetaData(){
  for(DmpRootIOFolderMap::iterator aFolder=fInTreeSet.begin();aFolder!=fInTreeSet.end();++aFolder){
    if("Event" != aFolder->first){
      for(DmpRootIOTreeMap::iterator it=fInTreeSet[aFolder->first].begin();it!=fInTreeSet[aFolder->first].end();++it){
        it->second->GetEntry();
      }
    }
  }
}

//-------------------------------------------------------------------
bool DmpRootIOSvc::PrepareEvent(const long &evtID){
  if(0 == fInTreeSet.size()){
    // some algorithm not use input root file, like Sim and Rdc
    return true;
  }
  DmpLogDebug<<"reading event ID = "<<evtID<<DmpLogEndl;
  bool atLeastONeTree = false;
  for(DmpRootIOTreeMap::iterator it=fInTreeSet["Event"].begin();it!=fInTreeSet["Event"].end();++it){
    if(evtID < fEntriesOfTree["Event/"+it->first]){
      it->second->GetEntry(evtID);
      atLeastONeTree = true;
    }
  }
  return atLeastONeTree;
}

//-------------------------------------------------------------------
void DmpRootIOSvc::FillData(const std::string &floder){
  for(DmpRootIOTreeMap::iterator it=fOutTreeSet[floder].begin();it!=fOutTreeSet[floder].end();++it){
    DmpLogDebug<<it->first<<"\tFill "<<floder<<" data "<<it->second->GetEntries()<<DmpLogEndl;
    it->second->Fill();
  }
}

//-------------------------------------------------------------------
/*
DmpJobOption *DmpRootIOSvc::GetInputFileJobOption()const{
// *
// *  TODO:  could not get it correctly
// *
std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")"<<std::endl;
  static bool haveLoad = false;
  DmpJobOption *inputOpt=0;
  if(not haveLoad){
    inputOpt = new DmpJobOption();
    TTree *theTree = dynamic_cast<TTree*>(fInRootFile->Get("JobOption"));
    theTree->SetBranchAddress("JobOption",&inputOpt);
    haveLoad = true;
  }
  return inputOpt;
}
*/

//-------------------------------------------------------------------
DmpRootIOSvc *gRootIOSvc = DmpRootIOSvc::GetInstance();

