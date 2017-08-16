#include "account.h"
#include "Constants.h"

Constants constantsAccount;

/*
�֐���:
�T�v:
����:
�ԋp�l:
�쐬��:���c�C�V
�쐬��:8��16��(��)
*/
void account::init()
{
	connect();
}

/*
�֐���:
�T�v:
����:
�ԋp�l:
�쐬��:���c�C�V
�쐬��:8��16��(��)
*/
void account::login(std::string jsonString)
{
	getJSONMap(jsonString);

	createJson(json, constantsAccount.EMPTY_STRING, nullptr);

	string jsonOut = encodeJSONMap(this->json);

	string userID = getJsonValue(json, constantsAccount.STR_ID, constantsAccount.KEY_TEXT);
	string authority = getJsonValue(json, constantsAccount.AUTHORITY, constantsAccount.KEY_TEXT);

	setOutHTMLString(jsonOut);
}

/*
�֐���:
�T�v:
����:
�ԋp�l:
�쐬��:���c�C�V
�쐬��:8��16��(��)
*/
void account::logout()
{

}

/*
�֐���:
�T�v:
����:
�ԋp�l:
�쐬��:���c�C�V
�쐬��:8��16��(��)
*/
bool account::loginCheck()
{
	return false;
}

/*
�֐���:
�T�v:
����:
�ԋp�l:
�쐬��:���c�C�V
�쐬��:8��16��(��)
*/
void account::pageCheck(std::string strAuthority)
{
}
