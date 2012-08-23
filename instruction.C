#include <instruction.h>
#include <math.h>
#include <iostream>
const char *V_BIT_NAMES[] = { "V_VALUE",
	"V_ADDRESS",
	"V_READ",
	"V_WRITE",
	"V_FXBUS",
	"V_EXTIN",
	"V_EXTOUT",
	"V_SPEC",
	"V_GPR",
	"V_TRAMD",
	"V_TRAMA",
	"V_LABEL",
	"V_PRIVATE",
	"V_FIXED",
	"V_ARRAY",
	"V_MACRO",
	"V_DELAY",
	"V_LOOKUP",
	"V_LINE",
	"V_TRAM",
	"V_STEREO",
	"V_ONOFF",
	"V_ALIAS",
	"V_LINEDATA",
	"V_LINEADDR",
	"V_CONTROL",
	"V_LEAF",
	0
};

emuValue::emuValue(emu32 f)
{
	flags = f;
}
emuValue::~emuValue()
{
}
emu32 emuValue::getFlags(emu32 mask)
{
	return flags & mask;
}
void emuValue::setFlags(emu32 mask, emu32 values)
{
	flags = (flags & (~mask)) | (values & mask);
}

emuElement::emuElement(emu32 f, const emuString & sym):symbol(sym),
emuValue(f)
{
};
emuElement::~emuElement()
{
};
emu32 emuElement::getValue(int offset)
{
	return 0;
}
emu32 emuElement::getAddress(int offset)
{
	return 0;
}
emu32 emuElement::toMachineCode()
{
	return 0;
};
const emuString & emuElement::GetSymbol()
{
	return symbol;
};
const emuString emuElement::getSymbol()
{
	return symbol;
};

emuInitializer::emuInitializer(emuString sym, emu32 attributes):
emuElement(attributes, sym), length(1), ref(0)
{
	address = 0;
};

emuInitializer *emuInitializer::GetRef()
{
	return ref;
};

emuInitializer::emuInitializer(emuString sym, emuInitializer * ref_of, emu32 offset):
emuElement(((ref_of->getFlags(V_ALL & ~V_ARRAY) | V_ALIAS)), sym), ref(ref_of),
length(1)
{
	address = offset;
};

emuInitializer::~emuInitializer()
{
};

emu32 emuInitializer::getAddress(int offset)
{
	return address + offset;
}
emu32 emuInitializer::getValue(int offset)
{
	if (offset >= 0 && offset < initial_values.size())
		return initial_values[offset];
	else
		return 0;
};

int emuElement::nValues()
{
	return 1;
}

int emuElement::Length()
{
	return 1;
}

int emuInitializer::nValues()
{
	return initial_values.size();
}

int emuInitializer::Length()
{
	return length;
}

void emuInitializer::SetLength(int l)
{
	length = l;
}

void emuElement::dump_to_file(FILE * F)
{
	const char **bitnames;
	int j = 0;
	int first;
	char m[512];
	if (F) {
		fprintf(F, "\n%s", GetSymbol().chars());
		if (getFlags(V_ARRAY) || Length() != 1)
			fprintf(F, "[%d]", Length());
		fprintf(F, "\n");
		if (getFlags(V_ADDRESS))
			fprintf(F, "\taddress = 0x%04x\n", getAddress());
		fprintf(F, "\tflags = %s\n",
			flagsToString(getFlags(), V_BIT_NAMES).data());
	}
}

void emuInitializer::dump_to_file(FILE * F)
{
	int j = 0;
	int first;
	emuElement::dump_to_file(F);
	if (F) {
		if (ref) {
			fprintf(F, "\treference to = %s\n",
				ref->GetSymbol().chars());
			fprintf(F, "\toffset = 0x%03x\n", getAddress());
		}

		if (nValues()) {
			fprintf(F, "\tinitial_values = {");
			first = 1;
			for (vector < emu32 >::iterator i =
			     initial_values.begin(); i != initial_values.end();
			     i++) {
				if (!first)
					fprintf(F, ", ");
				fprintf(F, "0x%08x (%.7lf)", *i,
					(double) (*i) / MAX_POS);
				first = 0;
			}
			fprintf(F, "}\n");
		}
		if (properties.size()) {
			fprintf(F, "\tproperties = {");
			first = 1;
			for (emuInitializerPropertyList::iterator i =
			     properties.begin(); i != properties.end(); i++) {
				if (!first)
					fprintf(F, ", ");
				if (i->second->GetType() ==
				    emuInitializerProperty::iString)
					fprintf(F, "%s = %s", i->first.chars(),
						i->second->GetString().chars());
				else
					fprintf(F, "%s = 0x%08x (%.7lf)",
						i->first.chars(),
						i->second->GetNumber(),
						(double) (i->second->
							  GetNumber()) /
						MAX_POS);
				first = 0;
			}
			fprintf(F, "}\n");
		}
	}
}

void emuInitializer::addValue(emu32 v)
{
	flags |= V_VALUE;
	initial_values.push_back(v);
	if (initial_values.size() > 1)
		setArrayFlag();
};

void emuInitializer::setArrayFlag()
{
	flags |= V_ARRAY;
};

void emuInitializer::setFixedAddress(emu32 v)
{
	flags |= V_FIXED | V_ADDRESS;
	address = v;
};

void emuInitializer::setAddress(emu32 v)
{
	flags &= ~V_FIXED;
	flags |= V_ADDRESS;
	address = v;
};

bool emuInitializer::hasProperty(emuString name)
{
	return properties.hasProperty(name);
};
int emuInitializer::setProperty(emuString name, emu32 Value)
{
	return properties.setProperty(name, Value);
}
int emuInitializer::setProperty(emuString name, emuString Value)
{
	return properties.setProperty(name, Value);
}
emuInitializerProperty::iType emuInitializer::getPropertyType(emuString name)
{
	return properties.getPropertyType(name);
}
emuString emuInitializer::getStringProperty(emuString name)
{
	return properties.getStringProperty(name);
}
emu32 emuInitializer::getNumberProperty(emuString name)
{
	return properties.getNumberProperty(name);
}

bool emuInitializerPropertyList::hasProperty(emuString name)
{
	return get(name);
};

int emuInitializerPropertyList::setProperty(emuString name, emu32 Value)
{
	emuInitializerProperty *p;
	if (p = get(name))
		delete p;
	(*this)[name] = new emuInitializerPropertyNumber(Value);
	return (p != 0);
};

int emuInitializerPropertyList::setProperty(emuString name, emuString Value)
{
	emuInitializerProperty *p;
	if (p = get(name))
		delete p;
	(*this)[name] = new emuInitializerPropertyString(Value);
	return (p != 0);
};

emuInitializerProperty::iType emuInitializerPropertyList::
getPropertyType(emuString name)
{
	emuInitializerProperty *p;
	if (p = get(name))
		return p->GetType();
	else
		return emuInitializerProperty::iNone;
};

emuString emuInitializerPropertyList::getStringProperty(emuString name)
{
	emuInitializerProperty *p;
	if (p = get(name))
		return p->GetString();
	else
		return "";
};

emu32 emuInitializerPropertyList::getNumberProperty(emuString name)
{
	emuInitializerProperty *p;
	if (p = get(name))
		return p->GetNumber();
	else
		return 0;
}

emuExpression::emuExpression(emuExpression::expr_op o, emuExpression * a,
			     emuExpression * b)
{
	A = a;
	B = b;
	op = o;
}

emu32 emuExpression::getValue(int offset)
{
	return Eval();
}

unsigned long emuExpression::EvalFlags(emu32 or_mask)
{
	if (!A)
		return getFlags() | V_LEAF;
	if (!B)
		return A->EvalFlags(or_mask);
	const emu32 OR_FLAGS = 0;
	const emu32 AND_FLAGS = ~OR_FLAGS;
	return (((A->EvalFlags(or_mask) | B->EvalFlags(or_mask)) & or_mask)
		| ((A->EvalFlags(or_mask) & B->EvalFlags(or_mask)) & ~or_mask))
		& (~V_LEAF);
}

unsigned long emuExpression::Eval(int options)
{
	if (!A)
		return 0;
	switch (op) {
	case opPLUS:
		return A->Eval(options) + (B ? B->Eval(options) : 0);
	case opMINUS:
		return A->Eval(options) - (B ? B->Eval(options) : 0);
	case opTIMES:
		return A->Eval(options) * (B ? B->Eval(options) : 0);
	case opDIVIDE:
		return A->Eval(options) / (B ? B->Eval(options) : 0);
	case opMOD:
		return A->Eval(options) % (B ? B->Eval(options) : 0);
	case opLSHIFT:
		return A->Eval(options) << (B ? B->Eval(options) : 0);
	case opRSHIFT:
		return A->Eval(options) >> (B ? B->Eval(options) : 0);
	case opNEG:
		return -A->Eval(options);
	case opNOT:
		return ~A->Eval(options);
	case opLNOT:
		return !A->Eval(options);
	case opLAND:
		return A->Eval(options) && (B ? B->Eval(options) : 0);
	case opLOR:
		return A->Eval(options) || (B ? B->Eval(options) : 0);
	case opAND:
		return A->Eval(options) & (B ? B->Eval(options) : 0);
	case opOR:
		return A->Eval(options) | (B ? B->Eval(options) : 0);
	case opGT:
		return A->Eval(options) > (B ? B->Eval(options) : 0);
	case opGE:
		return A->Eval(options) >= (B ? B->Eval(options) : 0);
	case opLT:
		return A->Eval(options) < (B ? B->Eval(options) : 0);
	case opLE:
		return A->Eval(options) <= (B ? B->Eval(options) : 0);
	case opEQ:
		return A->Eval(options) == (B ? B->Eval(options) : 0);
	case opNE:
		return A->Eval(options) != (B ? B->Eval(options) : 0);
	case opLEAF:
	default:
		return A->Eval(options);
	}
}

emuExpression::n_type emuExpression::EvalType()
{
	if (!A)
		return NONE;
	if (!B)
		return A->EvalType();
	if (A->EvalType() == REGISTER && B->EvalType() == INT) {
		int ptr = (A->Eval(EMUOP_FLAG_ADDRESS));
		if (ptr >= 0x100 && ptr <= 0x3ff) {
			int offset = B->Eval();
			if (((ptr & 0xff) + offset > 0)
			    && (ptr & 0xff + offset <= 0x100))
				return REGISTER;
			else
				return INT;
		}
	}
	return min(A->EvalType(), B->EvalType());
}

emuExpression::~emuExpression()
{
	if (A)
		delete A;
	if (B)
		delete B;
}

emuNumber::emuNumber(const emuString & N, char ctype):emuExpression(opLEAF, 0,
								    0)
{
	value = from_char(N.c_str(), &ctype);
	setFlags(V_ALL, V_READ | V_VALUE);
}

inline int emuNumber::digit(char c)
{
	return ((c >= '0' && c <= '9') ? (c - '0')
		: ((c >= 'a' && c <= 'z') ? (c - 'a' + 10)
		   : ((c >= 'A' && c <= 'Z') ? (c - 'A' + 10)
		      : -1)));
}

int emuNumber::isnum(const char *c, unsigned long base)
{
	register const char *C = 0;
	register int v;
	for (C = c; *C; C++)
		if (((v = digit(*C)) < 0) || v >= base)
			return 0;
	return 1;
}

unsigned long emuNumber::char2base(const char *c, unsigned long base)
{
	unsigned long acc = 0;
	const char *C = 0;
	for (C = c; *C; C++)
		acc = ((unsigned long) digit(*C)) + (acc * base);
	return acc;
}

unsigned long emuNumber::from_char(const char *c, char *ntype)
{
	double dvalue;
	char ctype = 0;
	unsigned long rv = (unsigned long) (-1);
	if (ntype)
		ctype = *ntype;
	if (!ctype) {
		if (!strchr(c, '.') || strchr(c, 'E') || strchr(c, 'e'))
			ctype = '#';
		else if (c[0] == '0' && (c[1] == 'x' || c[1] == 'X')
			 && isnum(c + 2, 16)) {
			ctype = 'x';
			c += 2;
		}
	}
	dvalue = 0;
	switch (ctype) {
	case '#':
		if (sscanf(c, "%lf", &dvalue))
			rv = (unsigned long) (long) (dvalue * 0x00010000L	/* fraction 
										 * ? */ );
		else
			ctype = 0;
		break;
	case '&':
		if (sscanf(c, "%lf", &dvalue))
			rv = (unsigned long) (long) ceil(dvalue * 48000	/* samples 
									 */ );
		else
			ctype = 0;
		break;
	case 'x':
	case '$':
		if (isnum(c, 16))
			rv = char2base(c, 16);
		else
			ctype = 0;
		break;
	case '@':
		if (isnum(c, 8))
			rv = char2base(c, 8);
		else
			ctype = 0;
		break;
	case '%':
		if (isnum(c, 2))
			rv = char2base(c, 2);
		else
			ctype = 0;
		break;
	default:
		ctype = 'd';
		if (isnum(c, 10))
			rv = char2base(c, 10);
		else
			ctype = 0;
		break;
	}
	if (ntype)
		*ntype = ctype;
	return rv;
}

emuNumber::n_type emuNumber::EvalType()
{
	return nType;
}

unsigned long emuNumber::Eval(int options)
{
	return value;
}

emuInstruction::emuInstruction(emu8 opcode, emuExpression * r, emuExpression * a, emuExpression * x, emuExpression * y):
R(r), X(x), A(a), Y(y), opCode(opcode), PC(0)
{
}

emu8 emuInstruction::getOpCode()
{
	return opCode;
}

emu32 emuInstruction::getPC()
{
	return PC;
}

void emuInstruction::setPC(emu16 pc)
{
	PC = pc;
}

emuExpression *emuInstruction::getExpr(int i)
{
	switch (i) {
	case 0:
		return R;
	case 1:
		return A;
	case 2:
		return X;
	case 3:
		return Y;
	}
	return 0;
}

bool emuInstruction::isValid()
{
	/*
	 * to improve 
	 */
	return (R && A && X && Y);
}

emuInstruction::~emuInstruction()
{
	if (R)
		delete R;
	if (A)
		delete A;
	if (X)
		delete X;
	if (Y)
		delete Y;
};

emuInstructionList::emuInstructionList()
{

};

int emuInstructionList::addInstruction(emuInstruction * I)
{
	if (I) {
		I->PC = size();
		push_back(I);
	}
}

emuMacro::emuMacro(const emuString & NAME):emuElement(V_MACRO, NAME)
{
};
emuMacro::~emuMacro()
{
};

int emuMacro::isEmpty()
{
	return Tokens.empty();
}

int emuMacro::argCount()
{
	return Parameters.size();
};

int emuMacro::addParameter(const emuString & p)
{
	for (emuTokenList::iterator i = Parameters.begin();
	     i != Parameters.end(); i++)
		if (*i == p)
			return 0;
	Parameters.push_back(p);
	return 1;
};

void emuMacro::addToken(const emuString & t)
{
	Tokens.push_back(t);
};

emuString emuMacro::Expand()
{
	emuTokenList DummyList;
	return Expand(DummyList);
}

void emuMacro::ShowExpansion()
{
	emuTokenList DummyList;
	ShowExpansion(DummyList);
}

emuString emuMacro::Expand(const emuTokenList & arguments)
{
	emuTokenList::const_iterator i, inext;
	emuTokenList::const_iterator j = arguments.begin();
	emuTokenMap M;
	emuTokenMap::iterator s;
	for (i = Parameters.begin(); i != Parameters.end(); i++) {
		M[*i] = *j;
		j++;
	}
	emuString accumulator;
	emuString sym,
		sym2,
		sym3;
	bool blank_flag = 0;
	bool isarg = 0,
		wasarg = 0,
		wasplus = 0;
	for (i = Tokens.begin(); i != Tokens.end(); i++) {
		wasarg = isarg;
		sym = *i;
		if (sym == "##") {	// concat expansion operator
			isarg = false;
			inext = i;
			inext++;
			if (inext != Tokens.end()) {
				if (M.find(*inext) != M.end() || wasarg) {
					blank_flag = 0;
					i = inext;
					sym = *i;
				}
			}
		}

		if (blank_flag) {
			accumulator += " ";
		}
		else
			blank_flag = 1;

		if ((s = M.find(sym)) != M.end()) {
			accumulator += (s->second);
			isarg = true;
		}
		else {
			accumulator += (sym);
			isarg = false;
		}
	}
	// cout << "\nmacro "<< GetSymbol() <<" expanded to
	// "<<accumulator<<"\n";
	return accumulator;
}

void emuMacro::ShowExpansion(const emuTokenList & arguments)
{
	emuTokenList::const_iterator i, iplus;
	emuTokenList::const_iterator j = arguments.begin();
	emuTokenMap M;
	emuTokenMap::iterator s;
	cout << "==>\nMacro parameters -------------------------------------------\n";
	cout << GetSymbol() << "(";
	for (i = Parameters.begin(); i != Parameters.end(); i++) {
		cout << "\t" << *i << " => " << *j << "\n";
		j++;
	}
	cout << ")";
	cout << "\n- Macro definition ----------------------------\n";
	for (i = Tokens.begin(); i != Tokens.end(); i++)
		cout << *i << " ";
	cout << "\n- Macro expansion -----------------------------\n";
	cout << Expand(arguments);
	cout << "\n-----------------------------------------------\n";
}

emuInitRef::emuInitRef(emuInitializer * I, emuExpression * Index):
Initializer(I), emuExpression(opLEAF, Index)
{

}

emuExpression::n_type emuInitRef::EvalType()
{
	return LABEL;
}

unsigned long emuInitRef::Eval(int options)
{
	unsigned long index = emuExpression::Eval(0);
	if (options & EMUOP_FLAG_ADDRESS)
		return Initializer->getAddress(index);
	else
		return Initializer->getValue(index);
}

unsigned long emuInitRef::EvalFlags(emu32 mask)
{
	unsigned long index = emuExpression::Eval(0);
	return Initializer->getFlags(~0);
}

emuLabel::emuLabel(emuString sym):emuElement(V_LABEL, sym)
{
	I = 0;
};
emuInstruction *emuLabel::setInstruction(emuInstruction * M)
{
	I = M;
};
bool emuLabel::HasInstruction()
{
	return I;
}
emu32 emuLabel::getPC()
{
	if (I)
		return I->getPC();
	else
		return ~0L;
}
emu32 emuLabel::getValue(int)
{
	if (I)
		return I->getPC();
	else
		return ~0L;
}
