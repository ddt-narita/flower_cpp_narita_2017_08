#pragma once
#ifndef __GetParameter
#define __GetParameter

#include <string>

std::string UrlDecorde(const std::string & str);
bool isLF(const std::string & str,
	const std::string::size_type & length,
	std::string::size_type & index,
	char* tmpStr);
char hexToChar(char first, char second);
std::string getParameter(std::string key);


#endif // !__GetParameter
