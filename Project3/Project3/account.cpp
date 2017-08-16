#include "account.h"
#include "Constants.h"

Constants constantsAccount;

/*
ŠÖ”–¼:
ŠT—v:
ˆø”:
•Ô‹p’l:
ì¬Ò:¬“cC”V
ì¬“ú:8Œ16“ú(…)
*/
void account::init()
{
	connect();
}

/*
ŠÖ”–¼:
ŠT—v:
ˆø”:
•Ô‹p’l:
ì¬Ò:¬“cC”V
ì¬“ú:8Œ16“ú(…)
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
ŠÖ”–¼:
ŠT—v:
ˆø”:
•Ô‹p’l:
ì¬Ò:¬“cC”V
ì¬“ú:8Œ16“ú(…)
*/
void account::logout()
{

}

/*
ŠÖ”–¼:
ŠT—v:
ˆø”:
•Ô‹p’l:
ì¬Ò:¬“cC”V
ì¬“ú:8Œ16“ú(…)
*/
bool account::loginCheck()
{
	return false;
}

/*
ŠÖ”–¼:
ŠT—v:
ˆø”:
•Ô‹p’l:
ì¬Ò:¬“cC”V
ì¬“ú:8Œ16“ú(…)
*/
void account::pageCheck(std::string strAuthority)
{
}
