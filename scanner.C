// ---------------------------------------------------------------------------
#include <ctype.h>
#include <scanner.h>
#include <map>
#include <excep.h>
#include <stdio.h>

class TokenMap:public map < emuString, int, less < emuString > > {
      public:
	TokenMap() {
	};
	~TokenMap() {
	};
	void AddToken(const emuString & key, int tok) {
		insert(value_type(key, tok));
	};
	int GetToken(const emuString & key) {
		if (find(key) == end())
			return TOKEN_UNKNOWN;
		else
			return (*this)[key];
	};
	emuString GetSymbol(int token) {
		for (iterator i = begin(); i != end(); i++)
			if (i->second == token)
				return i->first;
		return "";
	}
};

// ---------------------------------------------------------------------------

emuString Scanner::inttos(int L)
{
	char c[20];
	sprintf(c, "%li", L);
	return emuString(c);
};

emuString Scanner::escape(const emuString & SS)
{
	emuString result;
	emuString SS2;
	if (SS[0] == StringDelimiterBegin
	    && SS[SS.length() - 1] == StringDelimiterEnd)
		SS2 = SS.substr(1, SS.length() - 2);
	else
		SS2 = SS;
	const char *S = SS2.chars();

	while (*S) {
		switch (*S) {
		case '\x0a':
			result += "\\n";
			break;
		case '\t':
			result += "\\t";
			break;
		default:
			if (*S > 31)
				result += *S;
			else {
				char hexc[10];
				sprintf(hexc, "\\x%02x",
					(int) (unsigned char) *S);
				result += hexc;
			}
		}
		S++;
	}
	return result;
}

void Scanner::AddToken(const emuString & symbol, int token)
{
	Map->AddToken(symbol, token);
};

Scanner::Scanner()
{
	*Separators = 0;
	*Blanks = 0;
	NSep = 0;
	NBlanks = 0;
	Map = new TokenMap();
	PStream = 0;
	ReadNewWord = true;
	SetStringDelimiters('\"', '\"');
};

Scanner::~Scanner()
{
	delete Map;
};

#define MAKE_SEP(a,b) (unsigned short)(((unsigned short)(unsigned char)(a)) | (((unsigned short)(unsigned char)(b))<<8))

bool Scanner::IsSeparator(char sep, char second)
{
	unsigned short int nsep = MAKE_SEP(sep, second);
	unsigned short int *CSep = Separators;
	while (*CSep)
		if (*CSep++ == nsep)
			return true;
	return false;
};

bool Scanner::BeginsSeparator(char sep)
{
	unsigned short int *CSep = Separators;
	while (*CSep)
		if ((char) (*CSep++ & 0xff) == sep)
			return true;
	return false;
};

bool Scanner::IsBlank(char blank)
{
	char *CBlank = Blanks;
	while (*CBlank)
		if (*CBlank++ == blank)
			return true;
	return false;
};

void Scanner::AddSeparator(char sep, char second)
{
	if (!IsSeparator(sep, second)) {
		unsigned short int nsep = MAKE_SEP(sep, second);
		Separators[NSep] = nsep;
		NSep++;
		Separators[NSep] = 0;
	};
};
void Scanner::AddBlank(char blank)
{
	if (!IsBlank(blank)) {
		Blanks[NBlanks++] = blank;
		Blanks[NBlanks] = 0;
	};
};

char Scanner::GetNextChar()
{
	if (PStream)
		LastChar = PStream->GetChar();
	return LastChar;
};

emuString Scanner::GetNextWord()
{
	char c;
	if (ReadNewWord) {
		LastWord = "";
		do {
			c = GetNextChar();
		} while (c && IsBlank(c));
		if (!c) {

		}
		else if (c == StringDelimiterBegin) {
			LastWord += c;
			c = GetNextChar();
			while (c && c != StringDelimiterEnd) {
				LastWord += c;
				c = GetNextChar();
			};
			if (c)
				LastWord += c;
		}
		else if (BeginsSeparator(c)) {
			LastWord += c;
			char c2 = GetNextChar();
			if (c2) {
				if (IsSeparator(c, c2))
					LastWord += c2;
				else
					PushBackChar();	// ReadNewChar=false;
			};
		}
		else
			while (c) {
				if (BeginsSeparator(c) || IsBlank(c)) {
					PushBackChar();
					// ReadNewChar=false;
					break;
				};
				LastWord += c;
				c = GetNextChar();
			};
	}
	else
		ReadNewWord = true;
	// cout << "-----------------------[ "<<LastWord<<" ]\n";
	return LastWord;
};

emuString Scanner::GetStreamName()
{
	emuString Acc;
	for (list < ParserStream * >::iterator i = streams.begin();
	     i != streams.end(); i++)
		if (*i)
			Acc += (*i)->GetName() + " at line " +
				inttos((*i)->GetLine()) + ":";
	if (PStream)
		Acc += PStream->GetName();
	return Acc;
}

int Scanner::BeginParsing(ParserStream * Stream)
{
	int i;
#ifdef DEBUG
	printf("\n");
	for (i = 0; i < streams.size(); i++)
		printf("    ");
	printf("%s", Stream->GetName().chars());
	printf(" {\n");
#endif
	if (streams.size() > 64) {
		throw emuException("<PARSER>",
				   " too many recursion levels (>64)",
				   GetStreamName(), GetLineNumber());
	}
	streams.push_back(PStream);
	PStream = Stream;
	return streams.size();
};

ParserStream *Scanner::FindStream(const emuString & name)
{
	for (list < ParserStream * >::iterator i = streams.begin();
	     i != streams.end(); i++)
		if ((*i) && name == (*i)->GetName())
			return *i;

	return 0;
}

int Scanner::EndParsing()
{
	int i;
#ifdef DEBUG
	printf("\n");
	for (i = 0; i < streams.size() - 1; i++)
		printf("    ");
	printf("}\n");
#endif
	if (PStream)
		delete PStream;

	if (streams.size()) {
		PStream = streams.back();
		streams.pop_back();
	}
	else {
		PStream = 0;
	}
	return PStream != 0;
};

int Scanner::Empty()
{
	return PStream == 0;
}

emuString Scanner::GetLastString()
{
	return GetLastWord();
	// emuString w(GetLastWord());
	// if (Token(w)==TOKEN_STRING) return w.at(1,w.length()-2);
	// else return w;
};

bool Scanner::IsIdentifier(const emuString & Symbol) const {
	return 0;
} 

int Scanner::Token(const emuString & Symbol) const {
	long i;
	double d;
	if (Symbol.empty())
		return TOKEN_NULL;
	if (Symbol[0] == '\xff' || Symbol[0] == '\0'
	    || Symbol[0] == '\x1a')
		return TOKEN_EOF;
	else if (Symbol[0] == StringDelimiterBegin
		 && Symbol[Symbol.length() - 1] == StringDelimiterEnd)
		return TOKEN_STRING;
	i = Map->GetToken(Symbol);
	if (i != TOKEN_UNKNOWN)
		return i;
	else if (!strchr(Symbol.c_str(), '.')
		 && sscanf(Symbol.c_str(), "%li", &i))
		return TOKEN_INT;
	else if (sscanf(Symbol.c_str(), "%lf", &d))
		return TOKEN_FLOAT;
	else if (IsIdentifier(Symbol))
		return TOKEN_IDENTIFIER;
	else
		return TOKEN_UNKNOWN;
};

void Scanner::SetStringDelimiters(char b, char e)
{
	StringDelimiterBegin = b;
	StringDelimiterEnd = e;
};

bool Scanner::Eof()
{
	return PStream ? PStream->Eof() : 1;
};

emuString Scanner::GetSymbol(int token)
{
	switch (token) {
	case TOKEN_INT:
		return "<integer value>";
	case TOKEN_FLOAT:
		return "<number>";
	case TOKEN_IDENTIFIER:
		return "<identifier>";
	}
	if (Map)
		return Map->GetSymbol(token);
	else
		return "<special>";
}

static inline short hexcnv(char c)
{
	c = toupper(c);
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

emuString Scanner::literal(const emuString & in)
{
	const char *S = in.chars();
	if (*S != '"')
		return in;

	emuString Value;
	char *value = (char *) malloc(in.length());
	char *D = value;
	char LastChar;
	short h1,
		h2;
	S++;			// Skips first "

	while (*S) {
		LastChar = *S++;
		if (LastChar == '\"')
			break;
		if (LastChar == '\\') {
			LastChar = *S++;
			switch (LastChar) {
			case 't':
				*D++ = '\t';
				break;
			case 'n':
				*D++ = '\n';
				break;
			case 'r':
				*D++ = '\r';
				break;
			case 'q':
				*D++ = '\"';
				break;
			case '\\':
				*D++ = '\\';
				break;
			case '0':
				*D++ = '\x00';
				break;
			case 'x':
				h1 = hexcnv(*S);
				h2 = hexcnv(S[1]);
				if (h1 >= 0 && h2 >= 0) {
					S += 2;
					*D++ = (char) ((h1 << 4) | h2);
				}
				else
					*D++ = 'x';
				break;
			default:
				*D++ = LastChar;
			};
		}
		else
			*D++ = LastChar;

	};
	*D = 0;
	Value += value;
	free(value);
	return Value;
};
