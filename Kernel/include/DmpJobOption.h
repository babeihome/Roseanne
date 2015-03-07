/*
 *  $Id: DmpJobOption.h, 2015-03-06 15:59:34 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 01/10/2014
*/

#ifndef DmpJobOption_H
#define DmpJobOption_H

#include "TObject.h"
#include <map>

class DmpJobOption : public TObject{
/*
 *  JobOptHolder for each Alg
 *
 */
public:
  DmpJobOption();
  ~DmpJobOption();
  DmpJobOption& operator=(const DmpJobOption &r);
  void Reset();
  void LoadFrom(DmpJobOption *r);

public:
  void SetJobName(std::string n){JobName = n;}
  void SetOption(std::string path,const std::string &v);

public:
  std::string GetJobName()const{return JobName;}
  short OptionSize()const{return CmdList.size();}
  void PrintOptions()const;
  std::string GetCommand(const short &i)const{return CmdList.at(i);}
  bool HasCommand(std::string)const;      // whole command, or just a part of command
  std::string GetValue(const std::string&)const;
  std::string GetValue(const short &i)const;

private:
  std::string                       JobName;        // algorithm list
  std::map<std::string,std::string> Option;         // option map
  std::vector<std::string>          CmdList;        //! order of command

  ClassDef(DmpJobOption,1)
};

#endif


