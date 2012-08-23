#ifndef _INSTRUCTION_H
#define _INSTRUCTION_H
#include "config.h"
#include <map>
#include <list>
#include <vector>
#include <scanner.h>
#include <asmdefs.h>

extern const char *V_BIT_NAMES[];
class emuValue {
      protected:
	emu32 flags;
      public:
	emuValue(emu32 f = 0);
	virtual ~ emuValue();
	virtual emu32 getValue(int offset = 0) = 0;
	emu32 getFlags(emu32 mask = ~0);
	void setFlags(emu32 mask, emu32 values);
	inline int isConstant() {
		return (flags & (V_READ | V_WRITE)) == V_READ;
	} inline int isMacro() {
		return flags & V_MACRO;
	}
	inline int hasValue() {
		return flags & V_VALUE;
	}
	inline int hasAddress() {
		return flags & V_ADDRESS;
	}
};

class emuElement:public emuValue {
	emuString symbol;
      public:
	  emuElement(emu32 f = 0, const emuString & sym = "");
	  virtual ~ emuElement();

	const emuString getSymbol();
	virtual int nValues();
	virtual int Length();
	virtual emu32 getValue(int offset = 0);
	virtual emu32 getAddress(int offset = 0);
	virtual emu32 toMachineCode();
	virtual const emuString & GetSymbol();
	virtual void dump_to_file(FILE * F);
};

class emuInitializerProperty {
      public:
	enum iType { iNone, iNumber, iString };
	  emuInitializerProperty(iType t):Type(t) {
	};
	virtual ~ emuInitializerProperty() {
	};
	iType GetType() {
		return Type;
	};
	virtual emuString GetString() {
		return "";
	};
	virtual emu32 GetNumber() {
		return 0;
	};

      private:
	iType Type;
};

class emuInitializerPropertyString:public emuInitializerProperty {
	emuString Value;
      public:
	  emuInitializerPropertyString(const emuString & V):Value(V),
		emuInitializerProperty(iString) {
	};
	virtual emuString GetString() {
		return Value;
	};
};

class emuInitializerPropertyNumber:public emuInitializerProperty {
	emu32 Value;
      public:
	  emuInitializerPropertyNumber(emu32 V):Value(V),
		emuInitializerProperty(iNumber) {
	};
	virtual emu32 GetNumber() {
		return Value;
	};
};

class emuInitializerPropertyList:public map < emuString,
  emuInitializerProperty * > {
      public:
	emuInitializerPropertyList() {
	} ~emuInitializerPropertyList() {
		for (iterator i = begin(); i != end(); i++)
			if (i->second)
				delete i->second;
	}
	emuInitializerProperty *get(emuString name) {
		iterator i = find(name);
		if (i != end())
			return i->second;
		else
			return 0;
	}
	bool hasProperty(emuString name);
	int setProperty(emuString name, emuString Value);
	int setProperty(emuString name, emu32 Value);
	emuInitializerProperty::iType getPropertyType(emuString name);
	emuString getStringProperty(emuString name);
	emu32 getNumberProperty(emuString name);
};

class emuInitializer:public emuElement {
	emu32 address;
	int length;
	  vector < emu32 > initial_values;
	emuInitializerPropertyList properties;
	emuInitializer *ref;
      public:
	  emuInitializer(emuString sym, emu32 attributes);
	  emuInitializer(emuString sym, emuInitializer * R, emu32 offset);
	void SetLength(int l);
	  virtual ~ emuInitializer();
	virtual int nValues();
	virtual int Length();
	virtual emu32 getAddress(int offset = 0);
	virtual emu32 getValue(int offset = 0);
	void addValue(emu32);
	void setAddress(emu32);
	void setFixedAddress(emu32);
	void setArrayFlag();
	bool hasProperty(emuString name);
	emuInitializerPropertyList *getPropertyList() {
		return &properties;
	} int setProperty(emuString name, emuString Value);
	int setProperty(emuString name, emu32 Value);
	emuInitializerProperty::iType getPropertyType(emuString name);
	emuString getStringProperty(emuString name);
	emu32 getNumberProperty(emuString name);
	emuInitializer *GetRef();
	virtual void dump_to_file(FILE * F);
};

#define EMUOP_FLAG_ADDRESS 2
class emuExpression:public emuValue {
      protected:
	emuExpression * A;
	emuExpression *B;
      public:
	enum expr_op { opLEAF, opPLUS, opMINUS, opTIMES, opDIVIDE, opMOD,
			opLSHIFT, opRSHIFT, opNEG, opNOT, opAND, opOR, opXOR,
			opLAND, opLNOT, opLOR, opGT, opGE, opLT, opLE, opEQ,
			opNE } op;
	enum n_type { NONE, INT, DECIMAL, TIME, REGISTER, LABEL };
	  emuExpression(emuExpression::expr_op o, emuExpression * a,
			emuExpression * b = 0);
	virtual emu32 getValue(int offset = 0);
	virtual unsigned long Eval(int options = 0);
	virtual unsigned long EvalFlags(emu32 or_mask);
	virtual n_type EvalType();
	  virtual ~ emuExpression();
};

class emuNumber:public emuExpression {
	emu32 value;
	n_type nType;
      public:
	static inline int digit(char c);
	static int isnum(const char *c, unsigned long base);
	static unsigned long char2base(const char *c, unsigned long base);
	static unsigned long from_char(const char *c, char *ntype = 0);
      public:
	  virtual n_type EvalType();
	virtual unsigned long Eval(int options = 0);
	  emuNumber(const emuString & N, char ctype);
};

class emuInitRef:public emuExpression {
	emuInitializer *Initializer;
      public:
	  virtual n_type EvalType();
	virtual unsigned long Eval(int options = 0);
	virtual unsigned long EvalFlags(emu32 or_mask);
	  emuInitRef(emuInitializer * I, emuExpression * Index);
};

class emuRegister:public emuNumber {
	emu32 address;
      public:
	  emuRegister(emuString sym, emu32 addr, emu32 val);
	  emuRegister(emuString sym, emu32 addr, emu8 nval, emu32 * val);
	  virtual ~ emuRegister();
	virtual emu32 getAddress(int offset = 0);
	virtual emu32 getValue(int offset = 0);
	  emuRegister(emu32 addr, const emuString & N, char ctype);
};

class emuSymbolTable;
class emuInstruction {
	emu8 opCode;
	emu32 line;
	emuExpression *R;
	emuExpression *A;
	emuExpression *X;
	emuExpression *Y;
	emu32 PC;
      public:
	  emuInstruction(emu8 opcode, emuExpression * r, emuExpression * a,
			 emuExpression * x, emuExpression * y);
	bool isValid();
	emuExpression *getExpr(int i);
	emu8 getOpCode();
	emu32 getPC();
	void setPC(emu16);
	 ~emuInstruction();
	friend class emuInstructionList;
};

class emuLabel:public emuElement {
	emuInstruction *I;
      public:
	  emuLabel(emuString sym);
	bool HasInstruction();
	emuInstruction *setInstruction(emuInstruction * M);
	emu32 getPC();
	virtual emu32 getValue(int offset = 0);
};

class emuInstructionList:public vector < emuInstruction * > {
      public:
	emuInstructionList();
	int addInstruction(emuInstruction *);
};

typedef map < emuString,
  emuString,
  less < emuString > >emuTokenMap;
typedef list < emuString > emuTokenList;

class emuMacro:public emuElement {
	emuTokenList Parameters;
	emuTokenList Tokens;
      public:
	  emuMacro(const emuString & name);
	  virtual ~ emuMacro();
	int argCount();
	int addParameter(const emuString & p);
	void addToken(const emuString & t);
	int isEmpty();
	emuString Expand(const emuTokenList & arguments);
	emuString Expand();
	void ShowExpansion(const emuTokenList & arguments);
	void ShowExpansion();
};

#endif
