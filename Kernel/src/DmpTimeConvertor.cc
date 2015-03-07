/*
 *  $Id: DmpTimeConvertor.cc, 2015-03-05 07:00:01 DAMPE $
 *  Author(s):
 *    Zhiyong ZHANG (zhzhy@mail.ustc.edu.cn) 27/10/2014
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 28/10/2014
*/

#include <time.h>
#include <stdio.h>
#include <sstream>
#include "DmpTimeConvertor.h"

namespace DmpTimeConvertor{

//-------------------------------------------------------------------
std::string Second2Date(const int &second){
  //conminder time zone
  struct tm t0= {0,0,0,1,1-1,1970-1900};//{sec,min,hour,mday(1-31),mon(0-11),year(real-1900)}
  struct tm launchTime= {0,0,0,1,1-1,2013-1900};//{sec,min,hour,mday(1-31),mon(0-11),year(real-1900)}
  time_t mytime = (int)difftime(mktime(&launchTime),mktime(&t0))+second;
  char tmp[256];
  strftime(tmp,256,"%F %T",gmtime(&mytime));
  return (std::string)tmp;
}

//-------------------------------------------------------------------
int Date2Second(const std::string &date){
//time YYYY-MM-DD HH:MM:SS
  std::string tmp = date.substr(0,4)+" "+date.substr(5,2)+" "+date.substr(8,2)+" "+date.substr(11,2)+" "+date.substr(14,2)+" "+date.substr(17,2);
  std::istringstream iss(tmp);
  int year,month,day,hour,min,sec;
  iss>>year>>month>>day>>hour>>min>>sec;
  struct tm mytime={sec,min,hour,day,month-1,year-1900};
  struct tm launchTime= {0,0,0,1,1-1,2013-1900};//{sec,min,hour,mday(1-31),mon(0-11),year(real-1900)}

  return (int)difftime(mktime(&mytime),mktime(&launchTime));
}

}


