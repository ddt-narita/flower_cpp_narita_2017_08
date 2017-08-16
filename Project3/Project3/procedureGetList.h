#pragma once
#ifndef __ProcedureGetList
#define __ProcedureGetList

#include "procedureBase.h"
#include "Constants.h"

Constants constants1;
class procedureGetList :public procedureBase {
public:
	void init() {
		procedureBase::init();
	}

	void job(std::string jsonString) {
		procedureBase::init();
		std::string retArrayString = "";
		try {
			retArrayString = getListJSONPlusKey(this->json, constants1.STR_TABLE_DATA);
		}
		catch (sql::SQLException &e) {
			e.what();
			return;
		}
		disconnect();
		setOutHTMLString(retArrayString);
	}


	void run(std::string jsonString) {
		this->init();
		this->job(jsonString);
	}
};






#endif // !__ProcedureGetList
