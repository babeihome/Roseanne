#ifndef DmpSvcDatabase_H
#define DmpSvcDatabase_H

#include "DmpVSvc.h"
#include <fstream>
#include <vector>
#include <mysql/mysql.h>
#include <map>
#include <sstream>
#include <stdlib.h>

typedef std::map<int, std::vector<double> >	DmpParameterHolder;
typedef std::map<std::string, std::string>	DmpParameterSteering;

class DmpSvcDatabase : public DmpVSvc{
/*
 *  DmpSvcDatabase
 *
 */
public:
	static DmpSvcDatabase* GetInstance()
	{
		static DmpSvcDatabase Database;
		return &Database;
	}

  DmpSvcDatabase();
  ~DmpSvcDatabase();

  //void Set(const std::string &type,const std::string &value);
  // if you need to set some options for your algorithm at run time. Overload Set()
  bool Initialize();
  bool Finalize();

public:
  bool LinkMySQL(std::string userName="DAMPEDB");
  std::fstream *GetData(std::string t0);		// return a file which contains all parameter, refer to t0
  std::fstream *GetData(std::vector<std::string> conditions);
  
  bool Import_pedestal(bool test,std::string path);
  bool Import_pedestal(DmpParameterHolder &pedPar, DmpParameterSteering &steering);
  bool Test();
private:

	std::ofstream logfile;
	MYSQL conn;
	char * strcpy(char * strDest, const char * strSrc);
	void InsertData(const char *table_name, std::map<std::string,std::string> data);

};

extern DmpSvcDatabase *gDatabase;

#endif
