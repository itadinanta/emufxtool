#ifndef _DSP_DEFS_H
#define _DSP_DEFS_H

#define UNASM_PC 1
#define UNASM_RAW 2
#define UNASM_REM 4
#define UNASM_TRAM 8
#define UNASM_GPR 16
#define UNASM_CODE 0
#define UNASM_SYM 32

#define INSTR_MACS 0
#define INSTR_MACS1 1
#define INSTR_MACW 2
#define INSTR_MACW1 3
#define INSTR_MACINTS 4
#define INSTR_MACINTW 5
#define INSTR_ACC3 6
#define INSTR_MACMV 7
#define INSTR_ANDXOR 8
#define INSTR_TSTNEG 9
#define INSTR_LIMIT 10
#define INSTR_LIMIT1 11
#define INSTR_LOG 12
#define INSTR_EXP 13
#define INSTR_INTERP 14
#define INSTR_SKIP 15

#define GPR_BASE 0x100
#define TRAMD_BASE 0x200
#define TRAMA_BASE 0x300

#define MAX_SYM_LEN 32
#define PATCH_NAME_SIZE 32

#define SIGNATURE_SIZE 16
#define NGPRS 0x100
#define NTRAM 160
#define NLINES 0x100
#define NINSTR 512
#define NSYMS 1024
#define TRAM_LOOKUP 1
#define TRAM_DELAY 2

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

struct emu_fx_tram_line {
	u8 addr;
	u16 tram;
	u8 type;
	u32 offset;
};

struct emu_fx_tram {
	u32 size;
	u8 nrlines;
	u8 nwlines;
	u8 rlines[NLINES];
	u8 wlines[NLINES];
};

struct emu_fx_dsp_properties {
	u32 card_id;
	u32 gpr_first,
	  gpr_count;
	u32 line_first,
	  line_count,
	  tram_count;
	u32 iline_first,
	  iline_count,
	  itram_count;
	u32 line_address_offset;
	u32 code_size;
	u16 shift_opcode,
	  shift_R,
	  shift_A,
	  shift_X,
	  shift_Y;
	u32 mask_opcode,
	  mask_R,
	  mask_A,
	  mask_X,
	  mask_Y;
};

struct emu_fx_symbol {
	char name[MAX_SYM_LEN];
	u16 type;
	u32 value;
	u32 address;
	u16 flags;
};

struct emu_fx_static {
	u8 address;
	u32 value;
};

struct emu_fx_control {
	u8 address;
	u32 value;
	u32 min;
	u32 max;
	u8 type;
	u8 flags;
	char name[MAX_SYM_LEN * 2 + 1];
};

struct emu_fx_constant {
	u8 address;
	u32 value;
};

struct emu_fx_dynamic {
	u8 address;
};

struct emu_fx_patch {
	int id;
	char signature[SIGNATURE_SIZE + 1];
	char name[PATCH_NAME_SIZE + 1];
	int ninout;
	u8 inout[NGPRS][2];
	int ndynamic;
	struct emu_fx_dynamic dynamicd[NGPRS];
	int nstatic;
	struct emu_fx_static staticd[NGPRS];
	int ncontrol;
	struct emu_fx_control controld[NGPRS];
	int nconstant;
	struct emu_fx_constant constantd[NGPRS];
	int ntramr,
	  ntramw;
	struct emu_fx_tram_line tramr[NLINES],
	  tramw[NLINES];
	int nlookup;
	struct emu_fx_tram lookup[NTRAM];
	int ndelay;
	struct emu_fx_tram delay[NTRAM];
	int ninstruction;
	u32 instruction[NINSTR][2];
	int nsymbol;
	struct emu_fx_symbol symbol[NSYMS];
};

#endif
