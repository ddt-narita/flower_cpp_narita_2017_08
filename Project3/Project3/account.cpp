#include "account.h"
#include "Constants.h"

Constants constantsAccount;

/*
関数名:
概要:
引数:
返却値:
作成者:成田修之
作成日:8月16日(水)
*/
void account::init()
{
	connect();
}

/*
関数名:
概要:
引数:
返却値:
作成者:成田修之
作成日:8月16日(水)
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
関数名:
概要:
引数:
返却値:
作成者:成田修之
作成日:8月16日(水)
*/
void account::logout()
{

}

/*
関数名:
概要:
引数:
返却値:
作成者:成田修之
作成日:8月16日(水)
*/
bool account::loginCheck()
{
	return false;
}

/*
関数名:
概要:
引数:
返却値:
作成者:成田修之
作成日:8月16日(水)
*/
void account::pageCheck(std::string strAuthority)
{
}
