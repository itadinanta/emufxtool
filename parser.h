// ---------------------------------------------------------------------------
#ifndef _EMUPARSER_H
#define _EMUPARSER_H
#include "config.h"
#include <scanner.h>
#include <excep.h>
#include <list>
#include <map>

// ---------------------------------------------------------------------------
class emuInstruction;
class emuExpression;
class emuInitializer;
class emuInitializerPropertyList;
class emuOperand;
class emuElement;
class emuException;
class emuInstructionList;
class emuPatch;

class emuSymbolTable:public map < emuString,
  emuElement *,
  less < emuString > > {
      public:
	emuSymbolTable();
	~emuSymbolTable();
	void set(const emuString & key, emuElement * E);
	emuElement *get(const emuString & key);
	int cleanup();
	int save(FILE * f);
	emuString GetSymbol(emuElement * E);
};

class emuMacro;
class emuMacroTable:public map < emuString,
  emuMacro *,
  less < emuString > > {
      public:
	emuMacroTable();
	~emuMacroTable();
	void set(const emuString & key, emuMacro * E);
	emuMacro *get(const emuString & key);
};

typedef list < emuString > emuTokenList;
class emuParser:public Scanner {
	list < emuString > errorList;
	emuSymbolTable *SYMTAB;
	  list < emuElement * >*SYMLIST;
	emuMacroTable *MTAB;
	emuString NAME;
	emuInstructionList *ILIST;
	emuInitializerPropertyList *PROPS;
	unsigned long Options;
	int CurrentTram;
      public:
	static const int optTokenList = 1;
	static const int optPreprocess = 2;
	static const int optExpandMacro = 4;
	  emuParser();
	 ~emuParser();
	/*
	 * Error Functions 
	 */
	void Error(emuString msg, int line = 0);
	void SetOptions(unsigned long);
	void ClearOptions(unsigned long);
	unsigned long GetOptions(unsigned long);
	int ErrorState();
	/*
	 * SubParsers 
	 */
	emuInstruction *ParseInstruction();
	emuExpression *ParseExpressionElement();
	emuExpression *ParseExpression();
	emuExpression *ParseOperand();
	int ParseBlock(emuMacro * body, bool eval = true);
	int ParseInitializerProperties(emuInitializerPropertyList * I,
				       const char *ValidStrings[],
				       const char *ValidNumbers[]);
	int ParseInitializerProperties(emuInitializer * I,
				       const char *ValidStrings[],
				       const char *ValidNumbers[]);
	emuInitializer *ParseInitializer(unsigned int options,
					 unsigned int valid_options,
					 const char *ValidStrings[],
					 const char *ValidNumbers[]);
	void SetPatchName(const emuString & S);
	// emuOperand *ParseParameterList();
	emuMacro *ParseMacro(const emuString & MacroName);
	int ParseVariables();
	int ParseControls();

	void ParsePatch();
	void ListToken(const emuString & Symbol);
	emuPatch *compile(emuPatch * P);
	/*
	 * Low level parser 
	 */
	// virtual void BeginParsing(ParserStream *Parser);
	virtual emuString GetNextWord();
	inline emuString GetUnparsedWord() {
		return Scanner::GetNextWord();
	} inline int GetUnparsedToken() {
		return Token(GetUnparsedWord());
	}
	inline int GetLastToken() {
		return Token(GetLastWord());
	}
	int SaveSymbols(const char *filename);
      protected:
	virtual bool IsIdentifier(const emuString & Word) const;
	emuElement *ResolveSymbol(const emuString &);
	emuString GetNextFloat();
	void SetName(const emuString & s);
	void AddSymbol(const emuString &, emuElement * E);
	void AddMacro(const emuString &, emuMacro * E);
      private:
	int OpPriority(int a);
	bool IsOperator(int token);
	bool IsOpcode(int token);
	bool IsIdentifier(const emuString & I);
	emuString ReadAList(emuTokenList * alist);
};

#endif
