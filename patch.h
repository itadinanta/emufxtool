#ifndef _PATCH_H
#define _PATCH_H
#include "config.h"
using namespace std;
#include <asmdefs.h>
#include <map>
#include <vector>
#include <stdio.h>
#include <strdsp.h>
#include <dsp.h>

class emuRelocationMap:public map < emu16,
  emu16 > {
      public:
	emuRelocationMap() {
	};
	emu16 relocate(emu16 addr) {
		iterator f;
		if ((f = find(addr)) != end())
			return f->second;
		else
			return addr;
	}
	emu16 remap(emu16 addr, emu16 reloc) {
		if (addr != reloc)
			(*this)[addr] = reloc;
	}
};

class emuReverseSymbolTable;

class emuRegisterImage {
      public:
	emu16 address;
	emu32 value;
	emu32 flags;
	  emuRegisterImage(const emuRegisterImage & i) {
		value = i.value;
		flags = i.flags;
		address = i.address;
	} const emuRegisterImage & operator =(const emuRegisterImage & i) {
		value = i.value;
		flags = i.flags;
		address = i.address;
		return *this;
	}
	emuRegisterImage() {
		value = 0;
		flags = 0;
		address = 0;
	}
      emuRegisterImage(emu32 v, emu32 f):value(v), flags(f), address(0) {
	}
      emuRegisterImage(emu16 addr, emu32 v, emu32 f):value(v), flags(f),
		address(addr)
	{
	}
};

class emuTramImage {
      public:
	emu32 size;
	emu32 offset;
	emu32 flags;
	  emuTramImage() {
		size = 0;
		flags = 0;
		offset = 0;
	} emuTramImage(emu32 f, emu32 s, emu32 o):size(s), offset(o), flags(f) {
	}
      emuTramImage(const emuTramImage & E):size(E.size), offset(E.offset),
		flags(E.flags) {
	}
	const emuTramImage & operator=(const emuTramImage & E) {
		size = E.size;
		offset = E.offset;
		flags = E.flags;
		return *this;
	}
};
typedef map < emu32,
  emuRegisterImage > emuRegisterMap;

class emuCode {
      public:
	emu32 hi, lo;
	emu16 pc;
	  emuCode(emu16, emu8, emu16, emu16, emu16, emu16, emuDSP *);
	  emuCode(emu16, emu32 hiword, emu32 loword);
	  emuCode(const emuCode & c) {
		pc = c.pc;
		hi = c.hi;
		lo = c.lo;
	} const emuCode & operator =(const emuCode & c) {
		hi = c.hi;
		lo = c.lo;
		return *this;
	}
	emu16 getPC() {
		return pc;
	}
	void split(emu8 *, emu16 *, emu16 *, emu16 *, emu16 *, emuDSP *);
	void relocate(emuRelocationMap * m, emuDSP * D);
	void join(emu8, emu16, emu16, emu16, emu16, emuDSP *);
	const char *comment(char *msg);
};

class emuControl {
	char *name;
	emu32 *initial_values;
	emu32 flags;
	emu16 first_gpr;
	emu16 ngprs;
	emu32 range_min,
	  range_max;
	emu8 ttable;
	emu8 index;
      public:
	enum emuTTable { none, t100, treble, bass, onoff };
	const char *getTTableStr();
	  emuControl(const char *ctrlname, emu32 Flags, emu16 gpr, emu16,
		     emu32 min, emu32 max, emu8 ttab, emu8 idx = 0);
	  emuControl(const emuControl & E);
	 ~emuControl();
	const char *getName();
	emu32 getValue(int i);
	emu32 getFlags();
	emu16 getGPR();
	emu16 getNValues();
	emu32 getRangeMin();
	emu32 getRangeMax();
	emu8 getTTable();
	emu8 getIndex();
	const emuControl & operator=(const emuControl & e);
      private:
	void copy(const emuControl & E);
};

class emuExport {
	char *symbol;
	emu32 flags;
	emu16 address;
	emu16 count;
      public:
	  emuExport(const char *sym_name, emu32 flags, emu16 address,
		    emu16 count);
	  emuExport(const emuExport & E);
	 ~emuExport();
	const char *getSymbol();
	emu16 getAddress();
	emu32 getFlags();
	emu16 getCount();
	const emuExport & operator=(const emuExport & e);
      private:
	void copy(const emuExport & E);
};

typedef map < emu32,
  emu16 > emuConstMap;
typedef vector < emuCode > emuCodeList;
typedef vector < emuControl > emuControlList;
typedef vector < emuTramImage > emuTramList;
typedef vector < emuExport > emuExportList;

class emuImage {
	int current_gpr;
	int current_line;
	int current_tram;
	int current_tram_size;
      public:
	  emuDSP DSP;
	emuRegisterMap Registers;
	emuConstMap Constants;
	emuTramList Trams;
	emuCodeList Code;
	emuControlList Controls;
	emuExportList Exports;
	  emuImage();
	int NewRegister(int base_addr, emu32 value, emu32 flags);
	int NewLine(emu32 offset, emu32 flags);
	int NewTram(emu32 length, emu32 flags);
	int NewVariable(emu32 value, emu32 flags);
	int MapConstant(emu32 value);
	emu32 getValue(emu16 addr);
	int NewInstruction(emu8, emu16, emu16, emu16, emu16);
	int NewControl(const char *ctrlname, emu32 fl, emu16 gpr, emu16,
		       emu32 min, emu32 max, emu8 ttable);
	int NewExport(const char *symbol_name, emu32 flags, emu16 addr,
		      emu16 count);
	patchString unAsm(emuCode * C, emuReverseSymbolTable * ST,
			  emuReverseSymbolTable * LBL);
	int unassemble(FILE * f, const char *symtab);
	int Merge(const emuImage & other);
	emu32 getTramSize();
	void Clear();
};

class emuPatch {
	emuImage Image;
      public:

	  emuPatch();
	 ~emuPatch();
	int Save(const char *filename);
	int Load(const char *filename);
	int hwGet(const char *device);
	int hwSet(const char *device);
	int unassemble(FILE * f, const char *symtab = 0);
	friend class emuParser;
};

#endif
