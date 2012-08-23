#ifndef _EMU_EXCEPTION_H
#define _EMU_EXECPTION_H
#include "config.h"
#include <strdsp.h>
#include <string>
#include <fstream>
#include <iostream>
typedef emuString eString;
class emuException {
	eString Message;
	eString FileName;
	int LineNumber;
	void copy(const emuException & E) {
		Message = E.Message;
		LineNumber = E.LineNumber;
		FileName = E.FileName;
      } public:
	  emuException(const emuException & E) {
		copy(E);
	}
	const emuException & operator =(const emuException & E) {
		copy(E);
		return *this;
	}
      emuException(eString symbol, eString message, eString filename = "", int line = 0):LineNumber(line), FileName(filename),
		Message("'" + symbol + "' " +
			message) {
		cout << filename << "\n\tat line " << line << ":" << "'" +
			symbol + "' " + message << '\n';
	};
	emuString message() {

		return FileName + ":" + /* eString(LineNumber) */ +":" +
			Message;
	};
	int line() {
		return LineNumber;
	};
};

#endif
