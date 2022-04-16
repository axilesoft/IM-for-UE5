
#include "LibEncodeHelperWin.h"


#include <windows.h>

int LibEncodeHelperWin::string_to_codepage(std::string code_str, unsigned int * OutCode)
{
	std::transform(code_str.begin(), code_str.end(), code_str.begin(), ::tolower);

	if (code_str == "shift_jis" || code_str == "shift-jis") {
		*OutCode = CP_ACP;
		return 0;
	}

	if (code_str == "utf-8") {
		*OutCode = CP_UTF8;
		return 0;
	}
	return -1;
}

std::string LibEncodeHelperWin::convert_encoding(const std::string &str, const char *fromcode, const char *tocode)
{
	unsigned int from_code = 0;
	unsigned int to_code = 0;
	if ((string_to_codepage(std::string(fromcode),&from_code) != 0) ||
		(string_to_codepage(std::string(tocode),&to_code) != 0))
	{
		// ParamErr
		return std::string();
	}

	WCHAR *utf16str;
	char *outstr;
	int rtn_len;

	// utf-16に変換
	int utf16str_len = MultiByteToWideChar(from_code, 0, str.c_str(), -1, NULL, 0);
	utf16str = new WCHAR[utf16str_len + 1];
	rtn_len = MultiByteToWideChar(from_code, 0, str.c_str(), -1, utf16str, utf16str_len);
	utf16str[rtn_len] = L'\0';

	// utf-16から任意の文字コードに変換
	int outstr_len = WideCharToMultiByte(to_code, 0, utf16str, -1, NULL, 0, NULL, NULL);
	outstr = new char[outstr_len + 1];
	rtn_len = WideCharToMultiByte(to_code, 0, utf16str, -1, outstr, outstr_len, NULL, NULL);
	outstr[rtn_len] = '\0';

	std::string s(outstr);
	delete[] utf16str;
	delete[] outstr;

	return s; 
}
