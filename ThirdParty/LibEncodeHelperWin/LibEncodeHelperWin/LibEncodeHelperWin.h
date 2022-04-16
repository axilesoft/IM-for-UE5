/**********************
* Copy From :
* @muzun
* http://bebolog.blogspot.jp/2014/11/c.html
************************/
#pragma once

#include <iostream>
#include <string>
#include <algorithm>    // std::transform

// 使い方:
//   std::string utf8_str;  // UTF-8でエンコードされた文字
//   std::string sjis_str = convert_encoding(utf8_str, "utf-8", "shift-jis");

class LibEncodeHelperWin
{
public:
	LibEncodeHelperWin(){}
	~LibEncodeHelperWin(){}

	//windows only used
	int string_to_codepage(std::string code_str, unsigned int * OutCode);

	std::string convert_encoding(const std::string &str, const char *fromcode, const char *tocode);
};
