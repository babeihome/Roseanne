/*
 *  $Id: DmpLoadParameters.h, 2015-04-23 11:10:34 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 01/03/2015
*/

#ifndef LoadParameters_H
#define LoadParameters_H

#include <map>
#include <vector>
#include <string>
//#include "DmpParameterPsd.h"
//#include "DmpParameterBgo.h"
//#include "DmpParameterNud.h"
//#include "DmpParameterStk.h"

#define Mark_S  "[Steering]"
#define Mark_D  "[Data]"
#define Mark_N  "[Note]"

typedef std::map<int, std::vector<double> >     DmpParameterHolder;
typedef std::map<std::string, std::string>     DmpParameterSteering;

namespace DAMPE{
 bool LoadParameters(std::string inFName,DmpParameterHolder &GetPar, DmpParameterSteering &GetSteering);

 bool __loadSteering(std::ifstream &f,DmpParameterSteering &steering);
 bool __isNextMarker(std::string aLine);

 void Check2(std::string naem);

         /*
 void Check(std::string naem);
namespace Bgo{
 bool LoadPedestal(std::string inFName,DmpParameterHolder &GetPar, DmpParameterSteering &GetSteering);
 bool LoadRelation(std::string inFName,DmpParameterHolder &GetPar, DmpParameterSteering &GetSteering);
 bool LoadMip(std::string inFName,DmpParameterHolder &GetPar, DmpParameterSteering &GetSteering);
};
namespace Psd{
 bool LoadPedestal(std::string inFName,DmpParameterHolder &GetPar, DmpParameterSteering &GetSteering);
 bool LoadRelation(std::string inFName,DmpParameterHolder &GetPar, DmpParameterSteering &GetSteering);
 bool LoadMip(std::string inFName,DmpParameterHolder &GetPar, DmpParameterSteering &GetSteering);
};
namespace Stk{
 bool LoadPedestal(std::string inFName,DmpParameterHolder &GetPar, DmpParameterSteering &GetSteering); // two elements in vector, first: mean, second: sigma
};
namespace Nud{
 bool LoadPedestal(std::string inFName,DmpParameterHolder &GetPar, DmpParameterSteering &GetSteering); // two elements in vector, first: mean, second: sigma
};
*/

};


#endif

