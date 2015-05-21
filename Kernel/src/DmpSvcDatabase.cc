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
	string filepath = "./Bgo_DmpRdcData.ped";
	DmpParameterSteering parameters;
	DmpParameterHolder datas;	
	DAMPE::LoadParameters(filepath, datas, parameters);
	cout << parameters.size() << " and " << datas.size() <<endl;
	
	/*
	for(std::map<int, std::vector<double> >::iterator iter=datas.begin(); iter!=datas.end(); ++iter)
	{
		cout << iter->first<< ' ';
		std::vector<double> double_vec = iter->second;
		// SIZE need to get
		for(int i=0; i<5;i++){
			cout << double_vec[i]<< ' ';
		}
		cout << endl;
	}
	return true;
	*/	
	Import_pedestal(datas, parameters);
	return true;
}

//-------------------------------------------------------------------
bool DmpSvcDatabase::Initialize(){
	std::cout<<"xxxxxx----"<<std::endl;
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
	//Import_pedestal(true, "");
	std::vector< std::string > attri_temp;

	type_dict[0] = "pedestal";
	type_dict[1] = "mips";
	type_dict[2] = "linear";
	type_dict[3] = "attenu";
	attri_temp.push_back("mean");
	attri_temp.push_back("sigma");	
	attri_temp.push_back("NDF");
	attri_temp.push_back("entries");	
	attri_temp.push_back("fit_status");
	attri.push_back(attri_temp);
	attri_temp.clear();

	attri_temp.push_back("width");
	attri_temp.push_back("MP");
	attri_temp.push_back("area");
	attri_temp.push_back("gsigma");
	attri_temp.push_back("X");
	attri_temp.push_back("NDF");
	attri_temp.push_back("entries");
	attri_temp.push_back("fit_status");
	attri.push_back(attri_temp);
	attri_temp.clear();

	attri_temp.push_back("p0");
	attri_temp.push_back("p1");
	attri_temp.push_back("NDF");
	attri_temp.push_back("entries");
	attri_temp.push_back("fit_status");
	attri.push_back(attri_temp);
	attri_temp.clear();

	attri_temp.push_back("p0");
	attri_temp.push_back("p0_error");
	attri_temp.push_back("p1");
	attri_temp.push_back("p1_error");
	attri.push_back(attri_temp);
	attri_temp.clear();

	SIZE.push_back(5);
	SIZE.push_back(8);
	SIZE.push_back(5);
	SIZE.push_back(4);

	str_0 = "0";
	str_1 = "1";
	str_2 = "2";
	str_3 = "3";

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

bool DmpSvcDatabase::GetData( std::map< std::string, std::string> query, DmpParameterHolder &pedPar, DmpParameterSteering &steering)
{
	// Reminder of add parameter of ofstream*
	map<string, string>::iterator query_iter = query.find("exp_id");
	string type;
	int type_num;
	std::string order;
	MYSQL_RES * result = NULL;
	MYSQL_ROW row;
	if(query_iter != query.end())
	{
		order = "SELECT * FROM index_table where exp_id = " + query_iter->second;
		if (mysql_query(&conn, order.c_str())){
			printf("Error %u: %s\n", mysql_errno(&conn), mysql_error(&conn));
			return 0;
		}
		result = mysql_store_result(&conn);
		row = mysql_fetch_row(result);
		// consider the condition of no result
		if(row){
			type = row[6];
			type_num = atoi(type.c_str());
			steering["StartTime"] = row[1];
			steering["StopTime"] = row[2];
			steering["FileName"] = row[3];
			steering["Detector"] = row[5];
			steering["Type"] = row[6];
		}
		else{
			cout << "No data selected" << endl;
		}
		mysql_free_result(result);
		order = "SELECT * FROM" + type_dict[atoi(type.c_str())] + "WHERE exp_id=" + query_iter->second + "and Type=" + type;
		
		if (mysql_query(&conn, order.c_str())){
			printf("Error %u: %s\n", mysql_errno(&conn), mysql_error(&conn));
			return 0;
		}
		result = mysql_store_result(&conn);
		// consider the condition of no result
	}
	else if(query.end() != query.find("time"))
	{
		
	}
	if(result){
		int global_id;

		for(int i=0;i<mysql_num_rows(result);i++)
		{
			row = mysql_fetch_row(result);
			global_id = atoi(row[2]);
			for(int j=3; j<SIZE[type_num]; j++)
			{
					
			}

		}
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
	std::string data;
	/*const std::string str_0 = "0";
	const std::string str_1 = "1";
	const std::string str_2 = "2";
	const std::string str_3 = "3";
	*/
	std::stringstream buffer_stream;
	std::vector<double> double_vec;
	map<string,string> data_dict;
	map<string,string> index(steering);
	map<int,string> type_dict;
	type_dict[0] = "pedestal";
	type_dict[1] = "mips";
	type_dict[2] = "linear";
	type_dict[3] = "attenu";
	int type_num;	
	//std::vector<std::string> attri;
	//showmap(steering);	
	/*if (steering["Type"] == str_0)
	{
		SIZE = 5;
		attri.push_back("mean");
		attri.push_back("sigma");	
		attri.push_back("NDF");
		attri.push_back("entries");	
		attri.push_back("fit_status");		
	}
	else if (steering["Type"] == str_1)
	{
		SIZE = 8;
		attri.push_back("width");
		attri.push_back("MP");
		attri.push_back("area");
		attri.push_back("gsigma");
		attri.push_back("X");
		attri.push_back("NDF");
		attri.push_back("entries");
		attri.push_back("fit_status");
	}
	else if (steering["Type"] == str_2)
	{
		SIZE = 5;

		attri.push_back("p0");
		attri.push_back("p1");
		attri.push_back("NDF");
		attri.push_back("entries");
		attri.push_back("fit_status");
	}
	else if (steering["Type"] == str_3)
	{
		SIZE = 4;

		attri.push_back("p0");
		attri.push_back("p0_error");
		attri.push_back("p1");
		attri.push_back("p1_error");
	}
	else
	{
		cout << "type not match!" << endl;
	}
	*/
	//Check if paras are match to table attribute
	//Check_ismatch(type, steering);
	
	//Construct data map waited inserting
	//cout << "Stage 1"<<endl;
	/*
	for(std::map<std::string , std::string >::iterator iter=steering.begin(); iter!=steering.end(); ++iter)
	{
		cout << iter->first <<endl;
		cout << iter->second<<endl;
		cout<<endl;
	}
	*/
	InsertData("index_table",index);
	

	//Get exp_id which belong to log just inserted
	std::string order = "SELECT exp_id FROM index_table order by exp_id DESC LIMIT 1";
	MYSQL_RES * result = NULL;
	MYSQL_ROW row;
	if (mysql_query(&conn, order.c_str())){
		printf("Error %u: %s\n", mysql_errno(&conn), mysql_error(&conn));
		return 0;
	}
	result = mysql_store_result(&conn);
	row = mysql_fetch_row(result);
	// consider the condition of no result
	if(row){
		data_dict["exp_id"] = row[0];
	}
	else{
		cout << "No data selected" << endl;
	}
	mysql_free_result(result);
	//exp_id has been placed in data_dict
	

	//cout << "stage 2";	
	for(std::map<int, std::vector<double> >::iterator iter=pedPar.begin(); iter!=pedPar.end(); ++iter)
	{
		//cout << "Stage 2.1";
		buffer_stream << iter->first;
		data = buffer_stream.str();
		buffer_stream.clear();
		buffer_stream.str("");
		data_dict["global_id"] = data;
		double_vec = iter->second;
		//cout << "Stage 2.2";
		type_num = atoi(steering["Type"].c_str());
		for(int i=0; i<SIZE[type_num];i++){
			buffer_stream << double_vec[i];
			data = buffer_stream.str();
			buffer_stream.clear();
			buffer_stream.str("");
			data_dict[attri[type_num][i]] = data;
		}
		
		/* SHOW DATA CONTENTS
		 * for(std::map<std::string , std::string >::iterator iter=data_dict.begin(); iter!=data_dict.end(); ++iter)
		{
			cout << iter->first <<endl;
			cout << iter->second<<endl;
			cout<<endl;
		}
		*/
		InsertData(type_dict[type_num].c_str(), data_dict);
	}
	//cout << "Stage 3";

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
		cout<<"1"<<endl;
		return;
	}
	result = mysql_store_result(&conn);
	/*if (result == NULL){
		if(!mysql_error(&conn)){
			printf("Error %u: %s\n", mysql_errno(&conn), mysql_error(&conn));
			cout<<"2"<<endl;
		}
		else{
			printf("Error %u: %s\n", mysql_errno(&conn), mysql_error(&conn));
			cout<<"3"<<endl;
		}
		return;
	}
       */
	return;	
}

char * DmpSvcDatabase::strcpy(char * strDest,const char * strSrc)
{
	if ((NULL==strDest) || (NULL==strSrc)) //[1]
		cout<< "Invalid argument(s)"; //[2]
	char * strDestCopy = strDest; //[3]
	while ((*strDest++=*strSrc++)!='\0'); //[4]
	return strDestCopy;
}

void DmpSvcDatabase::showmap(std::map< std::string, std::string> map)
{
	
	for(std::map<std::string , std::string >::iterator iter=map.begin(); iter!=map.end(); ++iter)
	{
		cout << iter->first <<endl;
		cout << iter->second<<endl;
		cout<<endl;
	}
	return;
}

DmpSvcDatabase *gDatabase = DmpSvcDatabase::GetInstance();
