#pragma once

#ifndef __Constants
#define __Constants

#include <string>

using namespace std;

class Constants {
public:
	// DB�ڑ��Ɏg�p����h���C�o�[�̃N���X����
	const string JDBC_DRIVER = "org.mariadb.jdbc.Driver";
	// �ڑ�����f�[�^�x�[�X�̃A�h���X
	const string DSN = "jdbc:mariadb://localhost/ddthink-com00006";
	// �ڑ����郆�[�U�[��
	const string DB_USER = "root";
	// �ڑ�����p�X���[�h
	const string DB_PASSWORD = "";
	//JSON��db_getQuery�L�[�̕�����
	const string DB_GETQUERY = "db_getQuery";
	//JSON��db_setQuery�L�[�̕�����
	const string DB_SETQUERY = "db_setQuery";
	//JSON��db_column�L�[�̕�����
	const string DB_COLUMN = "db_column";
	//JSON��text�L�[�̕�����
	const string KEY_TEXT = "text";
	//JSON��html�L�[�̕�����
	const string KEY_HTML = "html";
	//JSON��src�L�[�̕�����
	const string KEY_SRC = "src";
	//JSON��value�L�[�̕�����
	const string KEY_VALUE = "value";
	//�A���_�[�o�[��̕�����
	const string  STR_TWO_UNDERBAR = "__";
	//����ԍ���̒萔
	const string COLUMN_NAME_USER_KEY = "user_key";
	// JSON��value�L�[�̕�����
	const string STR_TABLE_DATA = "tableData";
	// �󕶎��������萔
	const string EMPTY_STRING = "";
	// �N�b�L�[�̗L������
	const int COOKIE_EXPIRATION = 60 * 60 * 24;
	// �N�b�L�[�A�Z�b�V�������ʃL�[userId(���[�UID)�̒萔
	const string USER_ID = "userId";
	// �N�b�L�[�A�Z�b�V�������ʃL�[authority(���[�U����)�̒萔
	const string AUTHORITY = "authority";
	// �N�b�L�[�A�Z�b�V�������ʃL�[pageAuth(�y�[�W����)�̒萔
	const string PAGE_AUTH = "pageAuth";
	// jsp�̃Z�b�V�����N�b�L�[����
	const string JSP_SESSION_COOKIE_NAME = "JSESSIONID";
	// �Z�b�V�����̗L�������i�b�j
	static const int SESSION_EXPIRATION_TIME = 60 * 24;
	// password������������
	const string STR_PASSWORD = "password";
	// id������������
	const string STR_ID = "id";
	// Json��Login���ʃL�[userName(���[�U��)�̒萔
	const string USER_NAME = "userName";
	//�ԋp�pJSON������A�O
	const string ERROR_JSON_FRONT = "{\"createTagState\":\"";
	//�ԋp�pJSON������A��
	const string ERROR_JSON_BACK = "\"}";
	//�N���C�A���g����擾�����u���������z��̏ꍇ�̋�؂蕶��
	const string ARRAY_DELIMITER = "','";

	//�摜UPLOAD�Ŏg�p����萔
	//���M���ꂽ�t�@�C����name�����̒l��萔�ɓ����
	const string SEND_IMAGE_NAME = "imageFile";
	//POST���ꂽ�f�B���N�g�����̃L�[
	const string DIRECTORY_KEY = "dir";
	//POST���ꂽinput type="file"�v�f��name�����l
	const string POSTED_NAME = "postedName";
	//�f�t�H���g�̕ۑ���
	const string SAVE_DIRECTORY = "/uploadImage/flowerImage/";
	//�摜�̕ۑ���(�\���Ɏg�p)
	const string IMAGE_DIRECTORY = "uploadImage/flowerImage/";
	//�摜�ۑ��������̃��b�Z�[�W
	const string SUCCESS_UPLOAD_MESSAGE = "�摜�̕ۑ��ɐ������܂����B";
	//�摜�ۑ����s���̃��b�Z�[�W
	const string FAILED_UPLOAD_MESSAGE = "�摜�̕ۑ��Ɏ��s���܂����B";
	//�����ȃt�@�C�������M���ꂽ�Ƃ��̃��b�Z�[�W
	const string INVALID_FILE_SEND_MESSAGE = "�����ȃt�@�C�������M����܂����B";
	//�����ȃt�@�C�������M���ꂽ�Ƃ��̃��b�Z�[�W
	const string NOT_EXIST_FILE_MESSAGE = "�t�@�C��������܂���";
	//�t�@�C���^�C�v���摜�ł��鎖������������
	const string FILE_TYPE_IMAGE = "image";
	//��M�����f�[�^�̃L�[������
	const string GET_IMAGE_KEY = "photo";
	//�f�[�^�t�H�[�}�b�g�������萔 (�N���������b�𐔒l�ŏo��)
	const string DATETIME_FROMAT_STRING = "yyyyMMddHHmmss";

	//�L���ԍ����Z�b�V�����ɓ���邽�߂̃L�[������ �u���O�쐬�A�ҏW�Ɏg�p����
	const string KEY_NUMBER = "number";
	//�u���O�쐬�A�ҏW�Ɏg�p����A���������l�ɕϊ��ł��������������߂̃I�u�W�F�N�g�L�[
	const string KEY_SUCCESS = "success";
	//�u���O�쐬�A�ҏW�Ɏg�p����A�u���O�ҏW��ʂ�HTML������URL
	const string URL_CREATEARTICLE_HTML = "/window/member/page/createarticle.html";

	static const int IS_LOGIN = 1;

	static const int IS_NOT_LOGIN = 0;

};

#endif