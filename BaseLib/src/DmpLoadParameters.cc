/*
 *  $Id: DmpLoadParameters.cc, 2015-03-02 09:13:07 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 01/03/2015
*/

#include <fstream>
#include <iostream>
#include <sstream>

#include "DmpLoadParameters.h"

#ifndef LoadPedestal_CXX
#define LoadPedestal_CXX

namespace DAMPE{
namespace Bgo{
 std::map<int, std::vector<double> >  LoadPedestal(std::string inFName)
 {
   std::map<int, std::vector<double> >  pedPar;
   std::ifstream inPed_f(inFName.c_str());
   if(! inPed_f.good()){
     std::cout<<"open file failed...\t"<<inFName<<std::endl;
     inPed_f.close();
     return pedPar;
   }
   std::string  tmp;
   getline(inPed_f,tmp);
   getline(inPed_f,tmp);
   getline(inPed_f,tmp);
   int gid=0;
   std::vector<double> mean_sig(2,0);
   while(! inPed_f.eof()){
     getline(inPed_f,tmp);
     std::istringstream stream(tmp);
     stream>>gid;
     stream>>mean_sig[0]>>mean_sig[0]>>mean_sig[0]>>mean_sig[0]>>mean_sig[0];
     stream>>mean_sig[1];
     pedPar.insert(std::make_pair(gid,mean_sig));
   }

   inPed_f.close();
   return pedPar;
 }

 void Check(std::string naem)
 {
   std::map<int, std::vector<double> >  mypedPar = LoadPedestal(naem);
   for(std::map<int,std::vector<double> >::iterator it = mypedPar.begin();it != mypedPar.end();++it){
           std::cout<<it->first<<"\t"<<it->second.at(0)<<"\t"<<it->second.at(1)<<std::endl;
   }
 }

};

namespace Psd{
 std::map<int, std::vector<double> >  LoadPedestal(std::string inFName)
 {
   std::map<int, std::vector<double> >  pedPar;
   return pedPar;
 }
}

namespace Stk{
 std::map<int, std::vector<double> >  LoadPedestal(std::string inFName)
 {
   std::map<int, std::vector<double> >  pedPar;
   return pedPar;
 }
}

namespace Nud{
 std::map<int, std::vector<double> >  LoadPedestal(std::string inFName)
 {
   std::map<int, std::vector<double> >  pedPar;
   return pedPar;
 }
}
};

#endif

