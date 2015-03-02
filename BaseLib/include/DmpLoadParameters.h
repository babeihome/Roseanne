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

namespace DAMPE{
namespace Bgo{
 std::map<int, std::vector<double> >  LoadPedestal(std::string inFName); // two elements in vector, first: mean, second: sigma
 void Check(std::string naem);
};
namespace Psd{
 std::map<int, std::vector<double> >  LoadPedestal(std::string inFName);
};
namespace Stk{
 std::map<int, std::vector<double> >  LoadPedestal(std::string inFName);
};
namespace Nud{
 std::map<int, std::vector<double> >  LoadPedestal(std::string inFName);
};
};


#endif

