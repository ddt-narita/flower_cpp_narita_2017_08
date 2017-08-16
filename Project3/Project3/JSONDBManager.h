#pragma once

#ifndef __JSONDBManager
#define __JSONDBManager

#include "picojson.h"
#include "DbConnect.h"
#include "Db_ResultTree.h"
#include "cppconn/resultset.h"

class JSONDBManager :public DbConnect{
public:
	int processedRecords;
	picojson::value json;

private:
	std::string outHTMLString;

public:
	void createJson(picojson::value &json, std::string key, Db_ResultTree *dbrt_parent);
	sql::ResultSet& executeQuery(picojson::value& json, std::string queryKey);
	std::string getDBColumn(std::string key, Db_ResultTree *db_resultTree);
	std::string getListJSON(picojson::value &json);
	void outputJSON(std::string jsonString, std::string key);
	void getJSONMap(std::string jsonString);
	bool checkColumn(sql::ResultSet *rs, std::string columnName);
	std::string getListJSONPlusKey(picojson::value &json, std::string key);
	std::string getJsonValue(picojson::value &map, std::string jsonKey, std::string valueKey);
	std::string getOutHTMLStrring();
	void setOutHTMLString(std::string outHTMLString);
	std::string encodeJSONMap(picojson::value json);
	std::string encodeJSONArray(picojson::value json);

private:
	std::string createReplaceValue(picojson::value& childObjectValue);
	std::string encryptionSHA1(std::string);
	std::string escapeSQLValue(std::string str);
	std::string escapeJSONValue(std::string str);
	std::string encodeJSON(picojson::value json);
};



#endif __JSONDBManager