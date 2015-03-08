/*
 *  $Id: DmpRootIOSvc.h, 2014-11-05 23:07:56 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 20/05/2014
*/

#ifndef DmpRootIOSvc_H
#define DmpRootIOSvc_H

#ifndef __CINT__
#include <boost/filesystem.hpp>     // path
#endif
#include <map>
#include <vector>
#include "DmpVSvc.h"

class TFile;
class TTree;

class DmpRootIOSvc : public DmpVSvc{
/* 
 *  DmpRootIOSvc
 *
 *      it's the first element in DmpServiceManager
 *      it's a singleton for eanch job
 *      only one input and one output root file
 *      input of Simulation and Rdc is not compatible.
 */
public:
  static DmpRootIOSvc* GetInstance(){
    static DmpRootIOSvc instance;
    return &instance;
  }
  ~DmpRootIOSvc();
  bool Initialize();
  bool Finalize();

public:         // binding functions
  void SetInputPath(const std::string &v);
  void SetOutputPath(const std::string &v);
  void SetInputFile(const std::string &v);  // input should be a root file
  void SetOutputFile(const std::string &v); // if not use it, will save data into input root file(if input not exist, will create a default one)
  void SetWriteList(std::string path_SplitBySemicolon);  // Floder/Tree;Floder/Tree1;Floder1/Tree2
  void SetOutFileKey(std::string v){fOutFileKey = v;}
  void SetFirstInputEvent(long i);
  void AddWriteList(std::string path_SplitBySemicolon);
  void AppendWriteList(std::string v){fWriteList.push_back(v);}

public:
  bool WriteValid(const std::string &treeName); // in write list, no branch
  TTree* GetOutputTree(std::string folder_treeName);
  TTree* GetInputTree(std::string folder_treeName);
  bool PrepareEvent(); //only read fInTreeSet["Event"]
  bool PrepareFirstEvent(); //only read fFirstInputEntry from fInTreeSet["Event"]
  void FillData(const std::string &floderName);
 /*
  *  invoke FillData("Event") in DmpCore::Run(), if fAlgMgr->ProcessThisEvent() return true
  *
  */

public:
  std::string GetInputPath()const{return fInPath;}
  std::string GetOutputPath()const{return fOutPath;}
  std::string GetInputFileName()const{return fInFileName.string();}//return full input file name
  std::string GetOutputFileName()const{return fOutFileName.string();}
  std::string GetInputStem()const{return fInFileName.stem().string();}//return input filename without suffix and path
  std::string GetOutputStem()const{return fOutFileName.stem().string();}
  std::string GetInputExtension()const{return fInFileName.extension().string();}
  std::string GetOutputExtension()const{return fOutFileName.extension().string();}
  std::string GetOutFileKey()const{return fOutFileKey;}

public:
  long GetFirstInputEntryID()const{return fFirstInputEntry;}
  long GetInputEventID()const;
  long GetOutputEventID()const{return fWritingEvtID;}

public:
  TFile *GetOutputRootFile()const{return fOutRootFile;}
  TFile *GetInputRootFile()const{return fInRootFile;}
  std::string GetJobOptTreeName(std::string dir);
  int GetTreeNumberInDir(std::string dirName,TFile *f=0)const; // f == 0: for fInRootFile

private:
  DmpRootIOSvc();
  std::vector<std::string> SplitIt(std::string arg, std::string m="/",int size = 2)const;

private:
typedef std::map<std::string, TTree*>  DmpRootIOTreeMap;                // key is "Tree"
typedef std::map<std::string, DmpRootIOTreeMap>  DmpRootIOFolderMap;    // key is "Folder"
#ifndef __CINT__
  boost::filesystem::path   fInFileName;        // include path
  boost::filesystem::path   fOutFileName;       // include path
#endif
  std::string               fOutFileKey;        // key word for the name of output root file
  std::string               fInPath;            // input file path
  std::string               fOutPath;           // output file path
  std::vector<std::string>  fWriteList;         // folderName/treeName
  /*
   *    fWriteList[0] and [1]: is defined in DmpCore
   */
  TFile         *fInRootFile;
  TFile         *fOutRootFile;
  DmpRootIOFolderMap    fInTreeSet;     // input trees
  DmpRootIOFolderMap    fOutTreeSet;    // output trees

private:
  long      fFirstInputEntry;      // for user. from this entry in input tree
  long      fCurrentInputEntry;    // 
  long      fEntriesOfTree;     // all input tree must have the same entries
  long      fWritingEvtID;       // into output tree

};

//-------------------------------------------------------------------
extern DmpRootIOSvc  *gRootIOSvc;

#endif


