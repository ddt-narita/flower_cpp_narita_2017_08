#pragma once
#ifndef __account
#define __account

#include "JSONDBManager.h"

class account :public JSONDBManager {
public:
	picojson::object pageAuthorityCheck;
private:



public:
	void init();
	void login(std::string jsonString);
	void logout();
	bool loginCheck();
	void pageCheck(std::string strAuthority);

};



#endif // !__account
