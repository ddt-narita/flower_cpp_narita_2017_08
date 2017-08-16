#pragma once

#ifndef __DbConnect
#define __DbConnect

#include  <iostream>
#include  <sstream>
#include  <memory>
#include  <mysql_connection.h>

class DbConnect {
public:
	sql::Connection *con;

	void connect();
	void disconnect();
};


#endif __DbConnect