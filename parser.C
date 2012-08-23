#include <stack>
#include <list>
#include <ctype.h>
#include <parser.h>
#include <instruction.h>
#include <patch.h>

using namespace std;

#define DEBUG(str) cerr << str

#define BLANK " "
enum emuTokens {
	// Punteggiatura
	tInstructionEnd,
	tListSeparator,
	tOpenParen,
	tClosedParen,
	tOpenBrace,
	tClosedBrace,
	tOpenSquare,
	tClosedSquare,
	tBeginComment,
	tEndComment,
	tColon,
	tSemicolon,
	tEol,

	// OpCodes
	tMACs,
	tMACns,
	tMACw,
	tMACnw,
	tMACINTs,
	tMACINTw,
	tACC3,
	tMACMV,
	tANDXOR,
	tTSTNEG,
	tLIMITge,
	tLIMITl,
	tLOG,
	tEXP,
	tINTERP,
	tSKIP,
	// Direttive
	tFor,
	tIf,
	tElse,
	tMacro,
	tCode,
	tInclude,
	tDefine,
	tCurrentLine,
	tCurrentFile,
	tDefined,
	tUndefined,
	tEval,
	// initializers and trams
	tPatch,
	tExport,
	tPrivate,
	tVar,
	tConst,
	tFxBus,
	tExtIn,
	tExtOut,
	tTram,
	tLine,
	tDelay,
	tLookup,
	tOffset,
	tLength,
	tOnOff,
	tMono,
	tStereo,
	tRange,
	tRangeInterval,
	tMin,
	tMax,
	tRead,
	tWrite,
	tSpec,
	tAbsolute,
	tInput,
	tOutput,
	tControl,
	tWarn,
	tErr,
	// Prefissi
	tDollar,
	tSharp,
	tAt,
	tRegister,
	// Operatori infissi
	tPlus,
	tMinus,
	tUnaryMinus,
	tTimes,
	tMod,
	tDivide,
	tXor,
	tLShift,
	tRShift,
	tEqual,
	tGreater,
	tGreaterEqual,
	tLessEqual,
	tLess,
	tNotEqual,
	tAnd,
	tOr,
	tNot,
	tBitwiseAnd,
	tBitwiseOr,
	tBitwiseNot,
	tPower,
	tAddress,
	tAssign
};

const emuTokens emuFirstOper = tPlus;
const emuTokens emuLastOper = tBitwiseNot;

const emuTokens emuFirstOp = tMACs;
const emuTokens emuLastOp = tSKIP;

emuSymbolTable::emuSymbolTable()
{
};
emuSymbolTable::~emuSymbolTable()
{
	cleanup();
};

void emuSymbolTable::set(const emuString & key, emuElement * E)
{
	iterator found = find(key);
	if (found != end()) {
		if (found->second)
			delete found->second;
		found->second = E;
	}
	else
		insert(value_type(key, E));
};

emuElement *emuSymbolTable::get(const emuString & key)
{
	iterator found = find(key);
	if (found == end())
		return 0;
	else
		return found->second;
};

int emuSymbolTable::save(FILE * f)
{
	if (f) {
		int j;
		for (iterator i = begin(); i != end(); i++) {
			if ((*i->second).getFlags(V_ADDRESS)) {
				emuInitializer *S =
					(emuInitializer *) & (*i->second);
				fprintf(f, "0x%03x\t%s\n", S->getAddress(),
					i->first.data());
				if (S->getFlags(V_ARRAY)) {
					for (j = 0; j < S->Length(); j++) {
						fprintf(f, "0x%03x\t%s[0x%x]\n",
							S->getAddress() + j,
							i->first.data(), j);
					}
				}
			}
		}
	}
}

int emuSymbolTable::cleanup()
{
	int count = 0;
	for (iterator i = begin(); i != end(); i++)
		if (i->second) {
			count++;
			delete i->second;
		}
	clear();
	return count;
};

emuString emuSymbolTable::GetSymbol(emuElement * E)
{
	for (iterator i = begin(); i != end(); i++)
		if (i->second == E)
			return i->first;
	return "";
};

emuMacroTable::emuMacroTable()
{
};
emuMacroTable::~emuMacroTable()
{
	int count = 0;
	for (iterator i = begin(); i != end(); i++)
		if (i->second) {
			count++;
			delete i->second;
		}
	clear();
};

void emuMacroTable::set(const emuString & key, emuMacro * E)
{
	iterator found = find(key);
	if (found != end()) {
		if (found->second)
			delete found->second;
		found->second = E;
	}
	else
		insert(value_type(key, E));
};

emuMacro *emuMacroTable::get(const emuString & key)
{
	iterator found = find(key);
	if (found == end())
		return 0;
	else
		return found->second;
};

emuParser::emuParser()
{
	AddBlank(' ');
	AddBlank('\t');
	AddBlank('\x0d');
	AddBlank('\x0a');

	AddSeparator(';');
	AddSeparator(',');
	AddSeparator('{');
	AddSeparator('}');
	AddSeparator('(');
	AddSeparator(')');
	AddSeparator('[');
	AddSeparator(']');
	AddSeparator('>');
	AddSeparator('<');
	AddSeparator('>', '=');
	AddSeparator('>', '>');
	AddSeparator('=', '>');
	AddSeparator('<', '=');
	AddSeparator('<', '<');
	AddSeparator('/', '*');
	AddSeparator('*', '/');
	AddSeparator('/', '/');
	AddSeparator('=', '=');
	AddSeparator('<', '>');
	AddSeparator('!', '=');
	AddSeparator('&', '&');
	AddSeparator('|', '|');
	AddSeparator('&');
	AddSeparator('|');
	AddSeparator('~');
	AddSeparator('-', '>');
	AddSeparator('!');
	AddSeparator('=');
	AddSeparator(':');
	AddSeparator('^');
	AddSeparator('-');
	AddSeparator('+');
	AddSeparator('#', '#');
	AddSeparator('*');
	AddSeparator('/');
	AddSeparator('%');
	AddSeparator('$');
	AddSeparator('@');
	AddSeparator('#');
	// Punteggiatura
	// AddToken("\x0a",tInstructionEnd);
	AddToken(";", tInstructionEnd);

	AddToken("MACS", tMACs);
	AddToken("MACS1", tMACns);
	AddToken("MACNS", tMACns);
	AddToken("MACW", tMACw);
	AddToken("MACW1", tMACnw);
	AddToken("MACNW", tMACnw);
	AddToken("MAC0", tMACs);
	AddToken("MAC1", tMACns);
	AddToken("MAC2", tMACw);
	AddToken("MAC3", tMACnw);

	AddToken("MACINT", tMACINTs);
	AddToken("MACINT0", tMACINTs);
	AddToken("MACINTS", tMACINTs);
	AddToken("MACINTW", tMACINTw);
	AddToken("MACINT1", tMACINTw);

	AddToken("ACC3", tACC3);
	AddToken("MACMV", tMACMV);
	AddToken("ANDXOR", tANDXOR);
	AddToken("TSTNEG", tTSTNEG);
	AddToken("LIMIT", tLIMITge);
	AddToken("LIMIT0", tLIMITge);
	AddToken("LIMITGE", tLIMITge);
	AddToken("LIMITL", tLIMITl);
	AddToken("LIMIT1", tLIMITl);
	AddToken("LOG", tLOG);
	AddToken("EXP", tEXP);
	AddToken("INTERP", tINTERP);
	AddToken("SKIP", tSKIP);

	AddToken("\x0d", tInstructionEnd);
	AddToken(",", tListSeparator);
	AddToken("(", tOpenParen);
	AddToken(")", tClosedParen);
	AddToken("{", tOpenBrace);
	AddToken("}", tClosedBrace);
	AddToken("[", tOpenSquare);
	AddToken("]", tClosedSquare);
	AddToken("/" "*", tBeginComment);
	AddToken("*" "/", tEndComment);
	AddToken(":", tColon);
	AddToken("->", tColon);
	// AddToken(";",tSemicolon);

	// Direttive
	AddToken(".for", tFor);
	AddToken(".if", tIf);
	AddToken(".else", tElse);
	AddToken(".eval", tEval);
	AddToken(".macro", tDefine);
	AddToken(".include", tInclude);
	AddToken(".defined", tDefined);
	AddToken(".undefined", tUndefined);
	AddToken(".define", tDefine);
	AddToken(".line", tCurrentLine);
	AddToken(".file", tCurrentFile);
	AddToken(".warn", tWarn);
	AddToken(".err", tErr);

	// controls
	AddToken("control", tControl);
	AddToken("onoff", tOnOff);
	AddToken("mono", tMono);
	AddToken("stereo", tStereo);
	AddToken("range", tRange);
	AddToken("to", tRangeInterval);
	// AddToken("min",tMin);
	// AddToken("max",tMax);
	// tram lines
	AddToken("line", tLine);
	AddToken("tram", tTram);
	AddToken("delay", tDelay);
	AddToken("lookup", tLookup);
	AddToken("read", tRead);
	AddToken("write", tWrite);
	// AddToken("offset",tOffset);
	// AddToken("length",tLength);
	// initializers
	AddToken("var", tVar);
	AddToken("const", tConst);
	AddToken("fxbus", tFxBus);
	AddToken("extin", tExtIn);
	AddToken("extout", tExtOut);

	// attributes
	AddToken("absolute", tAbsolute);
	AddToken("fixed", tAbsolute);
	AddToken("special", tSpec);
	AddToken("system", tSpec);
	AddToken("export", tExport);
	AddToken("public", tExport);
	AddToken("private", tPrivate);
	AddToken("+", tPlus);
	AddToken("-", tMinus);
	AddToken("<", tLess);
	AddToken("==", tEqual);
	AddToken(">", tGreater);
	AddToken("<", tLessEqual);
	AddToken("==", tEqual);
	AddToken(">", tGreaterEqual);
	AddToken("!=", tNotEqual);
	AddToken("<>", tNotEqual);
	AddToken("&&", tAnd);
	AddToken("||", tOr);
	AddToken("!", tNot);
	AddToken("\xfe", tUnaryMinus);
	AddToken("*", tTimes);
	AddToken("/", tDivide);
	AddToken("%", tMod);
	AddToken("^", tXor);
	AddToken("<<", tLShift);
	AddToken(">>", tRShift);

	AddToken("#", tSharp);
	AddToken("$", tDollar);
	AddToken("@", tAt);
	AddToken("?", tRegister);
	AddToken("&", tBitwiseAnd);
	AddToken("|", tBitwiseOr);
	AddToken("~", tBitwiseNot);
	AddToken("=>", tAddress);
	AddToken("at", tAddress);
	AddToken("=", tAssign);
	Options = 0;
	SYMTAB = new emuSymbolTable();
	SYMLIST = new list < emuElement * >();
	MTAB = new emuMacroTable();
	ILIST = new emuInstructionList();
	PROPS = new emuInitializerPropertyList();
	CurrentTram = 0;
};

int emuParser::SaveSymbols(const char *filename)
{
	FILE *f = fopen(filename, "wt");
	if (f) {
		if (SYMTAB)
			SYMTAB->save(f);
		fclose(f);
		return 1;
	}
	return 0;
}

bool emuParser::IsIdentifier(const emuString & Word) const {
	const char *c = Word.chars();
	char C = toupper(*c);
	if ((C < 'A' || C > 'Z') && C != '_')
		return false;
	*c++;
	while (*c)
	{
		C = toupper(*c);
		if ((C < 'A' || C > 'Z') && (C < '0' || C > '9') && (C != '_') && (C != '.'))
			return false;
		c++;
	} return true;
}

emuParser::~emuParser()
{
	delete SYMTAB;
	delete SYMLIST;
	delete MTAB;
	delete ILIST;
	delete PROPS;
};

#define THROW_ERR(msg) {throw emuException(GetLastWord(),msg,GetStreamName(),GetLineNumber());}
#define THROW_ERR_T(t) {throw emuException(GetLastWord(),"found where '"+GetSymbol(t)+"' expected",GetStreamName(),GetLineNumber());}
#define THROW_ERR_S(sym,msg) {throw emuException(sym,msg,GetStreamName(),GetLineNumber());}
#define EXPECT(t) {if ((Token=GetNextToken())!=t) THROW_ERR_T(t)};
#define EXPECTC(t,C) {if ((Token=GetNextToken())!=t) {{C;};THROW_ERR_T(t);}}

int emuParser::ErrorState()
{
	return errorList.size();
}

emuString emuParser::ReadAList(emuTokenList * alist)
{
	int Token = this->Token(GetNextWord());
	int ParenMode = tInstructionEnd;
	int parenCount = 0;
	int ArgCount = 0;
	emuString Arg;
	emuString Tail;
	alist->clear();

	if (Token == tOpenParen) {
		ParenMode = tClosedParen;
		parenCount = 1;
		Token = GetNextToken();
	}
	while (1) {
		if (Token == tOpenParen && ParenMode == tClosedParen)
			parenCount++;
		else if (Token == tClosedParen && ParenMode == tClosedParen)
			parenCount--;

		if (Token == TOKEN_EOF) {
			THROW_ERR_S("<end of file>", "inside macro arguments");
		}
		else if (Token == tListSeparator && parenCount <= 1) {
			if (ArgCount) {
				if (!Arg.length())
					THROW_ERR_S(",", " unexpected");
				alist->push_back(Arg);
				Arg = "";
			}
			else {
				Tail = GetLastWord();
				break;
			}
		}
		else if (Token == ParenMode && !parenCount) {
			if (ArgCount)
				alist->push_back(Arg);
			if (Token != tClosedParen)
				Tail = GetLastWord();
			break;
		}
		else if (Token == tClosedParen && parenCount < 1) {
			Tail = GetLastWord();
			break;
		}
		else if (Arg.length()) {
			Arg = Arg + BLANK + GetLastWord();
			ArgCount++;
		}
		else {
			Arg = GetLastWord();
			ArgCount++;
		}
		Token = GetNextToken();
	}
	return Tail;
}

emuString emuParser::GetNextWord()
{
	/*
	 * Preprocessing recursive engine 
	 */
	if (ReadNewWord) {
		int tok,
			t = GetUnparsedToken();
		while (t == TOKEN_EOF) {
			if (!EndParsing())
				break;
			else
				t = GetUnparsedToken();
		}
		while (t == tBeginComment) {
			while (1) {
				t = GetUnparsedToken();
				if (t == tEndComment || t == TOKEN_EOF)
					break;
			}
			t = GetUnparsedToken();
		}
		FileParserStream *F;
		emuExpression *E;
		emuMacro *Macro;
		emuString Tail;
		emuString acc;
		emuString NextWord = Scanner::GetLastWord();

		switch (t) {
		case TOKEN_EOF:
			// This Should happen only if no streams left
			if (!Empty())
				return GetNextWord();
			return "\xff";

		case tIf:{
				emuExpression *E = ParseExpression();
				if (E) {
					emuMacro Macro(".if()");
					emuMacro MacroE(".else()");
					ParseBlock(&Macro, false);
					if (GetUnparsedToken() == tElse)
						ParseBlock(&MacroE, false);
					else
						Tail += GetLastWord();
					if (E->Eval()) {
						BeginParsing(new
							     StringParserStream
							     (Macro.Expand() +
							      BLANK + Tail,
							      Macro.
							      GetSymbol()));
						if (GetOptions(optExpandMacro))
							Macro.ShowExpansion();
					}
					else if (!MacroE.isEmpty()) {
						BeginParsing(new
							     StringParserStream
							     (MacroE.Expand(),
							      MacroE.
							      GetSymbol()));
						if (GetOptions(optExpandMacro))
							MacroE.ShowExpansion();
					}
					else {
						BeginParsing(new
							     StringParserStream
							     (BLANK + Tail,
							      Macro.
							      GetSymbol()));
					}
					NextWord = GetNextWord();
				}
			}
			break;
		case tWarn:
		case tErr:{
				emuMacro Macro(".err");
				ParseBlock(&Macro, true);
				if (t == tErr)
					THROW_ERR_S("user error",
						    Macro.Expand())
						else
					cerr << Macro.Expand() << "\n";
				NextWord = GetNextWord();
			}
			break;
		case tFor:{
				emuMacro Macro("for");
				emuExpression *E1 = 0,
					*E2 = 0;
				int v1,
					v2,
					step,
					i;
				char buf[20];
				emuTokenList alist;
				int Token;
				try {
					EXPECT(TOKEN_IDENTIFIER);
					Macro.addParameter(GetLastWord());
					EXPECT(tAssign);
					E1 = ParseExpression();
					EXPECT(tColon);
					E2 = ParseExpression();
					ParseBlock(&Macro, false);

					v1 = E1->Eval();
					if (E1)
						delete E1;
					v2 = E2->Eval();
					if (E2)
						delete E2;

					if (v1 <= v2)
						step = 1;
					else
						step = -1;
					for (i = v1; (step * i) <= (step * v2);
					     i += step) {
						alist.clear();
						sprintf(buf, "%ld", i);
						alist.push_back(emuString(buf));
						acc += (Macro.Expand(alist) +
							BLANK);
						if (GetOptions(optExpandMacro))
							Macro.ShowExpansion
								(alist);
					}
					BeginParsing(new
						     StringParserStream(acc,
									Macro.
									GetSymbol
									()));
					NextWord = GetNextWord();
				}
				catch(...) {
					if (E1)
						delete E1;
					if (E2)
						delete E2;
					throw;
				}
			}
			break;

		case tDefined:
		case tUndefined:
			NextWord = GetUnparsedWord();
			if (GetLastToken() != TOKEN_IDENTIFIER)
				THROW_ERR_S(NextWord,
					    "found where <IDENTIFIER> expected");
			if (ResolveSymbol(NextWord))
				NextWord = (t == tDefined ? "1" : "0");
			else
				NextWord = (t == tDefined ? "0" : "1");
			break;

		case tEval:
			E = ParseExpression();
			if (!ReadNewWord)
				Tail = GetNextWord();
			BeginParsing(new
				     StringParserStream(inttos(E->Eval()) +
							" " + Tail + " ",
							".eval"));
			NextWord = GetNextWord();
			break;
		case tCurrentFile:
			BeginParsing(new
				     StringParserStream(GetStreamName(),
							".file"));
			NextWord = GetNextWord();
			break;
		case tCurrentLine:
			BeginParsing(new
				     StringParserStream(inttos(GetLineNumber()),
							".line"));
			NextWord = GetNextWord();
			break;
		case tInclude:
			NextWord = Scanner::literal(GetUnparsedWord());
			if (FindStream(NextWord) || GetStreamName() == NextWord)
				THROW_ERR
					("circular include references not allowed");
			F = new FileParserStream(NextWord.chars());
			if (F && !F->Bad()) {
				BeginParsing(F);
				NextWord = GetNextWord();
			}
			else {
				delete F;
				THROW_ERR_S(Scanner::GetLastWord(),
					    ": could not open");
			}
			break;
		case TOKEN_IDENTIFIER:
			if (Macro = MTAB->get(NextWord)) {
				/*
				 * Read macro argument lists 
				 */
				emuTokenList alist;
				emuString Tail = ReadAList(&alist);

				/*
				 * Expand arguments in macro definition 
				 */
				if (alist.size() != Macro->argCount())
					THROW_ERR_S(Macro->GetSymbol(),
						    "requires " +
						    inttos(Macro->argCount()) +
						    " arguments");

				StringParserStream *SP =
					new StringParserStream(Macro->
							       Expand(alist) +
							       BLANK + Tail,
							       Macro->
							       GetSymbol());
				if (GetOptions(optExpandMacro))
					Macro->ShowExpansion(alist);

				if (SP) {
					BeginParsing(SP);
					NextWord = GetNextWord();
				}
			}
			break;

		case tDefine:{
				if (GetUnparsedToken() != TOKEN_IDENTIFIER)
					throw emuException(GetLastWord(),
							   "is not a valid macro name",
							   numToString
							   (GetLineNumber()));
				emuString Sym = GetLastWord();
				emuMacro *Macro = ParseMacro(Sym);
				if (Macro)
					AddMacro(Sym, Macro);
				NextWord = GetNextWord();
			}
			break;
		}
		LastWord = NextWord;
		if (GetOptions(optTokenList))
			ListToken(LastWord);
	}
	else
		ReadNewWord = true;
	return LastWord;
}

void emuParser::SetOptions(unsigned long opts)
{
	Options |= opts;
}

void emuParser::ClearOptions(unsigned long opts)
{
	Options &= ~opts;
}

unsigned long emuParser::GetOptions(unsigned long mask)
{
	return Options & mask;
}

void emuParser::ListToken(const emuString & Symbol)
{
	int token;
	token = Token(Symbol);
	fprintf(stderr, "%-8d|%-50s\n", token, escape(Symbol).chars());
}

void emuParser::Error(emuString S, int line)
{
	errorList.push_back(GetStreamName() + ":" +
			    numToString(line ? line : GetLineNumber()) + ":" +
			    S);
};

emuString emuParser::GetNextFloat()
{
	emuString Symbol = GetNextWord();
	emuString Sign;
	emuString Exponent;
	double dvalue;
	int token_mantissa = this->Token(Symbol);
	if (token_mantissa == TOKEN_FLOAT) {
		return Symbol;
	}
	else {
		if (toupper(Symbol[Symbol.length() - 1]) == 'E') {
			Symbol = Symbol.substr(0, Symbol.length() - 1);
			if (sscanf(Symbol.chars(), "%lf", &dvalue)) {
				int token_sign = GetNextToken();
				int token_exponent;
				if (token_sign == tPlus || token_sign == tMinus) {
					Sign = GetLastWord();
					token_exponent = GetNextToken();
					if (token_exponent == TOKEN_INT)
						Exponent = GetLastWord();
				}
				else if (token_sign == TOKEN_INT) {
					Sign = "+";
					Exponent = GetLastWord();
				}

			}
			LastWord = Symbol + "E" + Sign + Exponent;
			return LastWord;
		}
		else {
			if (sscanf(Symbol.chars(), "%lf", &dvalue))
				return Symbol;
			THROW_ERR_S(Symbol,
				    "found where <floating point number> expected");
		}
	}
};

emuExpression *emuParser::ParseExpressionElement()
{
	int Token,
		TN;
	char TSymbol;
	const char *C;
	emuExpression *E = 0,
		*E1 = 0;
	emuString Number;
	emuElement *I = 0;
	while ((Token = GetNextToken()) == tPlus);
	switch (Token) {
	case tMinus:
		E1 = ParseExpressionElement();
		if (E1)
			return new emuExpression(emuExpression::opNEG, E1);
		break;

	case tBitwiseNot:
		E1 = ParseExpressionElement();
		if (E1)
			return new emuExpression(emuExpression::opNOT, E1);
		break;

	case tNot:
		E1 = ParseExpressionElement();
		if (E1)
			return new emuExpression(emuExpression::opLNOT, E1);
		break;
	case TOKEN_INT:
		Number = GetLastWord();
		C = Number.chars();
		if (C[0] == '0' && toupper(C[1] == 'x')
		    && emuNumber::isnum(C + 2, 16))
			return new emuNumber(C + 2, '$');
		if (emuNumber::isnum(C, 10))
			return new emuNumber(Number, 'd');
		break;
	case TOKEN_FLOAT:
		Number = GetLastWord();
		return new emuNumber(Number, '#');
		break;
	case tSharp:
	case tBitwiseAnd:
		TSymbol = GetSymbol(Token)[0];
		Number = GetNextFloat();
		Token = this->Token(Number);
		if (Token == TOKEN_FLOAT || Token == TOKEN_INT)
			return new emuNumber(Number, TSymbol);
		else
			THROW_ERR("invalid floating point number");
		break;
	case tDollar:
		Number = GetNextWord();
		if (emuNumber::isnum(Number.chars(), 16))
			return new emuNumber(Number, '$');
		else
			THROW_ERR("invalid hexadecimal");
		break;
	case tAt:
		Number = GetNextWord();
		if (emuNumber::isnum(Number.chars(), 8))
			return new emuNumber(Number, '@');
		else
			THROW_ERR("invalid octal");
		break;
	case tMod:
		Number = GetNextWord();
		if (emuNumber::isnum(Number.chars(), 2))
			return new emuNumber(Number, '%');
		else
			THROW_ERR("invalid binary");
		break;

	case tColon:
		EXPECT(TOKEN_IDENTIFIER);
		Number = GetLastWord();
		if (I = ResolveSymbol(Number)) {
			if (!I->getFlags(V_LABEL)) {
				THROW_ERR_S(Number,
					    "symbols previously declared cannot be used as labels");
			}
			else
				return new emuInitRef((emuInitializer *) I, E);
		}
		else {
			emuLabel *L = new emuLabel(Number);
			AddSymbol(Number, L);
			return new emuInitRef((emuInitializer *) L, E);
		}
		break;

	case TOKEN_IDENTIFIER:
		if (I = ResolveSymbol(GetLastWord())) {
			if (GetNextToken() == tOpenSquare) {
				if (I->getFlags(V_ARRAY)) {
					E = ParseExpression();
					EXPECTC(tClosedSquare, if (E) delete E);
				}
				else
					THROW_ERR_S(I->GetSymbol(),
						    "is not an array");
			}
			else
				PushBackWord();
			return new emuInitRef((emuInitializer *) I, E);
		}
		else
			THROW_ERR("undefined");
		break;
	case tOpenParen:
		E = ParseExpression();
		EXPECTC(tClosedParen, delete E);
		return E;

	default:
		break;
	};
	if (E)
		delete E;
	THROW_ERR("unexpected inside expression");
	return 0;
};

int emuParser::OpPriority(int a)
{
	switch (a) {
	case tPower:
		return 100;

	case tTimes:
	case tMod:
	case tDivide:
		return 90;

	case tPlus:
	case tMinus:
		return 80;

	case tLShift:
		return 79;
	case tRShift:
		return 79;
	case tBitwiseAnd:
		return 75;
	case tBitwiseOr:
		return 71;

	case tGreater:
	case tGreaterEqual:
	case tLessEqual:
	case tLess:
	case tNotEqual:
	case tEqual:
		return 70;

	case tAnd:
		return 60;

	case tOr:
		return 50;

	default:
		return 0;
	}
};

emuExpression *emuParser::ParseExpression()
{
	int Token,
		StackToken;
	stack < emuExpression * >ExprStack;
	stack < int >OpStack;
	emuExpression *CurrentExpression = 0,
		*FirstExpression = 0,
		*ThisExpression = 0,
		*E1,
		*E2;
	while (1) {
		try {
			ThisExpression = ParseExpressionElement();
			ExprStack.push(ThisExpression);
			Token = GetNextToken();

			if ((OpStack.empty()
			     || OpPriority(Token) > OpPriority(OpStack.top()))) {
				// OpStack.push(Token);
			}
			else
				while (1) {
					if (OpStack.empty())
						break;
					StackToken = OpStack.top();
					if (OpPriority(StackToken) <
					    OpPriority(Token))
						break;
					E1 = ExprStack.top();
					ExprStack.pop();
					E2 = ExprStack.top();
					ExprStack.pop();
					OpStack.pop();
					// E2->next=E1;
					emuExpression::expr_op op;
					switch (StackToken) {
					case tPlus:
						op = emuExpression::opPLUS;
						break;
					case tMinus:
						op = emuExpression::opMINUS;
						break;
					case tLShift:
						op = emuExpression::opLSHIFT;
						break;
					case tRShift:
						op = emuExpression::opRSHIFT;
						break;
					case tBitwiseAnd:
						op = emuExpression::opAND;
						break;
					case tBitwiseOr:
						op = emuExpression::opOR;
						break;
					case tAnd:
						op = emuExpression::opLAND;
						break;
					case tOr:
						op = emuExpression::opLOR;
						break;
					case tGreater:
						op = emuExpression::opGT;
						break;
					case tGreaterEqual:
						op = emuExpression::opGE;
						break;
					case tLess:
						op = emuExpression::opLT;
						break;
					case tLessEqual:
						op = emuExpression::opLE;
						break;
					case tEqual:
						op = emuExpression::opEQ;
						break;
					case tNotEqual:
						op = emuExpression::opNE;
						break;
					case tTimes:
						op = emuExpression::opTIMES;
						break;
					case tDivide:
						op = emuExpression::opDIVIDE;
						break;
					case tMod:
						op = emuExpression::opMOD;
						break;
					case tXor:
						op = emuExpression::opXOR;
						break;
					}
					emuExpression *E =
						new emuExpression(op, E2, E1);
					ExprStack.push(E);
				};
			if (!IsOperator(Token)) {
				PushBackWord();
				return ExprStack.top();
			}
			else
				OpStack.push(Token);
		}
		catch(...) {
			while (!ExprStack.empty()) {
				delete ExprStack.top();
				ExprStack.pop();
			};
			throw;
		}
	}
};

void emuParser::AddSymbol(const emuString & S, emuElement * E)
{
	SYMTAB->set(S, E);
	SYMLIST->push_back(E);
}

void emuParser::AddMacro(const emuString & S, emuMacro * M)
{
	MTAB->set(S, M);
}

emuElement *emuParser::ResolveSymbol(const emuString & Symbol)
{
	if (emuElement * RET = MTAB->get(Symbol))
		return RET;
	return SYMTAB->get(Symbol);
	return 0;
}

emuExpression *emuParser::ParseOperand()
{
	return ParseExpression();
};

emuInstruction *emuParser::ParseInstruction()
{
	emu8 opCode;
	int tok;
	emuExpression *R = 0,
		*A = 0,
		*X = 0,
		*Y = 0;
	tok = GetNextToken();
	opCode = (tok - emuFirstOp);
	if ((R = ParseOperand()) && GetNextToken() == tListSeparator &&
	    (A = ParseOperand()) && GetNextToken() == tListSeparator &&
	    (X = ParseOperand()) && GetNextToken() == tListSeparator &&
	    (Y = ParseOperand())) {
		emuInstruction *I = new emuInstruction(opCode, R, A, X, Y);
		if (I->isValid())
			return I;
		else
			delete I;
	}
	return 0;
}

int emuParser::ParseInitializerProperties(emuInitializer * I,
					  const char *ValidStrings[],
					  const char *ValidNumbers[])
{
	return ParseInitializerProperties(I->getPropertyList(), ValidStrings,
					  ValidNumbers);
}

int emuParser::ParseInitializerProperties(emuInitializerPropertyList * I,
					  const char *ValidStrings[],
					  const char *ValidNumbers[])
{
	int Token;
	int n = 0;
	const char **C;
	int valid;
	EXPECT(tOpenBrace);
	while (1) {
		Token = GetNextToken();
		if (Token == tClosedBrace)
			break;
		else if (Token == TOKEN_IDENTIFIER || Token == tTram) {
			emuString PropName = GetLastWord();
			EXPECT(tAssign);
			Token = GetNextToken();
			if (Token == TOKEN_STRING) {
				if (ValidStrings) {
					valid = 0;
					for (C = ValidStrings; *C; C++)
						if (PropName == *C) {
							valid = 1;
							break;
						}
				}
				else
					valid = 1;
				if (valid)
					I->setProperty(PropName,
						       escape(GetLastWord()));
				else
					THROW_ERR_S(PropName,
						    "property not allowed for this object");
			}
			else {
				PushBackWord();
				emuExpression *E = ParseExpression();
				if (E) {
					emu32 V = E->Eval();
					delete E;
					if (ValidNumbers) {
						valid = 0;
						for (C = ValidNumbers; *C; C++)
							if (PropName == *C) {
								valid = 1;
								break;
							}
					}
					else
						valid = 1;

					if (valid)
						I->setProperty(PropName, V);
					else
						THROW_ERR_S(PropName,
							    "property not allowed for this object");
				}
			}

			Token = GetNextToken();
			if (Token != tListSeparator)
				PushBackWord();
		}
		else
			THROW_ERR_T(TOKEN_IDENTIFIER);
	}
}

emuInitializer *emuParser::ParseInitializer(unsigned int options,
					    unsigned int valid_options,
					    const char **strings,
					    const char **numbers)
{
#define RET_END(token) if ((token)==tListSeparator || (token)==tInstructionEnd) {PushBackWord(); return I;}
	emuString Identifier;
	int Token;
	int array_decl_count = 0;
	int array_init_count = 0;
	int init_counter = 1;
	Token = GetNextToken();
	if (Token != TOKEN_IDENTIFIER) {
		THROW_ERR("is not a valid initializer symbol");
		return 0;
	}
	Identifier = GetLastWord();

	if (ResolveSymbol(Identifier)) {
		THROW_ERR("already defined");
		return 0;
	}
	emuInitializer *I = new emuInitializer(Identifier, options);
	Token = GetNextToken();
	RET_END(Token);
	if (Token == tOpenSquare) {
		if (!(valid_options & V_ARRAY)) {
			delete I;
			THROW_ERR("array not allowed for this datatype");
		}
		I->setArrayFlag();
		Token = GetNextToken();
		if (Token == tClosedSquare) {
			array_decl_count = 0;
		}
		else {
			PushBackWord();
			emuExpression *E = ParseExpression();
			if (E) {
				array_decl_count = E->Eval();
				delete E;
				if (array_decl_count > 0x100
				    || !array_decl_count) {
					delete I;
					THROW_ERR_S(I->GetSymbol(),
						    "too big, limited to 255 elements");
				}
			}
			EXPECT(tClosedSquare);
		}
		Token = GetNextToken();
		if (array_decl_count)
			I->SetLength(array_decl_count);
		RET_END(Token);
	}
	if (Token == tAddress) {
		if (!(valid_options & V_FIXED)) {
			delete I;
			THROW_ERR("fixed not allowed");
		}
		emuExpression *E = ParseExpression();
		if (E) {
			I->setFixedAddress(E->Eval(V_ADDRESS));
			delete E;
		}
		Token = GetNextToken();
		RET_END(Token);
	}
	else if (options & V_FIXED) {
		{
			delete I;
			THROW_ERR_T(tAddress);
		}
	}
	if (Token == tAssign) {
		if (I->getFlags(V_ARRAY) || I->getFlags(V_STEREO)) {
			EXPECT(tOpenBrace);
			while (1) {
				emuExpression *E = ParseExpression();
				if (E) {
					I->addValue(E->Eval());
					array_init_count++;
					delete E;
				}
				Token = GetNextToken();
				if (Token == tClosedBrace)
					break;
				if (Token != tListSeparator)
					THROW_ERR_T(tListSeparator);
			}
			while (I->nValues() < array_decl_count)
				I->addValue(0);
		}
		else {
			emuExpression *E = ParseExpression();
			if (E) {
				I->addValue(E->Eval());
				delete E;
			}
		}
		if (!array_decl_count && array_init_count)
			I->SetLength(array_init_count);
		Token = GetNextToken();
		if (I->getFlags(V_ARRAY) && !I->getFlags(V_FIXED)
		    && array_decl_count
		    && (array_init_count > array_decl_count)) {
			emuString V = I->GetSymbol();
			delete I;
			THROW_ERR_S(V, "has too many initializers");
		}
		RET_END(Token);
	}

	if (Token == tOpenBrace) {
		PushBackWord();
		try {
			ParseInitializerProperties(I, strings, numbers);
			Token = GetNextToken();
		}
		catch(...) {
			delete I;
			throw;
		}
		RET_END(Token);
	}
	RET_END(Token);
#undef RET_END
	return I;
}

int emuParser::ParseVariables()
{
	int Token;
	int Brace = 0;
	emu32 InitOptions = 0;
	emu32 ValidOptions = 0;
	emu32 TmpOptions = 0;
	emuInitializer *I;
	const char **ANY_PROPERTIES = 0;
	const char *NO_PROPERTIES[] = { 0 };
	const char *LINE_NUM_PROPERTIES[] = { "tram", "offset", "length", 0 };
	const char *CONTROL_NUM_PROPERTIES[] = { "min", "max", "index", 0 };
	const char *CONTROL_SWITCH_PROPERTIES[] = { "index", 0 };
	const char *CONTROL_STR_PROPERTIES[] = { "name", "translation", 0 };
	const char *TRAM_NUM_PROPERTIES[] = { "size", 0 };
	const char **PropStrings = NO_PROPERTIES;
	const char **PropNumbers = NO_PROPERTIES;

	Token = GetNextToken();
	switch (Token) {
	case tFxBus:
		InitOptions = V_READ | V_FXBUS | V_FIXED;
		ValidOptions = InitOptions;
		break;
	case tExtIn:
		InitOptions = V_READ | V_EXTIN | V_FIXED;
		ValidOptions = InitOptions;
		break;
	case tExtOut:
		InitOptions = V_WRITE | V_EXTOUT | V_FIXED;
		ValidOptions = InitOptions;
		break;
	case tConst:
		InitOptions = V_READ;
		ValidOptions = InitOptions | V_ARRAY | V_READ | V_PRIVATE;
		break;
	case tSpec:
		InitOptions = V_FIXED | V_READ | V_SPEC;
		ValidOptions = InitOptions;
		break;
	case tVar:
		InitOptions = V_READ | V_WRITE;
		ValidOptions =
			InitOptions | V_ARRAY | V_REGISTER | V_PRIVATE | V_FXBUS
			| V_EXTIN | V_EXTOUT | V_FIXED;
		break;
	case tTram:
		InitOptions = V_TRAM;
		ValidOptions = InitOptions | V_DELAY | V_LOOKUP;
		PropNumbers = TRAM_NUM_PROPERTIES;
		break;
	case tLine:
		InitOptions = V_LINE;
		ValidOptions = InitOptions | V_READ | V_WRITE;
		PropNumbers = LINE_NUM_PROPERTIES;
		break;
	case tControl:
		InitOptions = V_READ | V_CONTROL;
		ValidOptions = InitOptions | V_STEREO | V_ONOFF;
		PropStrings = CONTROL_STR_PROPERTIES;
		PropNumbers = CONTROL_NUM_PROPERTIES;
		break;
	}

	while (1) {
		Token = GetNextToken();
		if (Token == TOKEN_IDENTIFIER) {
			PushBackWord();
			break;
		}
		switch (Token) {
		case tAbsolute:
			TmpOptions |= V_FIXED;
			break;
		case tExport:
			TmpOptions &= ~V_PRIVATE;
			break;
		case tPrivate:
			TmpOptions |= V_PRIVATE;
			break;
		case tStereo:
			TmpOptions |= V_STEREO;
			break;
		case tMono:
			TmpOptions &= ~V_STEREO;
			break;
		case tRange:
			TmpOptions &= ~V_ONOFF;
			break;
		case tOnOff:
			TmpOptions |= V_ONOFF;
			break;
		case tRead:
			TmpOptions |= V_READ;
			break;
		case tWrite:
			TmpOptions |= V_WRITE;
			break;
		case tDelay:
			if (TmpOptions & V_LOOKUP)
				THROW_ERR_S("delay",
					    "cannot be used with 'lookup'")
					else
				TmpOptions |= V_DELAY;
			break;
		case tLookup:
			if (TmpOptions & V_DELAY)
				THROW_ERR_S("lookup",
					    "cannot be used with 'delay'")
					else
				TmpOptions |= V_LOOKUP;
			break;
		default:
			THROW_ERR("unexpected");
		}
		if ((TmpOptions & ValidOptions) != TmpOptions)
			THROW_ERR("not allowed for this datatype");
	}
	if ((TmpOptions & (V_LINE | V_DELAY | V_LOOKUP)) == V_LINE)
		THROW_ERR_S("delay|lookup", "required for this datatype");
	if (TmpOptions & V_ONOFF)
		PropNumbers = CONTROL_SWITCH_PROPERTIES;
	while (1) {
		Token = GetNextToken();
		if (Token != TOKEN_IDENTIFIER)
			THROW_ERR_T(TOKEN_IDENTIFIER);
		PushBackWord();
		I = ParseInitializer(InitOptions | TmpOptions, ValidOptions,
				     PropStrings, PropNumbers);
		if (!I)
			break;
		else
			AddSymbol(I->GetSymbol(), I);
		if (I->getFlags(V_ARRAY) && !I->nValues() && !I->Length()) {
			THROW_ERR_S(I->GetSymbol(),
				    "array requires initializers or length");
		}
		if (I->getFlags(V_STEREO)) {
			I->SetLength(2);
			if (!I->nValues()) {
				I->addValue(0);
				I->addValue(0);
			}
			if (I->nValues() == 1) {
				I->addValue(I->getValue(0));
			}
			emuInitializer *ILeft =
				new emuInitializer(I->GetSymbol() +
						   emuString(".left"), I, 0);
			emuInitializer *IRight =
				new emuInitializer(I->GetSymbol() +
						   emuString(".right"), I, 1);
			AddSymbol(ILeft->GetSymbol(), ILeft);
			AddSymbol(IRight->GetSymbol(), IRight);
		}
		if (I->getFlags(V_ARRAY) && !I->getFlags(V_FIXED)) {
			int i;
			for (i = I->nValues(); i < I->Length(); i++)
				I->addValue(0);
		}
		if (I->getFlags(V_LINE)) {
			if (!I->hasProperty("tram"))
				I->setProperty("tram", CurrentTram);
			I->setFlags(V_TRAMD | V_TRAMA, V_TRAMD);
			emuInitializer *IData =
				new emuInitializer(I->GetSymbol() +
						   emuString(".data"), I, 0);
			emuInitializer *IAddress =
				new emuInitializer(I->GetSymbol() +
						   emuString(".address"), I,
						   0x100);
			AddSymbol(IData->GetSymbol(), IData);
			IData->setFlags(V_TRAMD, V_TRAMD);
			AddSymbol(IAddress->GetSymbol(), IAddress);
			IAddress->setFlags(V_TRAMA, V_TRAMA);
		}
		if (I->getFlags(V_TRAM)) {
			CurrentTram++;
			I->addValue(CurrentTram);
		}

		Token = GetNextToken();
		if (Token == tInstructionEnd)
			break;
		if (Token != tListSeparator)
			THROW_ERR_T(tListSeparator);
	}
	return 1;
}

int emuParser::ParseControls()
{
}

int emuParser::ParseBlock(emuMacro * Macro, bool eval)
{
	int Token,
		token;
#define B_NEXT_WORD ((eval)?(GetNextWord()):(GetUnparsedWord()))
#define B_NEXT_TOKEN (this->Token(B_NEXT_WORD))
	if (B_NEXT_TOKEN == tOpenBrace) {
		int parencount = 1;
		while (1) {
			token = B_NEXT_TOKEN;
			if (token == tClosedBrace) {
				parencount--;
				if (!parencount)
					break;
			}
			else if (token == tOpenBrace) {
				parencount++;
			}
			else if (token == TOKEN_EOF) {
				THROW_ERR_S("}", "missing in macro definition");
			}
			Macro->addToken(GetLastWord());
		}
	}
	else {
		while (1) {
			token = B_NEXT_TOKEN;
			if (token == tInstructionEnd || token == TOKEN_EOF)
				break;
			else
				Macro->addToken(GetLastWord());
		}
	}
	return 1;

#undef B_NEXT_TOKEN
#undef B_NEXT_WORD
}

emuMacro *emuParser::ParseMacro(const emuString & MacroName)
{
	int Token,
		token;
	EXPECT(tOpenParen);
	emuMacro *Macro = new emuMacro(MacroName);
	while (1) {
		token = GetNextToken();
		if (token == tClosedParen)
			break;
		if (token == TOKEN_IDENTIFIER) {
			Macro->addParameter(GetLastWord());
			token = GetNextToken();
			PushBackWord();
			if (token != tClosedParen)
				EXPECTC(tListSeparator, delete Macro);
		}
	}
	try {
		ParseBlock(Macro, false);
	}
	catch(...) {
		delete Macro;
		throw;
	}
	return Macro;
}

void emuParser::ParsePatch()
{
	emuMacro *Macro;
	emuString Sym;
	list < emuLabel * >llist;
	bool issep = true;
	bool wassep = false;
	while (1) {
		try {
			int tok = GetNextToken();
			if (tok == TOKEN_EOF)
				break;
			if (GetOptions(optPreprocess)) {
				emuString W = GetLastWord();
				wassep = issep;
				issep = ((W.length() == 1 && IsSeparator(W[0]))
					 || (W.length() == 2
					     && IsSeparator(W[0], W[1])));
				if (!issep && !wassep
				    && Token(W) != tInstructionEnd)
					printf("%s", BLANK);
				printf("%s", W.chars());
				if (Token(W) == tInstructionEnd)
					printf("\n");
				continue;
			}
			else if (tok == tInstructionEnd)
				continue;
			else if (IsOpcode(tok)) {
				PushBackWord();
				emuString OPTOKEN = GetLastWord();
				emuInstruction *I = ParseInstruction();
				if (!I)
					THROW_ERR_S(OPTOKEN,
						    "invalid instruction format");

				ILIST->addInstruction(I);
				for (list < emuLabel * >::iterator ll =
				     llist.begin(); ll != llist.end(); ll++)
					(*ll)->setInstruction(I);
				llist.clear();
			}
			else {
				switch (tok) {

				case TOKEN_IDENTIFIER:
					Sym = GetLastWord();
					if (GetNextToken() == tColon) {
						/*
						 * label 
						 */
						emuLabel *M;
						if (!(M = (emuLabel *)
						      ResolveSymbol(Sym)))
							THROW_ERR_S(Sym,
								    "label without reference");
						if (((emuLabel *) M)->
						    HasInstruction())
							THROW_ERR_S(Sym,
								    "label already defined");
						llist.push_back(M);
					}
					else {
						THROW_ERR_S(Sym,
							    "undefined symbol");
					}
					break;
				case tPatch:{
						static const char
						 *PATCH_STR_PROPERTIES[]
							= { "name", "card_name",
							"author", "version",
							"comment", 0
						};
						static const char
						 *PATCH_NUM_PROPERTIES[]
							= { "card_id",
							"gpr_first",
								"gpr_count",
							"line_first",
							"line_count",
							"tram_count",
							"iline_first",
							"iline_count",
							"itram_count",
							"line_address_offset",
							"code_size",
							"shift_opcode",
							"shift_R",
							"shift_A",
							"shift_X",
							"shift_Y",
							"mask_opcode", "mask_R",
							"mask_A",
							"mask_X",
							"mask_Y", 0
						};
						ParseInitializerProperties
							(PROPS,
							 PATCH_STR_PROPERTIES,
							 PATCH_NUM_PROPERTIES);
					}
					break;
				case tInstructionEnd:
					continue;
				case tControl:
				case tFxBus:
				case tExtIn:
				case tExtOut:
				case tTram:
				case tLine:
				case tConst:
				case tVar:
					PushBackWord();
					ParseVariables();
					break;
				default:
					THROW_ERR_S(GetLastWord(),
						    " found where var, const, define, macro or instruction expected");
				}
			}
		}
		catch(emuException & E) {
			Error(E.message(), E.line());
			while (1) {
				int tk = GetUnparsedToken();
				if (tk == TOKEN_EOF || tk == tInstructionEnd)
					break;
			}
		}
	}
	for (list < emuLabel * >::iterator ll = llist.begin();
	     ll != llist.end(); ll++)
		(*ll)->setInstruction(0);
	llist.clear();
}

bool emuParser::IsOperator(int token)
{
	return (token >= emuFirstOper && token <= emuLastOper);
}

bool emuParser::IsOpcode(int token)
{
	return (token >= emuFirstOp && token <= emuLastOp);
}

void emuParser::SetPatchName(const emuString & S)
{
	NAME = S;
}
#define WARN(x) fprintf(stderr,"%s",x)
emuPatch *emuParser::compile(emuPatch * P)
{
	/*
	 * to move 
	 */
	int PC = 0;
	for (emuInstructionList::iterator I = ILIST->begin(); I != ILIST->end();
	     I++) {
		(*I)->setPC(PC++);
	}

	for (list < emuElement * >::iterator i = SYMLIST->begin();
	     i != SYMLIST->end(); i++) {
		if (!*i)
			continue;
		int j;
		int ADDR;
		emu32 fl = (*i)->getFlags(V_ALL);
		char m1[512];
		const emuString SYM = (*i)->getSymbol();
		if (!(fl & V_ADDRESS)) {
			emuInitializer *I = ((emuInitializer *) (*i));
			if (fl & V_FIXED) {
				// errore
			}
			else if (fl & V_ALIAS) {
				ADDR = I->getAddress() +
					I->GetRef()->getAddress();
				I->setAddress(ADDR);
			}
			else if (fl & V_TRAM) {
				P->Image.NewTram(I->getValue(),
						 I->getNumberProperty("size"));
			}
			else if (fl & V_LABEL) {
				// I->addValue(((emuLabel*)I)->getInstruction()->getPC());
			}
			else if (fl == (V_READ | V_VALUE)) {
				// dont'assign address to pure constants
			}
			else if (fl & V_LINE) {
				ADDR = P->Image.NewLine(I->
							getNumberProperty
							("offset"), fl);
				I->setAddress(ADDR);
			}
			else if (((fl & (V_CONSTANT | V_WRITE)) == V_CONSTANT)
				 && !(fl & (V_ARRAY | V_CONTROL))) {
				ADDR = P->Image.MapConstant(I->getValue(0));
				I->setAddress(ADDR);
			}
			else {
				ADDR = P->Image.NewVariable(I->getValue(0),
							    V_GPR | V_READ |
							    V_WRITE | (fl &
								       V_CONTROL));
				I->setAddress(ADDR);
				I->setFlags(V_GPR, V_GPR);
				if (fl & V_ARRAY) {
					for (j = 1; j < (*i)->Length(); j++) {
						P->Image.NewVariable(I->
								     getValue
								     (j),
								     V_GPR |
								     V_READ |
								     V_WRITE |
								     (fl &
								      V_CONTROL));
					}
				}
				if (fl & V_CONTROL) {
					emu32 min,
						max;
					emu8 ttable;
					if (fl & V_ONOFF) {
						min = 0;
						max = MAX_POS;
						ttable = (unsigned char)
							emuControl::onoff;
					}
					else {
						min = I->
							getNumberProperty
							("min");
						max = I->
							getNumberProperty
							("max");
						ttable = (unsigned char)
							emuControl::t100;
					}
					emuString TT =
						I->
						getStringProperty
						("translation");
					if (!TT.empty()) {
						if (!TT.compare("t100"))
							ttable = (unsigned char)
								emuControl::
								t100;
						else if (!TT.compare("bass"))
							ttable = (unsigned char)
								emuControl::
								bass;
						else if (!TT.compare("treble"))
							ttable = (unsigned char)
								emuControl::
								treble;
						else if (!TT.compare("onoff"))
							ttable = (unsigned char)
								emuControl::
								onoff;
						else
							ttable = (unsigned char)
								emuControl::
								none;
					}
					// fprintf(stderr,"control
					// \"%s\"\n",I->getStringProperty("name").chars());
					P->Image.NewControl(I->
							    getStringProperty
							    ("name").chars(),
							    fl, I->getAddress(),
							    I->Length(), min,
							    max, ttable);
				}
			}
		}
	}
	emu32 flags_and;
	emu32 flags_or;
	emu32 addr;
	emu32 value;
	emuExpression::n_type type;
	char m1[512],
		m2[512];
	for (emuInstructionList::iterator I = ILIST->begin(); I != ILIST->end();
	     I++) {
		emu8 opcode = (*I)->getOpCode();
		emu16 op[4];
		for (int i = 0; i < 4; i++) {
			emuExpression *E = (*I)->getExpr(i);

			flags_and = E->EvalFlags(0);
			flags_or = E->EvalFlags(~0);
			if (flags_and & V_ADDRESS) {
				addr = E->Eval(EMUOP_FLAG_ADDRESS);
				value = ~0;
			}
			else if (flags_and & V_LABEL) {
				value = E->Eval();
				if (value > ILIST->size())
					value = ILIST->size();
				if (opcode != INSTR_SKIP || i != 3)
					WARN("Invalid label reference\n");
				value = value - (*I)->getPC() - 1;
				addr = P->Image.MapConstant(value);
			}
			else if (flags_and & V_VALUE) {
				value = E->Eval();
				addr = P->Image.MapConstant(value);
			}
			else {
				value = E->Eval();
				addr = P->Image.MapConstant(value);
			}
			/*
			 * printf("%i) ADDR = %08x VALUE = %d AND(%s)
			 * OR(%s)\n", i,addr,value,
			 * emu_fx_flags_to_char(flags_and, m1, V_BIT_NAMES),
			 * emu_fx_flags_to_char(flags_or, m2, V_BIT_NAMES)); 
			 */

			op[i] = addr;
		}
		P->Image.NewInstruction(opcode, op[0], op[1], op[2], op[3]);
	}
	for (emuSymbolTable::iterator i = SYMTAB->begin(); i != SYMTAB->end();
	     i++) {
		emuInitializer *I = ((emuInitializer *) i->second);
		emu32 fl = I->getFlags();
		if (!(fl & V_PRIVATE)) {
			char m1[128];
			int count;
			emuString S = i->first;
			// fprintf(stderr,"public
			// %s\t%s\n",S.data(),emu_fx_flags_to_char(fl, m1,
			// V_BIT_NAMES));

			if (fl & V_ARRAY)
				count = I->Length();
			else
				count = 1;
			P->Image.NewExport(S.chars(), fl, I->getAddress(),
					   count);
		}
	}
	return P;
}
