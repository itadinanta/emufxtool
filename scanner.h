// ---------------------------------------------------------------------------
#ifndef parser_H
#define parser_H
// ---------------------------------------------------------------------------
#include "config.h"
#include <string>
#include <stdio.h>
#include <list>
using namespace std;
#include <strdsp.h>

class ParserStream {
	bool ReadNewChar;
	char LastChar;
      protected:
	int LineNumber;
	char NewLineChar;
	virtual char ReadChar() = 0;
	virtual void Rewind() = 0;
      public:
	  ParserStream():LineNumber(1), NewLineChar('\n'), ReadNewChar(true) {
	};
	virtual ~ ParserStream() {
	};
	virtual char GetChar() {
		if (ReadNewChar) {
			if (Eof() || Bad())
				return 0;
			LastChar = ReadChar();
			if (LastChar == NewLineChar)
				NewLine();
		}
		ReadNewChar = true;
		return LastChar;
	}
	virtual int Restart() {
		LastChar = 0;
		ReadNewChar = true;
		Rewind();
	}
	virtual bool Eof() = 0;
	virtual bool Bad() = 0;
	virtual emuString GetName() {
		return "stream";
	};
	void NewLine() {
		LineNumber++;
	};
	void FirstLine() {
		LineNumber = 1;
	};
	int GetLine() {
		return LineNumber;
	};
	void SetNewLineChar(char c) {
		NewLineChar = c;
	};
	void PushBackChar() {
		ReadNewChar = false;
	};
};

class StringParserStream:public ParserStream {
	int charPtr;
	emuString Data;
	emuString Name;
      public:
	  StringParserStream(const emuString & AS, const emuString & StreamName) {
		Data = AS;
		Name = StreamName;
		charPtr = 0;
	} virtual char ReadChar() {
		return Data[charPtr++];
	}
	virtual bool Eof() {
		return ((unsigned) charPtr == Data.length());
	}
	virtual bool Bad() {
		return 0;
	}
	virtual void Rewind() {
		charPtr = 0;
	}
	virtual emuString GetName() {
		return Name;
	};
};

class FstrucParserStream:public ParserStream {
	int charPtr;
	FILE *Data;
      public:
	  FstrucParserStream(FILE * f):Data(f) {
	};
	virtual char ReadChar() {
		fgetc(Data);
	}
	virtual bool Eof() {
		return feof(Data);
	}
	virtual void Rewind() {
		rewind(Data);
	}
	virtual bool Bad() {
		return !Data;
	}
	virtual emuString GetName() {
		char data[16];
		sprintf(data, "0x%08x", (unsigned long) Data);
		return emuString(data);
	}
};

class FileParserStream:public FstrucParserStream {
	emuString Filename;
      public:
	  FileParserStream(const char
			   *fname):FstrucParserStream(fopen(fname, "rt")),
		Filename(fname) {
	};
	~FileParserStream() {
	}
	virtual emuString GetName() {
		return Filename;
	}
};

#define TOKEN_UNKNOWN -1
#define TOKEN_STRING -2
// 0x.*, +n, -n, integer
#define TOKEN_INT -3
// +n.m, -n.m
#define TOKEN_FLOAT -4
#define TOKEN_EOF -5
#define TOKEN_NULL -5
#define TOKEN_OP -6
// &time
#define TOKEN_NUMBER -7
#define TOKEN_IDENTIFIER -8

class TokenMap;
class Scanner {
      public:
	static emuString inttos(int L);
	emuString escape(const emuString &);
	static emuString literal(const emuString & STR);
	  Scanner();
	  virtual ~ Scanner();
	void AddSeparator(char sep, char second = 0);
	void AddBlank(char blank);
	int BeginParsing(ParserStream * P);
	int EndParsing();
	int Empty();
	void AddToken(const emuString & symbol, int token);
	void SetStringDelimiters(char, char);
	inline int GetNextToken() {
		return Token(GetNextWord());
	};
	bool Eof();
	virtual emuString GetNextWord();
	emuString GetLastString();
	emuString GetLastWord() {
		return LastWord;
	};
	void PushBackWord() {
		ReadNewWord = false;
	};
	void PushBackChar() {
		if (PStream)
			PStream->PushBackChar();
	};
	int GetLineNumber() {
		if (PStream)
			return PStream->GetLine();
		else
			return 0;
	};
	emuString GetStreamName();

	ParserStream *FindStream(const emuString & name);
      protected:
	virtual int Token(const emuString & Word) const;
	virtual char GetNextChar();
	bool IsSeparator(char, char second = 0);
	bool BeginsSeparator(char);
	bool IsBlank(char);
	virtual bool IsIdentifier(const emuString & Word) const;
	emuString GetSymbol(int token);
      protected:
	emuString LastWord;
	bool ReadNewWord;
      private:
	short int NSep;
	unsigned short int Separators[1024];
	short int NBlanks;
	char Blanks[256];
	TokenMap *Map;
	char LastChar;
	ParserStream *PStream;
	list < ParserStream * >streams;
	char StringDelimiterBegin;
	char StringDelimiterEnd;
};

#endif
