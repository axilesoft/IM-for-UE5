/**********************
* Copy From :
* @muzun
* http://bebolog.blogspot.jp/2014/11/c.html
************************/
#pragma once

#include <iostream>
#include <string>
#include <algorithm>    // std::transform

#ifdef _WIN32
#include <LibEncodeHelperWin.h>
#else
#include <iconv.h>
#endif

// 使い方:
//   std::string utf8_str;  // UTF-8でエンコードされた文字
//   std::string sjis_str = convert_encoding(utf8_str, "utf-8", "shift-jis");

class EncodeHelper
{
public:
	EncodeHelper(){}
	~EncodeHelper(){}

#ifdef _WIN32
	//windows only used
	LibEncodeHelperWin libEncodeHelperWin;
#endif

	std::string convert_encoding(const std::string &str, const char *fromcode, const char *tocode);
};
static EncodeHelper encodeHelper;