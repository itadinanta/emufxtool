#ifndef __DSP_H
#define __DSP_H
#include "config.h"
#include <asmdefs.h>
#define EMU32BITS (sizeof(emu32)*8)

class emuDSP {
	emu32 card_id;
	emu32 gpr_first,
	  gpr_count;
	emu32 line_first,
	  line_count,
	  tram_count;
	emu32 iline_first,
	  iline_count,
	  itram_count;
	emu32 line_address_offset;
	emu32 code_size;
	emu32 shift_opcode,
	  shift_R,
	  shift_A,
	  shift_X,
	  shift_Y;
	emu32 mask_opcode,
	  mask_R,
	  mask_A,
	  mask_X,
	  mask_Y;
      private:
	void set64(emu32 * high, emu32 * low, emu32 shift, emu32 mask,
		   emu32 value);
	emu32 extract64(emu32 high, emu32 low, emu32 shift, emu32 mask);
      public:
	  emuDSP();
	emu32 getFirstGPR();
	emu32 getFirstLine();
	emu32 getFirstILine();
	emu32 getFirstLineAddress();
	emu32 getFirstILineAddress();
	emu32 getILineCount();
	emu32 getLineCount();
	emu32 getGPRCount();
	emu32 getCodeCount();
	emu32 getShiftOpcode();
	emu32 getShiftR();
	emu32 getShiftA();
	emu32 getShiftX();
	emu32 getShiftY();
	emu32 getMaskOpcode();
	emu32 getMaskR();
	emu32 getMaskA();
	emu32 getMaskX();
	emu32 getMaskY();

	emu32 getLineAddressOffset();
	bool isGPR(emu32 addr);
	bool isLine(emu32 addr);
	bool isILine(emu32 addr);
	bool isValidAddr(emu32 addr);
	void initDefaults();
	void splitInstr(emu32 high, emu32 low, emu8 * opcode, emu16 * R,
			emu16 * A, emu16 * X, emu16 * Y);
	void joinInstr(emu32 * high, emu32 * low, emu8 opcode, emu16 R, emu16 A,
		       emu16 X, emu16 Y);
};
#endif
