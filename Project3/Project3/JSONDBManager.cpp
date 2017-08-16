

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
�֐���:createJson
�T�v:DB����擾�����f�[�^�ɂ��Ă�Json���쐬����
����:picojson::object json
	:string key
	:Db_ResultTree
�ԋp�l:�Ȃ�
�쐬��:���c�C�V
�쐬��:8��15��(��)
*/
void JSONDBManager::createJson(picojson::object &json, std::string key, Db_ResultTree * dbrt_parent)
{
	//DB�̌��ʂ���c���[���\�z����N���X�̃C���X�^���X����
	Db_ResultTree *db_resultTree = new Db_ResultTree();
	
	//DB�̌��ʂ��c���[�̌��ʕ��Ɋi�[
	*(db_resultTree->db_result) =  executeQuery(json, key);
	//�����̐e�m�[�h���i�[
	db_resultTree->parent = dbrt_parent;
	//���݂�json���i�[
	db_resultTree->json = json;
	//���݂�key���i�[
	db_resultTree->keyData = key;


	//DB����̌��ʂ̒�����key�ɓ�������̂��擾
	string column = getDBColumn(key, db_resultTree);
	
	//json�̍Ő[�w���܂ł��ׂđ���
	for (picojson::object::iterator i = json.begin(); i != json.end(); i++) {
		//json��key���擾
		string keyString = i->first;
		//value���q�I�u�W�F�N�g�̎��i�܂��l�X�g���Ă���Ƃ��j
		if (i->second.is<picojson::object>()) {
			//�q�I�u�W�F�N�g�ɂ��čċA����
			createJson(json[i->first].get<picojson::object>(), keyString, db_resultTree);
		}
		//�J������������Ǝ擾�ł��A���L�[�̖��O��text,html,src�̂����ꂩ�ł���
		else if (column != "" && (keyString == constants.KEY_TEXT
			|| keyString == constants.KEY_HTML || keyString == constants.KEY_SRC)) {
			
			//���̃L�[�̒l���J�����ɒu������
			json[keyString].get<string>() = column;
		}
	}



}

/*
�֐���:executeQuery
�T�v:�N�G�������s���ADB���猋�ʃZ�b�g���擾
����:picojson::object& json�@�J�����g��JSON
	:string queryKey �N�G��������L�[
�ԋp�l:ResultSet *retRs DB����擾�������ʃZ�b�g��Ԃ�
�쐬��:���c�C�V
�쐬��:8��15��(��)
*/


sql::ResultSet& JSONDBManager::executeQuery(picojson::object &json, std::string queryKey)
{
	sql::ResultSet *retRs = nullptr;

	//���[�U����ی삷�邽�߃p�X���[�h��key�ɂ���΃n�b�V��������
	if (json.count(constants.STR_PASSWORD) != 0) {
		//�p�X���[�h�̂���}�b�v���擾
		picojson::object replaceJson = json[constants.STR_PASSWORD].get<picojson::object>();

		//�p�X���[�h��SHA1�ňÍ������Ēu������
		replaceJson[constants.KEY_VALUE].get<string>() = encryptionSHA1(replaceJson[constants.KEY_VALUE].get<string>());
		//������json�̃p�X���[�h�̂���}�b�v�ɂ��e��������
		json[constants.STR_PASSWORD].get<picojson::object>() = replaceJson;
	}
	//queryKey���L�[�Ɏ��m�[�h������
	if (json.count(queryKey) != 0) {
		//�N�G�������o��
		string query = json[queryKey].get<string>();
		//������ƃN�G�����擾�ł��Ă���
		if (query != "") {
			//�Ő[�w�܂ōċA�����Ő���
			for (picojson::object::iterator i = json.begin(); i != json.end(); i++) {
				//�q�������A�܂��l�X�g���Ă���Ƃ�
				if (i->second.is<picojson::object>()) {
					picojson::object childObject = json[i->first].get<picojson::object>();
					//�q�I�u�W�F�N�g��value�̃L�[�������Ă���
					if (childObject.count(constants.KEY_VALUE) != 0) {
						
						//�ϊ����ׂ���������擾
						string replaceValue = createReplaceValue(childObject[constants.KEY_VALUE]);
						//�N�G���̕ϊ��Ώۂ̕�������擾�����ϊ����ׂ�������ɒu������
						query = query.replace(query.find("'" + i->first + "'"), i->first.length() + 2, ("'" + replaceValue + "'"));
					}
				}
			}
		
			//�X�e�[�g�����g�쐬
			sql::Statement *stmt = con->createStatement();
			//�N�G�����e���R���\�[���ɕ\��
			cout << "query:" + query;
			//�N�G�������s
			retRs = stmt->executeQuery(query);
			//���ʃZ�b�g�̃J�[�\�������ʌ������擾���邽�߂ɍŌ�Ɏ����Ă���
			retRs->last();
			//���ʌ������擾
			this->processedRecords = retRs->getRow();
			//�擾�����̂ŃJ�[�\�����ŏ��ɖ߂�
			retRs->beforeFirst();
		}
	}
	//DB�ɃN�G���𓊂������ʃZ�b�g���擾����
	return *retRs;
}


/*
�֐���:createReplaceValue
�T�v:DB����擾�����f�[�^�ɂ��Ă�Json���쐬����
����:picojson::value& childObjectValue
�ԋp�l:string �쐬�����ԋp�p�̕�����
�쐬��:���c�C�V
�쐬��:8��16��(��)
*/
std::string JSONDBManager::createReplaceValue(picojson::value &childObjectValue)
{
	//�ԋp����u�����邽�߂̕�����
	string retReplaceString = "";
	//�f�[�^����邽�߂ɕ�������i�[����z��
	vector<string> childObjectArray;
	
	//�󂯎�����v�f���z��̎�
	if (childObjectValue.is<picojson::array>()) {
		//�z�񕔕����擾
		picojson::array tempChildObjectArray = childObjectValue.get<picojson::array>();
		//�z��̑S�v�f�i������j��ǉ�
		//childObjectArray.assign(tempChildObjectArray.begin(), tempChildObjectArray.end());
		for (picojson::array::iterator i = tempChildObjectArray.begin(); i != tempChildObjectArray.end(); i++) {
			childObjectArray.push_back(i->get<string>());
		}
	
	
	}//�z��ȊO�̎�
	else {
		//�������ǉ�����
		childObjectArray.push_back(childObjectValue.get<string>());
	}
	
	//������z��̑S�v�f���J��Ԃ�
	for (int i = 0; i < childObjectArray.size(); i++) {
		//�ԋp����u�����镶���񂪋󔒂Ȃ牽�������A�l������΋�؂蕶����ǉ�
		retReplaceString += retReplaceString.empty() ? "" : ",";
		//������ɑ΂��ăG�X�P�[�v�������{���Ēǉ����Ă���
		retReplaceString += escapeSQLValue(childObjectArray[i]);
	}

	//�쐬�����������ԋp����
	return retReplaceString;
}


/*
�֐���:getDBColumn
�T�v:DB����擾�����f�[�^�ɂ��Ă�Json���쐬����
�Q�l:http://qiita.com/iseki-masaya/items/70b4ee6e0877d12dafa8
	�R�����g���̃X�S���N�l�̃R�[�h���Q�l�Ƀf���~�^���ꕶ���łȂ��Ƃ��̂��̂��Ή������܂����B
����:string s ��؂肽���Ώە�����
	:string delim ��؂蕶���̎w��
�ԋp�l:string column �擾������̒l��ԋp
*/
vector<string> split(const string& s, string delim) {
	std::vector<string> elms;
	size_t offset = 0;
	while (true) {
		//�ŏ��ɂ���f���~�^�̈ʒu�擾
		size_t next = s.find_first_of(delim, offset);
		//�f���~�^�������Ȃ���
		if (next == string::npos) {
			//�c����x�N�g���̖����ɒǉ�
			elms.push_back(s.substr(offset));
			//�v���O�������I������
			return elms;
		}
		//�f���~�^�̑O�܂ł��x�N�g���̖����ɒǉ����Ă���
		elms.push_back(s.substr(offset, next - offset));
		//next + 1 ���@next + delim.size()�Ńf���~�^�̒����ɑΉ�
		offset = next + delim.size();
	}
}

/*
�֐���:getDBColumn
�T�v:DB����擾�����f�[�^�ɂ��Ă�Json���쐬����
����:string key JSON�I�u�W�F�N�g�̃L�[�@
	:Db_ResultTree dbrTree�@DB����擾�������ʂ��c���[�\���ɂ���N���X�̃C���X�^���X
�ԋp�l:string column �擾������̒l��ԋp
�쐬��:���c�C�V
�쐬��:8��15��(��)
*/
std::string JSONDBManager::getDBColumn(std::string key, Db_ResultTree *dbrTree)
{
	//�擾������̒l��ԋp����̂Ɏg���ϐ�
	string column = "";
	//�s�����i�[����ϐ�
	int columnNumber = 0;
	//�e�̃L�[���z��̗v�f�ł��邱�Ƃ������������܂�ł���Ƃ�
	if(dbrTree->parent->keyData.find(constants.STR_TWO_UNDERBAR) != string::npos){
		//�f���~�^�Ŕz��ɕ���
		vector<string> keySting = split(dbrTree->parent->keyData, constants.STR_TWO_UNDERBAR);
		//�s�����擾
		columnNumber = stoi(keySting[1]);
	}
	
	//�e�m�[�h�����Ȃ��Ȃ�܂ŌJ��Ԃ�
	while (dbrTree != nullptr) {
		//key�̗񖼂����񂪌��ʃZ�b�g�ɑ��݂���
		if (checkColumn(dbrTree->db_result, key)) {
			//���ʃZ�b�g�̃J�[�\����columnNumber�̈ʒu�ֈړ�
			if (dbrTree->db_result->absolute(columnNumber + 1)) {
				//�J�����̒l���擾
				column = dbrTree->db_result->getString(key);
			}//���[�v�𔲂���
			break;
		}//���ʃZ�b�g��key�̗񖼂̗�͑��݂��Ȃ�
		else {
			//�e�ɂ��Ē��ׂ���悤�ɐe���Z�b�g
			dbrTree = dbrTree->parent;
		}
	}//�擾������̒l��ԋp����
	return column;
}

/*
�֐���:getListJSON
�T�v:DB����擾�����f�[�^�ɂ��Ă�Json���쐬����
����:string key JSON�I�u�W�F�N�g�̃L�[�@
	:Db_ResultTree dbrTree�@DB����擾�������ʂ��c���[�\���ɂ���N���X�̃C���X�^���X
�ԋp�l:string column �擾������̒l��ԋp
�쐬��:���c�C�V
�쐬��:8��15��(��)
*/
string JSONDBManager::getListJSON(picojson::object & json)
{
	string strAll = "";		//Json�S�̂̕�������i�[����ϐ�
	string strBlock = "";	//�u���b�N���Ƃ̕�������i�[����ϐ�
	string strLine = "";	//��s���Ƃ̕�������i�[����ϐ�

	//DB�ɃN�G�����Ȃ����ʃZ�b�g���擾
	sql::ResultSet *rs = &executeQuery(json, constants.DB_GETQUERY);
	//�񐔂��擾���邽�߂̃��^�f�[�^�����
	sql::ResultSetMetaData *rsMeta = rs->getMetaData();
	//���^�f�[�^���猋�ʃZ�b�g�̗񐔂��擾
	int columnCount = rsMeta->getColumnCount();

	//���ʃZ�b�g�̍s�ɂ��Ă��ׂđ���
	while (rs->next()) {
		//��s���Ƃ̕�������N���A
		strLine = "";
		//�񐔕��J��Ԃ�
		for (int i = 0; i < columnCount; i++) {
			//��̖��O���擾����
			string sColName = rsMeta->getColumnLabel(i + 1);
			//��̒l���擾����
			string value = rs->getString(i + 1);
			//�擾�����l��NULL�Ȃ�󔒂��i�[
			value = &value == nullptr ? constants.EMPTY_STRING : value;
			//��s���Ƃ̕�����ɋ󔒂ł͂Ȃ��Ȃ�J���}�̋�؂��������
			strLine += strLine == constants.EMPTY_STRING ? constants.EMPTY_STRING : ",";

			//�擾�����l�ɃG�X�P�[�v�������s��
			value = escapeJSONValue(value);

			//��s�ゲ�Ƃ̃f�[�^�𕶎���ɒǉ�����
			strLine += "\"" + sColName + "\":\"" + value + "\"";
		}
		//�s�P�ʂ̕����񂪋󔒂łȂ��Ȃ�J���}�ŋ�؂�
		strBlock += strBlock == constants.EMPTY_STRING ? constants.EMPTY_STRING : ",";
		//�񂲂Ƃ̕�������u���b�N�ň͂�Œǉ�����
		strBlock += "{" + strLine + "}";
	}
	//������S�u���b�N��z��̂������ň͂�
	strAll = "[" + strBlock + "]";
	
	//������S�̂̕������ԋp����
	return strAll;
}


/*
�֐���:outputJSON
�T�v:DB����擾�������R�[�h��JSON�����@�g���Ă��Ȃ�
����:string jsonString: �N���C�A���g����󂯎����JSON������
	:string key�@JSON�̃g�b�v�m�[�h�̃L�[
�ԋp�l:�Ȃ�
�쐬��:���c�C�V
�쐬��:8��15��(��)
*/
void JSONDBManager::outputJSON(string jsonString, string key)
{
	//
	this->getJSONMap(jsonString);
	//
	try {
		//DB�Ɛڑ�����
		this->connect();
		//null�n�����ǂ����邩����
		this->createJson(json, key,nullptr);
		//DB�Ƃ̐ڑ���؂�
		this->disconnect();
	}//SQL��O����
	catch (exception &e) {
		cout << e.what();
		return;
	}
}

/*
�֐���:getJSONMap
�T�v:JSON�����񂩂�A�z�z���JSON���쐬����
����:string jsonString �ϊ�������json������
�ԋp�l:�Ȃ�
�쐬��:���c�C�V
�쐬��:8��15��(��)
*/
void JSONDBManager::getJSONMap(string jsonString)
{
	//picojson�Ńp�[�X���邽�߂̃I�u�W�F�N�g�𐶐�
	picojson::value pico;
	//������I�u�W�F�N�g��json��������p�[�X���Ċi�[
	picojson::parse(pico, jsonString);

	//�p�[�X����������A�z�z������o��
	this->json = pico.get<picojson::object>();
}

/*
�֐���:checkColumn
�T�v:����key�Ŏw�肵���񖼂����񂪂��邩�ǂ����`�F�b�N����
����:sql::ResultSet* rs�@�`�F�b�N�Ώۂ̌��ʃZ�b�g
	:string key�@���o��������
�ԋp�l:���邩�ǂ�����^���l�ŕԂ�
�쐬��:���c�C�V
�쐬��:8��15��(��)
*/
bool JSONDBManager::checkColumn(sql::ResultSet * rs, string columnName)
{
	//������i�[����ϐ�
	int boolRet = 0;

	//�����Ȃ��i�ꎞ�I��null�`�F�b�N�j�������������N�G���̈�s�����Ȃ�
	if (rs->next()) {
		rs->previous(); // ���̈ʒu�ɖ߂�
		
		//�����̌��ʃZ�b�g�̃��^�f�[�^���擾����
		sql::ResultSetMetaData *rsMeta = rs->getMetaData();
		//�񐔂��擾
		int columnCount = rsMeta->getColumnCount();
		//�񐔕��J��Ԃ�
		for (int i = 0; i < columnCount; i++) {
			//���^�f�[�^����񖼂��擾���ĒT���Ă���񖼂ƈ�v����
			if (rsMeta->getColumnLabel(i + 1) == columnName) {
				//true������킷������ԋp����^���l����̒l�Ɋi�[
				boolRet = 1;
				//���������̂Ń��[�v�𔲂���
				break;
			}
		}
	}
	//�������ĂP���i�[����Ă��邩��^���l��Ԃ�
	return boolRet == 1;
}

string JSONDBManager::getListJSONPlusKey(picojson::object & json, string key)
{
	//������JSON����z��^��JSON��������擾
	string retArray = this->getListJSON(json);
	//�p�[�X���邽�߂�picojson�I�u�W�F�N�g�쐬
	picojson::value pico;
	//�擾����JSON��������p�[�X
	picojson::parse(pico, retArray);
	//�p�[�X�����I�u�W�F�N�g��������json��key���ɒǉ�
	json[key] = pico;

	//�ǉ�����json�A�z�z����܂��G���R�[�h�����ŕ�����ɂ���
	retArray = encodeJSONArray(json);

	//JSON�������ԋp����
	return retArray;
}

#include <boost/uuid/sha1.hpp>
#include <boost/cstdint.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <sstream>

typedef boost::array<boost::uint8_t, 20> hash_data_t;

/*
�֐���:get_sha1_hash
�T�v:SHA1���������邽�߂ɕK�v�Ȋ֐��@�n�b�V���l��ԋp����
�Q�l:http://nukesaq88.hatenablog.com/entry/2013/04/19/183424
����:void data�@�ϊ������f�[�^
	:size_t byte_count�@�o�C�g��
�ԋp�l:boost::array<boost::uint8_t,20> hash_data_t 20�o�C�g�Œ�̃f�[�^
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
�֐���:encryptionSHA1
�T�v:�����Ŏ󂯎�����������SHA1�ňÍ������ĕԋp����
�Q�l:http://nukesaq88.hatenablog.com/entry/2013/04/19/183424
����:string str �Í���������������
�ԋp�l: string hashString�@�Í�������������
�쐬��:���c�C�V
�쐬��:8��16��(��)
*/
std::string JSONDBManager::encryptionSHA1(std::string key)
{
	//get_sha1_hash�֐����g������key��char�^�ɕϊ�
	char* buff = new char[key.length() + 1];
	//�o�b�t�@��key�̕�������i�[
	strcpy(buff, key.c_str());
	//�n�b�V���l���l��
	hash_data_t hash = get_sha1_hash(buff, 6);

	//�Í���������������X�g���[���Ŏ󂯂邽�߂̕ϐ�
	stringstream ss;
	//�擾�����n�b�V���l���͂��߂���I���܂�1�o�C�g���J��Ԃ�
	for (hash_data_t::const_iterator itr = hash.begin(); itr != hash.end(); ++itr)
	{
		//SHA1�ɂ��Í��������������16�i���Ŏ󂯎��
		ss << hex << ((*itr & 0xf0) >> 4)
			<< hex << (*itr & 0x0f);
	}
	//�󂯎�����������ԋp���镶����Ɋi�[
	string hashstring = ss.str();

	//�Í��������������Ԃ�
	return hashstring;
}


/*
�֐���:ReplaceAll
�T�v:����1���炷�ׂĂ̈���2������3�ɕϊ�����
�Q�l:http://www.geocities.jp/eneces_jupiter_jp/cpp1/010-055.html
����:string str1 �ϊ�������������
	:string str2 str1����㏑��������������
	:string str3 str2���㏑�����镶����
�ԋp�l:str1 �ԊҌ�̕�����
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
�֐���:escapeSQLValue
�T�v:�����̃N�G��������ɑ΂���SQL�p�̃G�X�P�[�v�������{��
����:string str �G�X�P�[�v�������������N�G��������
�ԋp�l:string retStr�@�G�X�P�[�v�������{�����������ԋp����
�쐬��:���c�C�V
�쐬��:8��15��(��)
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
�֐���:escapeJSONValue
�T�v:������JSON������ɃG�X�P�[�v�������{��
����:JSON������
�ԋp�l:string retStr �G�X�P�[�v�������{�����������ԋp����
�쐬��:���c�C�V
�쐬��:8��15��(��)
*/
string JSONDBManager::escapeJSONValue(string str)
{
	string retStr = str;
	ReplaceAll(retStr, "\\", "\\\\");
	ReplaceAll(retStr, "'", "\\'");
	return retStr;
}


/*
�֐���:getOutHTMLString
�T�v:�N���C�A���g�ɏo�͗p�̕������ԋp����
����:�Ȃ�
�ԋp�l:string outHTMLSting�@�N���C�A���g�ɏo�͂��镶�����Ԃ�
�쐬��:���c�C�V
�쐬��:8��15��(��)
*/
string JSONDBManager::getJsonValue(picojson::object & map, string jsonKey, string valueKey)
{
	//�ꎞ�I�u�W�F�N�g�𐶐����Ĉ����̃}�b�v����������̃L�[�̃I�u�W�F�N�g���擾
	picojson::object &tempObject = map[jsonKey].get<picojson::object>();
	//�擾�������̃I�u�W�F�N�g����value���܂񂾕�������擾
	string retStr = tempObject[valueKey].get<string>();

	//�擾�����������Ԃ�
	return retStr;
}

/*
�֐���:getOutHTMLString
�T�v:�N���C�A���g�ɏo�͗p�̕������ԋp����
����:�Ȃ�
�ԋp�l:string outHTMLSting�@�N���C�A���g�ɏo�͂��镶�����Ԃ�
�쐬��:���c�C�V
�쐬��:8��15��(��)
*/
string JSONDBManager::getOutHTMLStrring()
{
	return this->outHTMLString;
}

/*
�֐���:setOutHTMLString
�T�v:�N���C�A���g�ɏo�͗p�̕�����Ɉ����̕�������i�[
����:string outHTMLString �Z�b�g���镶����
�ԋp�l:�Ȃ�
�쐬��:���c�C�V
�쐬��:8��15��(��)
*/
void JSONDBManager::setOutHTMLString(string outHTMLString)
{
	this->outHTMLString = outHTMLString;
}

/*
�֐���:encodeJSON
�T�v:json�̘A�z�z����G���R�[�h��������֐�
	���z�񂩃}�b�v���̋�ʂ͂��Ȃ�
����:picojson::object json�@�G���R�[�h������JSON�A�z�z��
�ԋp�l:�G���R�[�h�������{�����������ԋp����
�쐬��:
�쐬��:
*/
string JSONDBManager::encodeJSON(picojson::object json) {
	string strJson;

	//value����łȂ��ƃG���R�[�h�ł��Ȃ��̂�
	//map���璼�ڃG���R�[�h�ł��Ȃ��̂�
	for (picojson::object::iterator i = json.begin(); i != json.end(); i++) {
		//�u���b�N����łȂ�
		if (strJson != "") {
			//�J���}�ŋ�؂��悤�ɂ���
			strJson += ",";
		}
		//�u���b�N���ƂɃL�[��value��JSON�I�u�W�F�N�g�̌`�ɂ���
		strJson +=  "\"" + i->first + "\":\"" + json[i->first].serialize() + "\"";
	}

	//�ł����G���R�[�h�������{�����������ԋp����
	return strJson;
}

/*
�֐���:encodeJSONMap
�T�v:�A�z�z���JSON�}�b�v�`���ɃG���R�[�h����
����:picojson::object json�@�G���R�[�h�������A�z�z���JSON
�ԋp�l:string strJsonMap �G���R�[�h�������{����JSON������
�쐬��:���c�C�V
�쐬��:8��16��(��)
*/
string JSONDBManager::encodeJSONMap(picojson::object json) {
	//
	string strJsonMap = "{" + encodeJSON(json) + "}";
	//
	return strJsonMap;
}

/*
�֐���:encodeJSONArray
�T�v:�A�z�z���JSON�ɔz���ɃG���R�[�h����
����:picojson::object json�@�G���R�[�h�������A�z�z���JSON
�ԋp�l:string strJsonMap �G���R�[�h�������{����JSON������
�쐬��:���c�C�V
�쐬��:8��16��(��)
*/
string JSONDBManager::encodeJSONArray(picojson::object json) {
	string strJsonArray = "[" + encodeJSON(json) + "]";
	return strJsonArray;
}