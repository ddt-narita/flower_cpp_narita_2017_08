#pragma once
#ifndef __ProcedureBase
#define __ProcedureBase

#include <string>
#include "account.h"

class procedureBase :public account {
public:
	void init() {
		account::init();
		loginCheck();
	}
	void job(std::string jsonString) {
		getJSONMap(jsonString);
	}
	void run() {
		this->init();
		this->job(nullptr);
	}
};



#endif // !__ProcedureBase
