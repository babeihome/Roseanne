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
#include "DmpJobOptLogger.h"

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
  void InputPath(const std::string &v);
  void InputFile(const std::string &v);
  void OutputPath(const std::string &v);
  void OutputFile(const std::string &v);
  bool WriteList(const std::string &v);
  void SetOutputKey(const std::string &v){fOutFileKey = v;}

public:
  bool WriteValid(const std::string &treeName); // in write list, no branch
  TTree* GetOutputTree(const std::string &folderName,const std::string &treeName);
  TTree* GetInputTree(const std::string &folderName,const std::string &treeName);
  void PrepareMetaData();
  bool PrepareEvent(const long &evtID); //only read fInTreeSet["Event"]
  void FillData(const std::string &floderName);
 /*
  *  invoke FillData("Event") in DmpCore::Run(), if fAlgMgr->ProcessThisEvent() return true
  *
  *  invoke FillData("Calibration") in the begin of DmpRootIOSvc::Finalize()
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
  DmpJobOptLogger *JobOptionLogger()const{return fJobLogger;}
  DmpJobOptLogger *GetInputFileJobOption()const;

private:
  DmpRootIOSvc();
  void CreateOutRootFile();

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
  TFile         *fInRootFile;
  TFile         *fOutRootFile;
  std::map<std::string,long>    fEntriesOfTree; // entries of each input event tree. key is "Folder/Tree"
  DmpRootIOFolderMap    fInTreeSet;     // input trees
  DmpRootIOFolderMap    fOutTreeSet;    // output trees
  DmpJobOptLogger       *fJobLogger;    // output job option
};

//-------------------------------------------------------------------
extern DmpRootIOSvc  *gRootIOSvc;

#endif


