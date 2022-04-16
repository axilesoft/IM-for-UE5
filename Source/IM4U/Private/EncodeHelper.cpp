
#include "EncodeHelper.h"
#include "IM4UPrivatePCH.h"

std::string EncodeHelper::convert_encoding(const std::string &str, const char *fromcode, const char *tocode)
{
#ifdef _WIN32
	return libEncodeHelperWin.convert_encoding(str,fromcode, tocode);
#else
	char *outstr, *instr;
	iconv_t icd;
	size_t instr_len = std::strlen(str.c_str());
	size_t outstr_len = instr_len * 2;

	if (instr_len <= 0) return "";

	// allocate memory
	instr = new char[instr_len + 1];
	outstr = new char[outstr_len + 1];
	strcpy(instr, str.c_str());
	icd = iconv_open(tocode, fromcode);
	if (icd == (iconv_t)-1) {
		return "Failed to open iconv (" + std::string(fromcode) + " to " + std::string(tocode) + ")";
	}
	char *src_pos = instr, *dst_pos = outstr;
	if (iconv(icd, &src_pos, &instr_len, &dst_pos, &outstr_len) == -1) {
		// return error message
		std::string errstr;
		int err = errno;
		if (err == E2BIG) {
			errstr = "There is not sufficient room at *outbuf";
		}
		else if (err == EILSEQ) {
			errstr = "An invalid multibyte sequence has been encountered in the input";
		}
		else if (err == EINVAL) {
			errstr = "An incomplete multibyte sequence has been encountered in the input";
		}
		iconv_close(icd);
		return "Failed to convert string (" + errstr + ")";
	}
	*dst_pos = '\0';
	iconv_close(icd);

	std::string s(outstr);
	delete[] instr;
	delete[] outstr;

	return s;
#endif
}
