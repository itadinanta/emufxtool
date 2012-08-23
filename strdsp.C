#include <strdsp.h>
emuString flagsToString(unsigned long flags, const char **bitnames)
{
	int first = 1;
	emuString S;
	while (bitnames && *bitnames && flags) {
		if (flags & 1) {
			if (!first)
				S += '|';
			S += *bitnames;
			first = 0;
		}
		flags >>= 1;
		bitnames++;
	}
	return S;
}

emuString numToString(int X)
{
	static char BUF[64];
	sprintf(BUF, "%d", X);
	return emuString(BUF);
}
