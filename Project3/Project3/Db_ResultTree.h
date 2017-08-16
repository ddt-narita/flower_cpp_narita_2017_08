#pragma once


#include "picojson.h"
#include "cppconn/resultset.h"

class Db_ResultTree {
public:
	Db_ResultTree *parent = nullptr;		//このノード（インスタンス）の親
	picojson::object json;	//JSONデータの連想配列
	std::string keyData = "";				//メンバのjsonキー
	sql::ResultSet *db_result = nullptr;			//DBの結果セット

	/*一番親のノードかどうか調べるための変数
	int topNode;

	bool is_top() {
		return topNode == 0;
	}
	*/
};