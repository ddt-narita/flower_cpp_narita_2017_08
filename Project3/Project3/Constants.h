#pragma once

#ifndef __Constants
#define __Constants

#include <string>

using namespace std;

class Constants {
public:
	// DB接続に使用するドライバーのクラス名称
	const string JDBC_DRIVER = "org.mariadb.jdbc.Driver";
	// 接続するデータベースのアドレス
	const string DSN = "jdbc:mariadb://localhost/ddthink-com00006";
	// 接続するユーザー名
	const string DB_USER = "root";
	// 接続するパスワード
	const string DB_PASSWORD = "";
	//JSONのdb_getQueryキーの文字列
	const string DB_GETQUERY = "db_getQuery";
	//JSONのdb_setQueryキーの文字列
	const string DB_SETQUERY = "db_setQuery";
	//JSONのdb_columnキーの文字列
	const string DB_COLUMN = "db_column";
	//JSONのtextキーの文字列
	const string KEY_TEXT = "text";
	//JSONのhtmlキーの文字列
	const string KEY_HTML = "html";
	//JSONのsrcキーの文字列
	const string KEY_SRC = "src";
	//JSONのvalueキーの文字列
	const string KEY_VALUE = "value";
	//アンダーバー二つの文字列
	const string  STR_TWO_UNDERBAR = "__";
	//会員番号列の定数
	const string COLUMN_NAME_USER_KEY = "user_key";
	// JSONのvalueキーの文字列
	const string STR_TABLE_DATA = "tableData";
	// 空文字を示す定数
	const string EMPTY_STRING = "";
	// クッキーの有効期限
	const int COOKIE_EXPIRATION = 60 * 60 * 24;
	// クッキー、セッション共通キーuserId(ユーザID)の定数
	const string USER_ID = "userId";
	// クッキー、セッション共通キーauthority(ユーザ権限)の定数
	const string AUTHORITY = "authority";
	// クッキー、セッション共通キーpageAuth(ページ権限)の定数
	const string PAGE_AUTH = "pageAuth";
	// jspのセッションクッキー名称
	const string JSP_SESSION_COOKIE_NAME = "JSESSIONID";
	// セッションの有効期限（秒）
	static const int SESSION_EXPIRATION_TIME = 60 * 24;
	// passwordを示す文字列
	const string STR_PASSWORD = "password";
	// idを示す文字列
	const string STR_ID = "id";
	// JsonのLogin判別キーuserName(ユーザ名)の定数
	const string USER_NAME = "userName";
	//返却用JSON文字列、前
	const string ERROR_JSON_FRONT = "{\"createTagState\":\"";
	//返却用JSON文字列、後
	const string ERROR_JSON_BACK = "\"}";
	//クライアントから取得した置換文字が配列の場合の区切り文字
	const string ARRAY_DELIMITER = "','";

	//画像UPLOADで使用する定数
	//送信されたファイルのname属性の値を定数に入れる
	const string SEND_IMAGE_NAME = "imageFile";
	//POSTされたディレクトリ名のキー
	const string DIRECTORY_KEY = "dir";
	//POSTされたinput type="file"要素のname属性値
	const string POSTED_NAME = "postedName";
	//デフォルトの保存先
	const string SAVE_DIRECTORY = "/uploadImage/flowerImage/";
	//画像の保存先(表示に使用)
	const string IMAGE_DIRECTORY = "uploadImage/flowerImage/";
	//画像保存成功時のメッセージ
	const string SUCCESS_UPLOAD_MESSAGE = "画像の保存に成功しました。";
	//画像保存失敗時のメッセージ
	const string FAILED_UPLOAD_MESSAGE = "画像の保存に失敗しました。";
	//無効なファイルが送信されたときのメッセージ
	const string INVALID_FILE_SEND_MESSAGE = "無効なファイルが送信されました。";
	//無効なファイルが送信されたときのメッセージ
	const string NOT_EXIST_FILE_MESSAGE = "ファイルがありません";
	//ファイルタイプが画像である事を示す文字列
	const string FILE_TYPE_IMAGE = "image";
	//受信したデータのキー文字列
	const string GET_IMAGE_KEY = "photo";
	//データフォーマットを示す定数 (年月日時分秒を数値で出力)
	const string DATETIME_FROMAT_STRING = "yyyyMMddHHmmss";

	//記事番号をセッションに入れるためのキー文字列 ブログ作成、編集に使用する
	const string KEY_NUMBER = "number";
	//ブログ作成、編集に使用する、文字が数値に変換できたかを示すためのオブジェクトキー
	const string KEY_SUCCESS = "success";
	//ブログ作成、編集に使用する、ブログ編集画面のHTMLを示すURL
	const string URL_CREATEARTICLE_HTML = "/window/member/page/createarticle.html";

	static const int IS_LOGIN = 1;

	static const int IS_NOT_LOGIN = 0;

};

#endif