/*
 *  $Id: DmpLoadParameters.h, 2015-03-02 09:12:26 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 01/03/2015
*/

#ifndef LoadParameters_H
#define LoadParameters_H

#include <map>
#include <vector>
#include <string>

typedef std::map<int, std::vector<double> >     DmpParameterHolder;

namespace DAMPE{
namespace Bgo{
 bool LoadPedestal(std::string inFName,DmpParameterHolder &GetPar, std::string &GetStartTime, std::string &GetStopTime); // two elements in vector, first: mean, second: sigma
};
namespace Psd{
 bool  LoadPedestal(std::string inFName,DmpParameterHolder &GetPar, std::string &GetStartTime, std::string &GetStopTime);
};
namespace Stk{
 bool  LoadPedestal(std::string inFName,DmpParameterHolder &GetPar, std::string &GetStartTime, std::string &GetStopTime);
};
namespace Nud{
 bool  LoadPedestal(std::string inFName,DmpParameterHolder &GetPar, std::string &GetStartTime, std::string &GetStopTime);
};

 void CheckPed(std::string naem,std::string DetectorID="Bgo");
};


#endif

