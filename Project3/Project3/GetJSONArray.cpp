#include "Constants.h"
#include "procedureGetList.h"
#include "LoginCheck.h"
#include "getParameter.h"
#include <new>

using namespace std;

int main() {
	string outValue = "";
	
	//パラメータからjsonデータを取得する
	string json = getParameter("json");

	procedureGetList listJsonDbGetter;

	try {
		listJsonDbGetter.run(json);
		outValue = listJsonDbGetter.getOutHTMLStrring();
	}
	catch (LoginCheckException & e) {
		outValue = constants1.ERROR_JSON_FRONT + to_string(e.checkLoginState()) + constants1.ERROR_JSON_BACK;
	}
	catch (std::exception &e) {
		
	}

	const char* answer = "Content-Type:text/html;charset=utf-8\n\n";

	cout << answer;

	cout << outValue;
}