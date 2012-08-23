#include <dsp.h>
void emuDSP::set64(emu32 * high, emu32 * low, emu32 shift, emu32 mask,
		   emu32 value)
{
	emu32 low_mask = 0,
		low_value = 0,
		high_mask = 0,
		high_value = 0;

	if (shift >= EMU32BITS) {
		high_mask = (mask << (shift - EMU32BITS));
		high_value = (value << (shift - EMU32BITS));
	}
	else {
		low_mask = (mask << shift);
		low_value = (value << shift);
		if (shift) {
			high_mask = (mask >> (EMU32BITS - shift));
			high_value = (value >> (EMU32BITS - shift));
		}
	}

	*low = (*low & (~low_mask)) | low_value;
	*high = (*high & (~high_mask)) | high_value;
}

emu32 emuDSP::extract64(emu32 high, emu32 low, emu32 shift, emu32 mask)
{
	if (shift >= EMU32BITS)
		return (high >> (shift - EMU32BITS)) & mask;
	else
		return ((low >> shift) |
			(shift ? (high << (EMU32BITS - shift)) : 0)) & mask;
}

emuDSP::emuDSP()
{
	initDefaults();
}
bool emuDSP::isGPR(emu32 addr)
{
	return addr >= gpr_first && addr < (gpr_first + gpr_count);
}
bool emuDSP::isLine(emu32 addr)
{
	return addr >= line_first && addr < (line_first + line_count);
}
bool emuDSP::isILine(emu32 addr)
{
	return addr >= gpr_first && addr < (iline_first + iline_count);
}
bool emuDSP::isValidAddr(emu32 addr)
{
	return addr < code_size;
}

void emuDSP::initDefaults()
{
	card_id = 1;		// SB Live!
	gpr_first = 0x100;
	gpr_count = 0x100;
	iline_first = 0x200;
	iline_count = 0x80;
	itram_count = 0x100000;
	line_first = 0x280;
	line_count = 0x80;
	tram_count = 0x100000;
	line_address_offset = 0x100;
	code_size = 512;
	shift_opcode = 52;
	shift_R = 42;
	shift_A = 32;
	shift_X = 10;
	shift_Y = 0;
	mask_opcode = 0x0f;
	mask_R = mask_A = mask_X = mask_Y = 0x3ff;
}

void emuDSP::splitInstr(emu32 high, emu32 low, emu8 * opcode, emu16 * R,
			emu16 * A, emu16 * X, emu16 * Y)
{
	*opcode = extract64(high, low, shift_opcode, mask_opcode);
	*R = extract64(high, low, shift_R, mask_R);
	*A = extract64(high, low, shift_A, mask_A);
	*X = extract64(high, low, shift_X, mask_X);
	*Y = extract64(high, low, shift_Y, mask_Y);
}

void emuDSP::joinInstr(emu32 * high, emu32 * low, emu8 opcode, emu16 R, emu16 A,
		       emu16 X, emu16 Y)
{
	*high = 0;
	*low = 0;
	set64(high, low, shift_opcode, mask_opcode, opcode);
	set64(high, low, shift_R, mask_R, R);
	set64(high, low, shift_A, mask_A, A);
	set64(high, low, shift_X, mask_X, X);
	set64(high, low, shift_Y, mask_Y, Y);
}

emu32 emuDSP::getFirstGPR()
{
	return gpr_first;
};
emu32 emuDSP::getFirstLine()
{
	return line_first;
};
emu32 emuDSP::getFirstILine()
{
	return iline_first;
}
emu32 emuDSP::getFirstLineAddress()
{
	return line_first + line_address_offset;
};
emu32 emuDSP::getFirstILineAddress()
{
	return iline_first;
}
emu32 emuDSP::getGPRCount()
{
	return gpr_count;
};
emu32 emuDSP::getLineCount()
{
	return line_count;
};
emu32 emuDSP::getILineCount()
{
	return iline_count;
}
emu32 emuDSP::getCodeCount()
{
	return code_size;
}
emu32 emuDSP::getLineAddressOffset()
{
	return line_address_offset;
};
emu32 emuDSP::getShiftOpcode()
{
	return shift_opcode;
};
emu32 emuDSP::getShiftR()
{
	return shift_R;
};
emu32 emuDSP::getShiftA()
{
	return shift_A;
};
emu32 emuDSP::getShiftX()
{
	return shift_X;
};
emu32 emuDSP::getShiftY()
{
	return shift_Y;
};
emu32 emuDSP::getMaskOpcode()
{
	return mask_opcode;
};
emu32 emuDSP::getMaskR()
{
	return mask_R;
};
emu32 emuDSP::getMaskA()
{
	return mask_A;
};
emu32 emuDSP::getMaskX()
{
	return mask_X;
};
emu32 emuDSP::getMaskY()
{
	return mask_Y;
};
