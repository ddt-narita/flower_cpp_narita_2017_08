

#include "JSONDBManager.h"
#include "Constants.h"
#include <boost/algorithm/string.hpp>
#include "mysql_connection.h"
#include  <memory>
#include  <mysql_driver.h>
#include  <mysql_connection.h>
#include  <mysql_error.h>
#include  <cppconn/Statement.h>
#include  <cppconn/ResultSet.h>
#include <string>

using namespace std;

Constants constants;



/*
関数名:createJson
概要:DBから取得したデータについてのJsonを作成する
引数:picojson::object json
	:string key
	:Db_ResultTree
返却値:なし
作成者:成田修之
作成日:8月15日(火)
*/
void JSONDBManager::createJson(picojson::object &json, std::string key, Db_ResultTree * dbrt_parent)
{
	//DBの結果からツリーを構築するクラスのインスタンス生成
	Db_ResultTree *db_resultTree = new Db_ResultTree();
	
	//DBの結果をツリーの結果部に格納
	*(db_resultTree->db_result) =  executeQuery(json, key);
	//引数の親ノードを格納
	db_resultTree->parent = dbrt_parent;
	//現在のjsonを格納
	db_resultTree->json = json;
	//現在のkeyを格納
	db_resultTree->keyData = key;


	//DBからの結果の中からkeyに当たるものを取得
	string column = getDBColumn(key, db_resultTree);
	
	//jsonの最深層部まですべて走査
	for (picojson::object::iterator i = json.begin(); i != json.end(); i++) {
		//jsonのkeyを取得
		string keyString = i->first;
		//valueが子オブジェクトの時（まだネストしているとき）
		if (i->second.is<picojson::object>()) {
			//子オブジェクトについて再帰処理
			createJson(json[i->first].get<picojson::object>(), keyString, db_resultTree);
		}
		//カラムがきちんと取得でき、かつキーの名前がtext,html,srcのいずれかである
		else if (column != "" && (keyString == constants.KEY_TEXT
			|| keyString == constants.KEY_HTML || keyString == constants.KEY_SRC)) {
			
			//そのキーの値をカラムに置き換え
			json[keyString].get<string>() = column;
		}
	}



}

/*
関数名:executeQuery
概要:クエリを実行し、DBから結果セットを取得
引数:picojson::object& json　カレントのJSON
	:string queryKey クエリがあるキー
返却値:ResultSet *retRs DBから取得した結果セットを返す
作成者:成田修之
作成日:8月15日(火)
*/


sql::ResultSet& JSONDBManager::executeQuery(picojson::object &json, std::string queryKey)
{
	sql::ResultSet *retRs = nullptr;

	//ユーザ情報を保護するためパスワードがkeyにあればハッシュ化する
	if (json.count(constants.STR_PASSWORD) != 0) {
		//パスワードのあるマップを取得
		picojson::object replaceJson = json[constants.STR_PASSWORD].get<picojson::object>();

		//パスワードをSHA1で暗号化して置き換え
		replaceJson[constants.KEY_VALUE].get<string>() = encryptionSHA1(replaceJson[constants.KEY_VALUE].get<string>());
		//引数のjsonのパスワードのあるマップにも影響させる
		json[constants.STR_PASSWORD].get<picojson::object>() = replaceJson;
	}
	//queryKeyをキーに持つノードがある
	if (json.count(queryKey) != 0) {
		//クエリを取り出す
		string query = json[queryKey].get<string>();
		//きちんとクエリが取得できている
		if (query != "") {
			//最深層まで再帰処理で潜る
			for (picojson::object::iterator i = json.begin(); i != json.end(); i++) {
				//子を持ち、まだネストしているとき
				if (i->second.is<picojson::object>()) {
					picojson::object childObject = json[i->first].get<picojson::object>();
					//子オブジェクトがvalueのキーを持っている
					if (childObject.count(constants.KEY_VALUE) != 0) {
						
						//変換すべき文字列を取得
						string replaceValue = createReplaceValue(childObject[constants.KEY_VALUE]);
						//クエリの変換対象の文字列を取得した変換すべき文字列に置き換え
						query = query.replace(query.find("'" + i->first + "'"), i->first.length() + 2, ("'" + replaceValue + "'"));
					}
				}
			}
		
			//ステートメント作成
			sql::Statement *stmt = con->createStatement();
			//クエリ内容をコンソールに表示
			cout << "query:" + query;
			//クエリを実行
			retRs = stmt->executeQuery(query);
			//結果セットのカーソルを結果件数を取得するために最後に持ってくる
			retRs->last();
			//結果件数を取得
			this->processedRecords = retRs->getRow();
			//取得したのでカーソルを最初に戻す
			retRs->beforeFirst();
		}
	}
	//DBにクエリを投げた結果セットを取得する
	return *retRs;
}


/*
関数名:createReplaceValue
概要:DBから取得したデータについてのJsonを作成する
引数:picojson::value& childObjectValue
返却値:string 作成した返却用の文字列
作成者:成田修之
作成日:8月16日(水)
*/
std::string JSONDBManager::createReplaceValue(picojson::value &childObjectValue)
{
	//返却する置換するための文字列
	string retReplaceString = "";
	//データを作るために文字列を格納する配列
	vector<string> childObjectArray;
	
	//受け取った要素が配列の時
	if (childObjectValue.is<picojson::array>()) {
		//配列部分を取得
		picojson::array tempChildObjectArray = childObjectValue.get<picojson::array>();
		//配列の全要素（文字列）を追加
		//childObjectArray.assign(tempChildObjectArray.begin(), tempChildObjectArray.end());
		for (picojson::array::iterator i = tempChildObjectArray.begin(); i != tempChildObjectArray.end(); i++) {
			childObjectArray.push_back(i->get<string>());
		}
	
	
	}//配列以外の時
	else {
		//文字列を追加する
		childObjectArray.push_back(childObjectValue.get<string>());
	}
	
	//作った配列の全要素分繰り返す
	for (int i = 0; i < childObjectArray.size(); i++) {
		//返却する置換する文字列が空白なら何もせず、値があれば区切り文字を追加
		retReplaceString += retReplaceString.empty() ? "" : ",";
		//文字列に対してエスケープ処理を施して追加していく
		retReplaceString += escapeSQLValue(childObjectArray[i]);
	}

	//作成した文字列を返却する
	return retReplaceString;
}


/*
関数名:getDBColumn
概要:DBから取得したデータについてのJsonを作成する
参考:http://qiita.com/iseki-masaya/items/70b4ee6e0877d12dafa8
	コメント欄のスゴロク様のコードを参考にデリミタが一文字でないときのものも対応させました。
引数:string s 区切りたい対象文字列
	:string delim 区切り文字の指定
返却値:string column 取得した列の値を返却
*/
vector<string> split(const string& s, string delim) {
	std::vector<string> elms;
	size_t offset = 0;
	while (true) {
		//最初にあるデリミタの位置取得
		size_t next = s.find_first_of(delim, offset);
		//デリミタがもうない時
		if (next == string::npos) {
			//残りをベクトルの末尾に追加
			elms.push_back(s.substr(offset));
			//プログラムを終了する
			return elms;
		}
		//デリミタの前までをベクトルの末尾に追加していく
		elms.push_back(s.substr(offset, next - offset));
		//next + 1 →　next + delim.size()でデリミタの長さに対応
		offset = next + delim.size();
	}
}

/*
関数名:getDBColumn
概要:DBから取得したデータについてのJsonを作成する
引数:string key JSONオブジェクトのキー　
	:Db_ResultTree dbrTree　DBから取得した結果をツリー構造にするクラスのインスタンス
返却値:string column 取得した列の値を返却
作成者:成田修之
作成日:8月15日(火)
*/
std::string JSONDBManager::getDBColumn(std::string key, Db_ResultTree *dbrTree)
{
	//取得した列の値を返却するのに使う変数
	string column = "";
	//行数を格納する変数
	int columnNumber = 0;
	//親のキーが配列の要素であることを示す文字を含んでいるとき
	if(dbrTree->parent->keyData.find(constants.STR_TWO_UNDERBAR) != string::npos){
		//デリミタで配列に分割
		vector<string> keySting = split(dbrTree->parent->keyData, constants.STR_TWO_UNDERBAR);
		//行数を取得
		columnNumber = stoi(keySting[1]);
	}
	
	//親ノードがいなくなるまで繰り返す
	while (dbrTree != nullptr) {
		//keyの列名を持つ列が結果セットに存在する
		if (checkColumn(dbrTree->db_result, key)) {
			//結果セットのカーソルをcolumnNumberの位置へ移動
			if (dbrTree->db_result->absolute(columnNumber + 1)) {
				//カラムの値を取得
				column = dbrTree->db_result->getString(key);
			}//ループを抜ける
			break;
		}//結果セットにkeyの列名の列は存在しない
		else {
			//親について調べられるように親をセット
			dbrTree = dbrTree->parent;
		}
	}//取得した列の値を返却する
	return column;
}

/*
関数名:getListJSON
概要:DBから取得したデータについてのJsonを作成する
引数:string key JSONオブジェクトのキー　
	:Db_ResultTree dbrTree　DBから取得した結果をツリー構造にするクラスのインスタンス
返却値:string column 取得した列の値を返却
作成者:成田修之
作成日:8月15日(火)
*/
string JSONDBManager::getListJSON(picojson::object & json)
{
	string strAll = "";		//Json全体の文字列を格納する変数
	string strBlock = "";	//ブロックごとの文字列を格納する変数
	string strLine = "";	//一行ごとの文字列を格納する変数

	//DBにクエリをなげ結果セットを取得
	sql::ResultSet *rs = &executeQuery(json, constants.DB_GETQUERY);
	//列数を取得するためのメタデータを作る
	sql::ResultSetMetaData *rsMeta = rs->getMetaData();
	//メタデータから結果セットの列数を取得
	int columnCount = rsMeta->getColumnCount();

	//結果セットの行についてすべて走査
	while (rs->next()) {
		//一行ごとの文字列をクリア
		strLine = "";
		//列数分繰り返す
		for (int i = 0; i < columnCount; i++) {
			//列の名前を取得する
			string sColName = rsMeta->getColumnLabel(i + 1);
			//列の値を取得する
			string value = rs->getString(i + 1);
			//取得した値がNULLなら空白を格納
			value = &value == nullptr ? constants.EMPTY_STRING : value;
			//一行ごとの文字列に空白ではないならカンマの区切りを加える
			strLine += strLine == constants.EMPTY_STRING ? constants.EMPTY_STRING : ",";

			//取得した値にエスケープ処理を行う
			value = escapeJSONValue(value);

			//一行後ごとのデータを文字列に追加する
			strLine += "\"" + sColName + "\":\"" + value + "\"";
		}
		//行単位の文字列が空白でないならカンマで区切る
		strBlock += strBlock == constants.EMPTY_STRING ? constants.EMPTY_STRING : ",";
		//列ごとの文字列をブロックで囲んで追加する
		strBlock += "{" + strLine + "}";
	}
	//作った全ブロックを配列のかっこで囲む
	strAll = "[" + strBlock + "]";
	
	//作った全体の文字列を返却する
	return strAll;
}


/*
関数名:outputJSON
概要:DBから取得したレコードでJSONを作る　使われていない
引数:string jsonString: クライアントから受け取ったJSON文字列
	:string key　JSONのトップノードのキー
返却値:なし
作成者:成田修之
作成日:8月15日(火)
*/
void JSONDBManager::outputJSON(string jsonString, string key)
{
	//
	this->getJSONMap(jsonString);
	//
	try {
		//DBと接続する
		this->connect();
		//null渡しをどうするか検討
		this->createJson(json, key,nullptr);
		//DBとの接続を切る
		this->disconnect();
	}//SQL例外発生
	catch (exception &e) {
		cout << e.what();
		return;
	}
}

/*
関数名:getJSONMap
概要:JSON文字列から連想配列のJSONを作成する
引数:string jsonString 変換したいjson文字列
返却値:なし
作成者:成田修之
作成日:8月15日(火)
*/
void JSONDBManager::getJSONMap(string jsonString)
{
	//picojsonでパースするためのオブジェクトを生成
	picojson::value pico;
	//作ったオブジェクトにjson文字列をパースして格納
	picojson::parse(pico, jsonString);

	//パースした中から連想配列を取り出す
	this->json = pico.get<picojson::object>();
}

/*
関数名:checkColumn
概要:引数keyで指定した列名を持つ列があるかどうかチェックする
引数:sql::ResultSet* rs　チェック対象の結果セット
	:string key　取り出したい列名
返却値:あるかどうかを真理値で返す
作成者:成田修之
作成日:8月15日(火)
*/
bool JSONDBManager::checkColumn(sql::ResultSet * rs, string columnName)
{
	//判定を格納する変数
	int boolRet = 0;

	//次がない（一時的なnullチェック）自分が投げたクエリの一行しかない
	if (rs->next()) {
		rs->previous(); // 元の位置に戻す
		
		//引数の結果セットのメタデータを取得する
		sql::ResultSetMetaData *rsMeta = rs->getMetaData();
		//列数を取得
		int columnCount = rsMeta->getColumnCount();
		//列数分繰り返す
		for (int i = 0; i < columnCount; i++) {
			//メタデータから列名を取得して探している列名と一致する
			if (rsMeta->getColumnLabel(i + 1) == columnName) {
				//trueをあらわす整数を返却する真理値判定の値に格納
				boolRet = 1;
				//見つかったのでループを抜ける
				break;
			}
		}
	}
	//見つかって１が格納されているかを真理値を返す
	return boolRet == 1;
}

string JSONDBManager::getListJSONPlusKey(picojson::object & json, string key)
{
	//引数のJSONから配列型のJSON文字列を取得
	string retArray = this->getListJSON(json);
	//パースするためのpicojsonオブジェクト作成
	picojson::value pico;
	//取得したJSON文字列をパース
	picojson::parse(pico, retArray);
	//パースしたオブジェクトを引数のjsonのkey部に追加
	json[key] = pico;

	//追加したjson連想配列をまたエンコード処理で文字列にする
	retArray = encodeJSONArray(json);

	//JSON文字列を返却する
	return retArray;
}

#include <boost/uuid/sha1.hpp>
#include <boost/cstdint.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <sstream>

typedef boost::array<boost::uint8_t, 20> hash_data_t;

/*
関数名:get_sha1_hash
概要:SHA1を実現するために必要な関数　ハッシュ値を返却する
参考:http://nukesaq88.hatenablog.com/entry/2013/04/19/183424
引数:void data　変換したデータ
	:size_t byte_count　バイト数
返却値:boost::array<boost::uint8_t,20> hash_data_t 20バイト固定のデータ
*/
hash_data_t
get_sha1_hash(const void *data, const std::size_t byte_count)
{
	boost::uuids::detail::sha1 sha1;
	sha1.process_bytes(data, byte_count);
	unsigned int digest[5];
	sha1.get_digest(digest);
	const boost::uint8_t *p_digest = reinterpret_cast<const boost::uint8_t *>(digest);
	hash_data_t hash_data;
	for (int i = 0; i < 5; ++i)
	{
		hash_data[i * 4] = p_digest[i * 4 + 3];
		hash_data[i * 4 + 1] = p_digest[i * 4 + 2];
		hash_data[i * 4 + 2] = p_digest[i * 4 + 1];
		hash_data[i * 4 + 3] = p_digest[i * 4];
	}
	return hash_data;
}


/*
関数名:encryptionSHA1
概要:引数で受け取った文字列をSHA1で暗号化して返却する
参考:http://nukesaq88.hatenablog.com/entry/2013/04/19/183424
引数:string str 暗号化したい文字列
返却値: string hashString　暗号化した文字列
作成者:成田修之
作成日:8月16日(水)
*/
std::string JSONDBManager::encryptionSHA1(std::string key)
{
	//get_sha1_hash関数を使うためkeyをchar型に変換
	char* buff = new char[key.length() + 1];
	//バッファにkeyの文字列を格納
	strcpy(buff, key.c_str());
	//ハッシュ値を獲得
	hash_data_t hash = get_sha1_hash(buff, 6);

	//暗号化した文字列をストリームで受けるための変数
	stringstream ss;
	//取得したハッシュ値をはじめから終わりまで1バイトずつ繰り返す
	for (hash_data_t::const_iterator itr = hash.begin(); itr != hash.end(); ++itr)
	{
		//SHA1による暗号化した文字列を16進数で受け取る
		ss << hex << ((*itr & 0xf0) >> 4)
			<< hex << (*itr & 0x0f);
	}
	//受け取った文字列を返却する文字列に格納
	string hashstring = ss.str();

	//暗号化した文字列を返す
	return hashstring;
}


/*
関数名:ReplaceAll
概要:引数1からすべての引数2を引数3に変換する
参考:http://www.geocities.jp/eneces_jupiter_jp/cpp1/010-055.html
引数:string str1 変換したい文字列
	:string str2 str1から上書きしたい文字列
	:string str3 str2を上書きする文字列
返却値:str1 返還後の文字列
*/
std::string ReplaceAll (std::string String1, std::string String2, std::string String3)
{
	std::string::size_type  Pos(String1.find(String2));

	while (Pos != std::string::npos)
	{
		String1.replace(Pos, String2.length(), String3);
		Pos = String1.find(String2, Pos + String3.length());
	}

	return String1;
}

/*
関数名:escapeSQLValue
概要:引数のクエリ文字列に対してSQL用のエスケープ処理を施す
引数:string str エスケープ処理をしたいクエリ文字列
返却値:string retStr　エスケープ処理を施した文字列を返却する
作成者:成田修之
作成日:8月15日(火)
*/
string JSONDBManager::escapeSQLValue(string str)
{
	string retStr = str;
	ReplaceAll(retStr, "\\", "\\\\");
	ReplaceAll(retStr, "\r\n", "\\n");
	ReplaceAll(retStr, "\r", "\\n");
	ReplaceAll(retStr, "\n", "\\n");
	ReplaceAll(retStr, "\"", "\\\"");
	return retStr;
}

/*
関数名:escapeJSONValue
概要:引数のJSON文字列にエスケープ処理を施す
引数:JSON文字列
返却値:string retStr エスケープ処理を施した文字列を返却する
作成者:成田修之
作成日:8月15日(火)
*/
string JSONDBManager::escapeJSONValue(string str)
{
	string retStr = str;
	ReplaceAll(retStr, "\\", "\\\\");
	ReplaceAll(retStr, "'", "\\'");
	return retStr;
}


/*
関数名:getOutHTMLString
概要:クライアントに出力用の文字列を返却する
引数:なし
返却値:string outHTMLSting　クライアントに出力する文字列を返す
作成者:成田修之
作成日:8月15日(火)
*/
string JSONDBManager::getJsonValue(picojson::object & map, string jsonKey, string valueKey)
{
	//一時オブジェクトを生成して引数のマップから第二引数のキーのオブジェクトを取得
	picojson::object &tempObject = map[jsonKey].get<picojson::object>();
	//取得したそのオブジェクト内のvalueを含んだ文字列を取得
	string retStr = tempObject[valueKey].get<string>();

	//取得した文字列を返す
	return retStr;
}

/*
関数名:getOutHTMLString
概要:クライアントに出力用の文字列を返却する
引数:なし
返却値:string outHTMLSting　クライアントに出力する文字列を返す
作成者:成田修之
作成日:8月15日(火)
*/
string JSONDBManager::getOutHTMLStrring()
{
	return this->outHTMLString;
}

/*
関数名:setOutHTMLString
概要:クライアントに出力用の文字列に引数の文字列を格納
引数:string outHTMLString セットする文字列
返却値:なし
作成者:成田修之
作成日:8月15日(火)
*/
void JSONDBManager::setOutHTMLString(string outHTMLString)
{
	this->outHTMLString = outHTMLString;
}

/*
関数名:encodeJSON
概要:jsonの連想配列をエンコード処理する関数
	※配列かマップかの区別はしない
引数:picojson::object json　エンコードしたいJSON連想配列
返却値:エンコード処理を施した文字列を返却する
作成者:
作成日:
*/
string JSONDBManager::encodeJSON(picojson::object json) {
	string strJson;

	//valueからでないとエンコードできないので
	//mapから直接エンコードできないので
	for (picojson::object::iterator i = json.begin(); i != json.end(); i++) {
		//ブロックが空でない
		if (strJson != "") {
			//カンマで区切れるようにする
			strJson += ",";
		}
		//ブロックごとにキーとvalueでJSONオブジェクトの形にする
		strJson +=  "\"" + i->first + "\":\"" + json[i->first].serialize() + "\"";
	}

	//できたエンコード処理を施した文字列を返却する
	return strJson;
}

/*
関数名:encodeJSONMap
概要:連想配列のJSONマップ形式にエンコードする
引数:picojson::object json　エンコードしたい連想配列のJSON
返却値:string strJsonMap エンコード処理を施したJSON文字列
作成者:成田修之
作成日:8月16日(水)
*/
string JSONDBManager::encodeJSONMap(picojson::object json) {
	//
	string strJsonMap = "{" + encodeJSON(json) + "}";
	//
	return strJsonMap;
}

/*
関数名:encodeJSONArray
概要:連想配列のJSONに配列状にエンコードする
引数:picojson::object json　エンコードしたい連想配列のJSON
返却値:string strJsonMap エンコード処理を施したJSON文字列
作成者:成田修之
作成日:8月16日(水)
*/
string JSONDBManager::encodeJSONArray(picojson::object json) {
	string strJsonArray = "[" + encodeJSON(json) + "]";
	return strJsonArray;
}