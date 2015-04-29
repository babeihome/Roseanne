/*
 *  $Id: DmpVSvc.h, 2014-07-08 14:43:05 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 10/04/2014
*/

#ifndef DmpVSvc_H
#define DmpVSvc_H

#include <string>
#include "DmpLog.h"

class DmpVSvc{
/*
 *  DmpVSvc
 *
 *      all service derived from this class, and then
 *      append into Dmp Service Manager, in python
 *
 */
public:
  DmpVSvc(const std::string &n):fName(n){}
  virtual ~DmpVSvc(){}
  virtual bool Initialize()=0;
  virtual bool Finalize()=0;

public:
  std::string Name() const {return fName;}
  std::string GetName() const {return fName;}

private:
  std::string   fName;

};

#endif


