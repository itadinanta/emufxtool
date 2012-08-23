#ifndef _ASMDEFS_H
#define _ASMDEFS_H
#include "config.h"
typedef unsigned long emu32;
typedef unsigned short emu16;
typedef unsigned char emu8;

#define V_VALUE    0x00000001L
#define V_ADDRESS  0x00000002L
#define V_READ     0x00000004L
#define V_WRITE    0x00000008L
#define V_FXBUS    0x00000010L
#define V_EXTIN    0x00000020L
#define V_EXTOUT   0x00000040L
#define V_SPEC     0x00000080L
#define V_GPR      0x00000100L
#define V_TRAMD    0x00000200L
#define V_TRAMA    0x00000400L
#define V_LABEL    0x00000800L
#define V_PRIVATE  0x00001000L
#define V_FIXED    0x00002000L
#define V_ARRAY    0x00004000L
#define V_MACRO    0x00008000L
#define V_DELAY    0x00010000L
#define V_LOOKUP   0x00020000L
#define V_LINE     0x00040000L
#define V_TRAM     0x00080000L
#define V_STEREO   0x00100000L
#define V_ONOFF    0x00200000L
#define V_ALIAS    0x00400000L
#define V_LINEDATA 0x00800000L
#define V_LINEADDR 0x01000000L
#define V_CONTROL  0x02000000L
#define V_LEAF     0x04000000L
#define V_USER     0x80000000L

#define V_ALL ((unsigned long)(~0L))
#define V_REGISTER V_ADDRESS
#define V_INITIALIZER (V_FXBUS|V_EXTIN|V_EXTOUT|V_SPEC|V_GPR|V_TRAMD|V_TRAMA)
#define V_CONSTANT (V_VALUE|V_READ)

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

#define _CC_REG_NORMALIZED 0x001
#define _CC_REG_BORROW   0x002
#define _CC_REG_MINUS	0x004
#define _CC_REG_ZERO	0x008
#define _CC_REG_SATURATE	0x010
#define _CC_REG_NONZERO	0x100

#define MAX_POS 0x7fffffff

#endif
