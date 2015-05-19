#include "DmpLoadParameters.h"
#include "DmpSvcDatabase.h"
#include <iostream>
#include <mysql/mysql.h>
#include <string>
#include <map>

#define HOST "dampe.ustc.edu.cn"
#define USER "cong"
#define PASSWD "cong"
#define DB "DmpSWDBTest"

using namespace std;
//-------------------------------------------------------------------
DmpSvcDatabase::DmpSvcDatabase()
 :DmpVSvc("DmpSvcDatabase")
{
	std::cout<<"\n\n Database object created\n\n"<<std::endl;
}

//-------------------------------------------------------------------
DmpSvcDatabase::~DmpSvcDatabase(){
}

bool DmpSvcDatabase::Test()
{
	string filepath = "../Bgo_DmpRdcData.ped";
	DmpParameterSteering parameters;
	DmpParameterHolder datas;	
	DAMPE::LoadParameters(filepath, datas, parameters);
	cout << parameters.size() << " and " << datas.size() <<endl;
	Import_pedestal(datas, parameters);
	return true;
}

//-------------------------------------------------------------------
bool DmpSvcDatabase::Initialize(){
	std::cout<<"xxxxxx----"<<std::endl;
	return true;
	mysql_init(&conn);
	if (&conn == NULL){
		printf("Error %u: %s\n", mysql_errno(&conn), mysql_error(&conn));
		return 0;
	}
	logfile.open("./log.txt");
	if(!mysql_real_connect(&conn, HOST, USER, PASSWD, DB, 3306, NULL, 0))
	{
		printf("Error %u: %s\n", mysql_errno(&conn), mysql_error(&conn));
		return 0;
	}
	GetData("500000");
	//Import_pedestal(true, "");
  	return true;
}

//-------------------------------------------------------------------
bool DmpSvcDatabase::Finalize(){
	//std::cout<<""<<std::endl;
	mysql_close(&conn);
  	logfile.close();
	return true;
}

std::fstream *DmpSvcDatabase::GetData(std::string t0)
{
	// Reminder of add parameter of ofstream*
	std::string order = "SELECT * FROM time_index order by abs(" + t0 + " - E_time0) DESC LIMIT 1";
	MYSQL_RES * result = NULL;
	MYSQL_ROW row;
	int num_fields = 0;
	std::fstream  fs;
	if (mysql_query(&conn, order.c_str())){
		printf("Error %u: %s\n", mysql_errno(&conn), mysql_error(&conn));
		return 0;
	}
	result = mysql_store_result(&conn);
	num_fields = mysql_num_fields(result);
	row = mysql_fetch_row(result);
	// consider the condition of no result
	if(row){
		fs.open("./pedestal.txt",std::ios::out);
		std::cout << row[1] << std::endl;
		fs << row[0];
		fs.close();
	}
	else{
		cout << "No data selected" << endl;
	}
	return 0;
}

bool DmpSvcDatabase::Import_pedestal(bool test, std::string path)
{
	int para_num = 0;
	char one_line[20];
	string one_line_str;
	string E_name;
	string order;
	//vector<string> data_vector;
	vector<string> para_name;
	map<string,string> data_dict;
	map<string,string> time_index;

	// open file and check
	if (test == true){
		path = "/data/beamTest/2nd_2014_10/Calibration/DAMPE/Pedestal/PedestalPar";
		para_num = 2;
		para_name.push_back("E_time0");
		para_name.push_back("E_time1");
		para_name.push_back("data_package");
	}
	std::ifstream infile(path.c_str());
	if(!infile)
	{
		cout<< "Targeted file is not existed" << endl;
	}

	//read in the parameter
	infile >> one_line;
	E_name = one_line;
	strcpy(one_line, "");

	for(int para_count = 0; para_count < para_num; para_count++)
	{
		infile >> one_line;
		one_line_str = one_line;
		data_dict[para_name[para_count]] = one_line_str;
		strcpy(one_line,"");
		//data_vector.push_back(one_line_str);
	}
	//insert_one(para_name,data_vector);
	
	ifstream in(path.c_str());
	istreambuf_iterator<char> beg(in), end;
	string strdata(beg, end);
	in.close();
	
	data_dict[para_name[para_num]] = strdata;
	
	InsertData("exp_data",data_dict);
	
	//data_vector.clear();
	order = "SELECT S_time FROM set_data WHERE S_time <= " + data_dict["E_time0"] + " ORDER BY S_time DESC LIMIT 1";
	logfile<< order <<endl;	
	if (mysql_query(&conn, order.c_str())){
		printf("Error %u: %s\n", mysql_errno(&conn), mysql_error(&conn));
		return 0;
	}

	MYSQL_RES * result;
	MYSQL_ROW row;
	int num_fields = 0;

	result = mysql_store_result(&conn);
	num_fields = mysql_num_fields(result);
	row = mysql_fetch_row(result);
	if(row){
		time_index["S_time"] =  row[0];
		time_index["E_time0"] =  data_dict["E_time0"];
		time_index["E_name"] = E_name;	
	}		
	InsertData("time_index",time_index);
	return 1;	
}

bool DmpSvcDatabase::Import_pedestal(DmpParameterHolder &pedPar, DmpParameterSteering &steering)
{	
	//Initialize variables
	int SIZE = 5;	
	std::string data;
	std::ostringstream buffer_stream;
	std::vector<double> double_vec;
	map<string,string> data_dict;
	map<string,string> index(steering);
	map<int,string> type_dict;
	type_dict[0] = "pedestal";
	type_dict[1] = "mips";
	type_dict[2] = "linear";
	type_dict[3] = "attenu";
	
	std::vector<string> pedestal;
	pedestal.push_back("mean");
	pedestal.push_back("sigma");	
	pedestal.push_back("NDF");
	pedestal.push_back("entries");	
	pedestal.push_back("fit_status");		
	//Check if paras are match to table attribute
	//Check_ismatch(type, steering);
	
	//Construct data map waited inserting
	InsertData("index",index);
		
	for(std::map<int, std::vector<double> >::iterator iter=pedPar.begin(); iter!=pedPar.end(); ++iter)
	{
		data_dict["global_id"] = iter->first;
		double_vec = iter->second;
		// SIZE need to get
		for(int i=0; i<SIZE;i++){
			data_dict[pedestal[i]] = double_vec[i];
		}
	}
	InsertData(type_dict[atoi(steering["type"].c_str())].c_str(), data_dict);

	return 1;	
}

void DmpSvcDatabase::InsertData(const char *table_name, map<string,string> data)
{	
	typedef map<string, string>::iterator DI;
	string para_name_set = "";
	string value_set = "";
	string order;
	string table_name_str = table_name;


	MYSQL_RES *result;
	MYSQL_ROW row;
	for(DI p = data.begin(); p != data.end(); p++)
	{
		para_name_set += ( p->first + ",");
		value_set += ("\"" + p->second + "\",");	
	}
	para_name_set = para_name_set.substr(0, para_name_set.length() - 1 );
	value_set = value_set.substr(0, value_set.length() - 1 );
	order = "INSERT INTO " + table_name_str + "(" + para_name_set + ")" + " VALUES (" + value_set + ")";
	logfile << order <<endl;
	if (mysql_query(&conn, order.c_str())){
		printf("Error %u: %s\n", mysql_errno(&conn), mysql_error(&conn));
		return;
	}
	result = mysql_store_result(&conn);
	if (result == NULL){
		if(mysql_error(&conn)){
			printf("Error %u: %s\n", mysql_errno(&conn), mysql_error(&conn));
		}
		else{
			printf("Error %u: %s\n", mysql_errno(&conn), mysql_error(&conn));
		}
		return;
	} 
}

char * DmpSvcDatabase::strcpy(char * strDest,const char * strSrc)
{
	if ((NULL==strDest) || (NULL==strSrc)) //[1]
		cout<< "Invalid argument(s)"; //[2]
	char * strDestCopy = strDest; //[3]
	while ((*strDest++=*strSrc++)!='\0'); //[4]
	return strDestCopy;
}
DmpSvcDatabase *gDatabase = DmpSvcDatabase::GetInstance();
