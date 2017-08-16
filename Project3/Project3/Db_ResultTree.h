#pragma once


#include "picojson.h"
#include "cppconn/resultset.h"

class Db_ResultTree {
public:
	Db_ResultTree *parent = nullptr;		//���̃m�[�h�i�C���X�^���X�j�̐e
	picojson::object json;	//JSON�f�[�^�̘A�z�z��
	std::string keyData = "";				//�����o��json�L�[
	sql::ResultSet *db_result = nullptr;			//DB�̌��ʃZ�b�g

	/*��Ԑe�̃m�[�h���ǂ������ׂ邽�߂̕ϐ�
	int topNode;

	bool is_top() {
		return topNode == 0;
	}
	*/
};