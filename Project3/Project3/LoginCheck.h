#pragma once
#ifndef __LoginCheckException
#define __LoginCheckException

#include <exception>
#include <new>
#include <string>

/*
クラス名:LoginCheckException
概要:ログインチェックエラー時の例外クラス
作成者:成田修之
作成日:8月16日(水)
*/
class LoginCheckException :private std::exception {
private:
	//エラー発生時のクッキーの値を保持するメンバ文字列
	std::string cookieValue;
	//エラー発生時のセッションの値を保持するメンバ文字列
	std::string sessionValue;

public:
	//コンストラクタ
	LoginCheckException(std::string cookiValue, std::string sessionValue) {
		//引数のクッキーの文字列をメンバに保持
		this->cookieValue = cookieValue;
		//引数のセッションの文字列をメンバに保持
		this->sessionValue = sessionValue;
	}

	/*
	関数名:checkLoginState
	概要:ログイン状態を調べて数値で返す
	作成者:成田修之
	作成日:
	*/
	int checkLoginState() {
		//返却するログインの状態を表す変数
		int retState = 0;

		//クッキーとセッションがきちんと取得できているとき
		if (cookieValue != "", sessionValue != "") {
			//ログイン状態であることを示す1を代入
			retState = 1;
		}

		//ログインの状態を返却する
		return retState;
	}
};


#endif // !__LoginCheckException
