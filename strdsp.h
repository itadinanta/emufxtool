#ifndef __STRDSP_H
#define __STRDSP_H
#include <string>
using namespace std;
typedef basic_string < char >emuString;
typedef emuString patchString;

emuString flagsToString(unsigned long flags, const char *FLAG_NAMES[]);
emuString numToString(int X);

#define c_str data
#define chars data

#endif
