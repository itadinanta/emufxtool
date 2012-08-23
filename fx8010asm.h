#ifndef _FX8010ASM_H
#define _FX8010ASM_H
#include "fx8010ioctl.h"
#include "asmdefs.h"

#ifdef __CPLUSPLUS__
extern "C" {
#endif
	inline int emu_fx_test_bit_s(unsigned int *v, int i) {
		int idx = i / (sizeof(*v) * 8);
		int bit = i % (sizeof(*v) * 8);
		if (v[idx] & (1L << bit))
			  return 1;
		  return 0;
	} inline void emu_fx_set_bit_s(unsigned int *v, int i) {
		int idx = i / (sizeof(*v) * 8);
		int bit = i % (sizeof(*v) * 8);
		v[idx] |= (1L << bit);
	}

	inline int emu_fx_test_bit(unsigned long int *v, int i) {
		int idx = i / (sizeof(*v) * 8);
		int bit = i % (sizeof(*v) * 8);
		if (v[idx] & (1L << bit))
			return 1;
		return 0;
	}

	inline void emu_fx_set_bit(unsigned long *v, int i) {
		int idx = i / (sizeof(*v) * 8);
		int bit = i % (sizeof(*v) * 8);
		v[idx] |= (1L << bit);
	}

	char *emu_fx_flags_to_char(unsigned long flags, char *buf,
				   const char **bitnames);
	int emu_fx_translate_constant(emu16 operand, emu32 * constant);
	void emu_fx_split_instr(emu32 high, emu32 low, emu8 * opcode, emu16 * R,
				emu16 * A, emu16 * X, emu16 * Y);
	void emu_fx_join_instr(emu32 * high, emu32 * low, emu8 opcode, emu16 R,
			       emu16 A, emu16 X, emu16 Y);
	emu16 emu_fx_reloc_op(emu16 operand, emu16 gpr_base, emu16 tramd_base,
			      emu16 trama_base);
	void emu_fx_reloc_instr(emu32 * high, emu32 * low, emu16 gpr_base,
				emu16 tramd_base, emu16 trama_base);
	const char *emu_fx_sym_type(emu16 type);
	const char *emu_fx_unasm_opcode(emu32 opcode);
	const char *emu_fx_parse_CCR(char *outmsg, emu16 operand);
	const char *emu_fx_well_known_operand(char *outmsg, emu16 operand);
	emu16 emu_fx_hwconstant(emu32 constant);
	const char *emu_fx_unasm_operand(emu32 operand, emu8 opcode, emu8 ord,
					 char *outmsg);
	const char *emu_fx_unasm_line(int symout, emu8 opcode, emu16 R, emu16 A,
				      emu16 X, emu16 Y, char *outmsg);
	// void emu_fx_code_info(emu10k1_fx8010_code_t * code,
	// emu10k1_fx8010_info_t * info, emu16 unasm_opt);
#ifdef __CPLUSPLUS__
}
#endif

#endif
