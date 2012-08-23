#ifdef __CPLUSPLUS__
extern "C" {
#endif
#include <asm/ioctl.h>
#include <fcntl.h>
#include <alsa/hwdep.h>
#ifdef __CPLUSPLUS__
}
#endif
#include <patch.h>
#include <stdio.h>
#include <fx8010asm.h>
#include <chunkfile.h>
#include <ctype.h>
#include <__strdup.h>
class emuReverseSymbolTable:public map < emu16,
  patchString > {
	emuDSP *D;
      public:
	  emuReverseSymbolTable(emuDSP * dsp = 0);
	patchString getString(emu16 addr);
	void addSymbol(emu16 addr, patchString sym);
	int loadFromTextFile(FILE * f);
};

emuReverseSymbolTable::emuReverseSymbolTable(emuDSP * dsp):D(dsp)
{

	// /* Hardware constants */
	// 
	// // addSymbol(0x040,"0x0");
	// // addSymbol(0x041,"0x1");
	// addSymbol(0x042,"0x2");
	// addSymbol(0x043,"0x3");
	// addSymbol(0x044,"0x4");
	// addSymbol(0x045,"0x8");
	// addSymbol(0x046,"0x10");
	// addSymbol(0x047,"0x20");
	// addSymbol(0x048,"0x100");
	// addSymbol(0x049,"0x10000");
	// addSymbol(0x04a,"0x80000");
	// addSymbol(0x04b,"0x10000000");
	// addSymbol(0x04c,"0x20000000");
	// addSymbol(0x04d,"0x40000000");
	// addSymbol(0X04e,"MIN_NEG");
	// addSymbol(0X04f,"MAX_POS");
	// addSymbol(0x050,"(-1)");
	// addSymbol(0x051,"(-2)");
	// addSymbol(0x052,"0xc0000000");
	// addSymbol(0x053,"0x4f1bbcdc");
	// addSymbol(0x054,"0x5a7ef9db");
	// addSymbol(0x055,"0x00100000");
	// 
	// /*FX_BUSes*/
	// addSymbol(0X000,"FXBUS_PCM_LEFT");
	// addSymbol(0X001,"FXBUS_PCM_RIGHT");
	// addSymbol(0X002,"FXBUS_PCM_LEFT_REAR");
	// addSymbol(0X003,"FXBUS_PCM_RIGHT_REAR");
	// addSymbol(0X004,"FXBUS_MIDI_LEFT");
	// addSymbol(0X005,"FXBUS_MIDI_RIGHT");
	// addSymbol(0X006,"FXBUS_PCM_CENTER");
	// addSymbol(0X007,"FXBUS_PCM_LFE");
	// addSymbol(0X00c,"FXBUS_MIDI_REVERB");
	// addSymbol(0X00d,"FXBUS_MIDI_CHORUS");
	// /* Inputs */
	// addSymbol(0X010,"EXTIN_AC97_L"); /* AC'97 capture channel - left */
	// addSymbol(0X011,"EXTIN_AC97_R"); /* AC'97 capture channel - right */
	// addSymbol(0X012,"EXTIN_SPDIF_CD_L"); /* internal S/PDIF CD - onboard 
	// 
	// - left */
	// addSymbol(0X013,"EXTIN_SPDIF_CD_R"); /* internal S/PDIF CD - onboard 
	// 
	// - right */
	// addSymbol(0X014,"EXTIN_ZOOM_L"); /* Zoom Video I2S - left */
	// addSymbol(0X015,"EXTIN_ZOOM_R"); /* Zoom Video I2S - right */
	// addSymbol(0X016,"EXTIN_TOSLINK_L"); /* LiveDrive - TOSLink Optical - 
	// 
	// left */
	// addSymbol(0X017,"EXTIN_TOSLINK_R"); /* LiveDrive - TOSLink Optical - 
	// 
	// right */
	// addSymbol(0X018,"EXTIN_LINE1_L"); /* LiveDrive - Line/Mic 1 - left
	// */
	// addSymbol(0X019,"EXTIN_LINE1_R"); /* LiveDrive - Line/Mic 1 - right
	// */
	// addSymbol(0X01a,"EXTIN_COAX_SPDIF_L"); /* LiveDrive - Coaxial S/PDIF 
	// 
	// - left */
	// addSymbol(0X01b,"EXTIN_COAX_SPDIF_R"); /* LiveDrive - Coaxial S/PDIF 
	// 
	// - right */
	// addSymbol(0X01c,"EXTIN_LINE2_L"); /* LiveDrive - Line/Mic 2 - left
	// */
	// addSymbol(0X01d,"EXTIN_LINE2_R"); /* LiveDrive - Line/Mic 2 - right
	// */
	// /*Outputs*/
	// addSymbol(0X020,"EXTOUT_AC97_L"); /* AC'97 playback channel - left
	// */
	// addSymbol(0X021,"EXTOUT_AC97_R"); /* AC'97 playback channel - right
	// */
	// addSymbol(0X022,"EXTOUT_TOSLINK_L"); /* LiveDrive - TOSLink Optical
	// - left */
	// addSymbol(0X023,"EXTOUT_TOSLINK_R"); /* LiveDrive - TOSLink Optical
	// - right */
	// addSymbol(0X024,"EXTOUT_CENTER"); /* SB Live 5.1 - center */
	// addSymbol(0X025,"EXTOUT_LFE"); /* SB Live 5.1 - LFE */
	// addSymbol(0X026,"EXTOUT_HEADPHONE_L"); /* LiveDrive - Headphone -
	// left */
	// addSymbol(0X027,"EXTOUT_HEADPHONE_R"); /* LiveDrive - Headphone -
	// right */
	// addSymbol(0X028,"EXTOUT_REAR_L"); /* Rear channel - left */
	// addSymbol(0X029,"EXTOUT_REAR_R"); /* Rear channel - right */
	// addSymbol(0X02a,"EXTOUT_ADC_CAP_L"); /* ADC Capture buffer - left */
	// addSymbol(0X02b,"EXTOUT_ADC_CAP_R"); /* ADC Capture buffer - right
	// */
	// addSymbol(0X02c,"EXTOUT_MIC_CAP"); /* MIC Capture buffer */
	// addSymbol(0X031,"EXTOUT_ACENTER"); /* Analog Center */
	// addSymbol(0X032,"EXTOUT_ALFE"); /* Analog LFE */
	// /*Special Regs*/
	// addSymbol(0x056,"ACCUM");
	// addSymbol(0x057,"CCR");
	// addSymbol(0x058,"NOISE");
	// addSymbol(0x059,"NOISE4096");
	// addSymbol(0x05a,"IRQ");
	// addSymbol(0x05b,"DBAC");
	// 
};

// void emuReverseSymbolTable::initEmuFX() {
// 
// }

int emuReverseSymbolTable::loadFromTextFile(FILE * f)
{
	unsigned int count = 0;
	if (f) {
		static const int buf_size = 1024;
		emu16 value = 0;
		char *sym = (char *) malloc(buf_size + 10);
		char *buf = (char *) malloc(buf_size + 10);
		while (!feof(f)) {
			memset(buf, 0, buf_size);
			value = 0;
			fgets(buf, buf_size - 1, f);

			if (!*buf || *buf == '#') {
				// Ignore empty lines and comment lines
			}
			else if (sscanf(buf, "%x%s", &value, sym) == 2) {
				addSymbol(value, sym);
				count++;
			}
			else
				fprintf(stderr,
					"loading symbol table: bad input line: %s\n");
		}
		free(sym);
		free(buf);
	}
	return count;
}

patchString emuReverseSymbolTable::getString(emu16 addr)
{
	iterator f;
	char outmsg[128];
	memset(outmsg, 0, sizeof(outmsg));
	if ((f = find(addr)) != end())
		return f->second;
	else if (D) {
		if (D->isGPR(addr))
			sprintf(outmsg, "_GPR_%02x", addr - GPR_BASE);
		else if (D->isLine(addr))
			sprintf(outmsg, "_TRAM_%02x", addr - TRAMD_BASE);
		else if (D->isLine(addr - D->getLineAddressOffset()))
			sprintf(outmsg, "_TRAMA_%02x", addr - TRAMA_BASE);
		else if (D->isILine(addr))
			sprintf(outmsg, "_ITRAM_%02x", addr - TRAMD_BASE);
		else if (D->isILine(addr - D->getLineAddressOffset()))
			sprintf(outmsg, "_ITRAMA_%02x", addr - TRAMA_BASE);
		else
			sprintf(outmsg, "?0x%03x", addr);
	}
	else {
		sprintf(outmsg, "?0x%03x", addr);
	}
	return patchString(outmsg);
}

void emuReverseSymbolTable::addSymbol(emu16 addr, patchString sym)
{
	(*this)[addr] = sym;
}

emuImage::emuImage()
{
	current_gpr = DSP.getFirstGPR();
	current_line = DSP.getFirstLine();
	current_tram = 0;
	current_tram_size = 0;
	Constants[0] = 0x040;
	Constants[1] = 0x041;
	Constants[2] = 0x042;
	Constants[3] = 0x043;
	Constants[4] = 0x044;
	Constants[0x8] = 0x045;
	Constants[0x10] = 0x046;
	Constants[0x20] = 0x047;
	Constants[0x100] = 0x048;
	Constants[0x10000] = 0x049;
	Constants[0x80000] = 0x04a;
	Constants[0x10000000] = 0x04b;
	Constants[0x20000000] = 0x04c;
	Constants[0x40000000] = 0x04d;
	Constants[0x80000000] = 0x04e;
	Constants[0x7fffffff] = 0x04f;
	Constants[0xffffffff] = 0x050;
	Constants[0xfffffffe] = 0x051;
	Constants[0xc0000000] = 0x052;
	Constants[0x4f1bbcdc] = 0x053;
	Constants[0x5a7ef9db] = 0x054;
	Constants[0x00100000] = 0x055;
}

int emuImage::NewVariable(emu32 value, emu32 flags = 0)
{
	flags |= V_READ | V_WRITE;
	int C = NewRegister(current_gpr, value, flags);
	current_gpr = C + 1;
	return C;
}

int emuImage::NewTram(emu32 size, emu32 flags)
{
	Trams.push_back(emuTramImage(current_tram_size, size, flags));
	current_tram_size += size;
	return current_tram++;
}

emu32 emuImage::getTramSize()
{
	emu32 tram_size = 0;
	for (emuTramList::iterator i = Trams.begin(); i != Trams.end(); i++)
		tram_size += i->size;
	return tram_size;
}

int emuImage::NewLine(emu32 offset, emu32 flags)
{
	flags |= V_READ | V_WRITE;
	int C = NewRegister(current_line, 0, flags);
	Registers[C + 0x100] = emuRegisterImage(offset, flags);
	current_line = C + 1;
	return C;
}

int emuImage::NewRegister(int base_addr, emu32 value, emu32 flags)
{
	while (Registers.find(base_addr) != Registers.end())
		base_addr++;
	Registers[base_addr] = emuRegisterImage(base_addr, value, flags);
	return base_addr;
}

emu32 emuImage::getValue(emu16 addr)
{
	emuRegisterMap::iterator R;
	if ((R = Registers.find(addr)) != Registers.end())
		return R->second.value;
	for (emuConstMap::iterator C = Constants.begin(); C != Constants.end();
	     C++)
		if (addr == C->second)
			return C->first;
	return 0;
}

int emuImage::MapConstant(emu32 constant)
{
	emuConstMap::iterator i;
	if ((i = Constants.find(constant)) != Constants.end())
		return i->second;
	int C = NewRegister(current_gpr, constant, V_READ);
	Constants[constant] = C;
	current_gpr = C + 1;
	return C;
}

emuCode::emuCode(emu16 p, emu8 opcode, emu16 R, emu16 A, emu16 X, emu16 Y, emuDSP * D):pc(p)
{
	join(opcode, R, A, X, Y, D);
}

emuCode::emuCode(emu16 p, emu32 hiword, emu32 loword):pc(p), hi(hiword),
lo(loword)
{
}

void emuCode::split(emu8 * opcode, emu16 * R, emu16 * A, emu16 * X, emu16 * Y,
		    emuDSP * D)
{
	D->splitInstr(hi, lo, opcode, R, A, X, Y);
}

void emuCode::join(emu8 opcode, emu16 R, emu16 A, emu16 X, emu16 Y, emuDSP * D)
{
	D->joinInstr(&hi, &lo, opcode, R, A, X, Y);
}

void emuCode::relocate(emuRelocationMap * m, emuDSP * D)
{
	emu8 opcode;
	emu16 R,
		A,
		X,
		Y;
	split(&opcode, &R, &A, &X, &Y, D);
	R = m->relocate(R);
	A = m->relocate(A);
	X = m->relocate(X);
	Y = m->relocate(Y);
	join(opcode, R, A, X, Y, D);
}

const char *emuCode::comment(char *msg)
{
	if (!msg)
		return 0;
	/*
	 * emu8 opcode; emu16 R,A,X,Y; split(&opcode,&R,&A,&X,&Y);
	 * emu_fx_unasm_line(1,opcode,R,A,X,Y, msg); return msg; 
	 */
	return msg;
}

emuControl::emuControl(const char *ctrlname, emu32 Flags, emu16 gpr, emu16 n,
		       emu32 min, emu32 max, emu8 ttab, emu8 idx)
{
	flags = Flags;
	first_gpr = gpr;
	range_min = min;
	range_max = max;
	ngprs = n;
	index = idx;
	name = __strdup(ctrlname);
	initial_values = 0;
	ttable = ttab;
}

emuControl::emuControl(const emuControl & E)
{
	copy(E);
}

emuControl::~emuControl()
{
	free(name);
	free(initial_values);
};
const char *emuControl::getTTableStr()
{
	static const char *ttd[] =
		{ "none", "t100", "treble", "bass", "onoff" };
	if ((int) ttable < sizeof(ttd) / sizeof(const char *))
		return ttd[ttable];
	else
		return ttd[0];
}

const char *emuControl::getName()
{
	return name;
}
emu32 emuControl::getValue(int i)
{
	if (initial_values && i >= 0 && i < ngprs)
		return initial_values[i];
	else
		return 0;
}
emu16 emuControl::getGPR()
{
	return first_gpr;
}
emu32 emuControl::getFlags()
{
	return flags;
}
emu16 emuControl::getNValues()
{
	return ngprs;
}
emu32 emuControl::getRangeMin()
{
	return range_min;
}
emu32 emuControl::getRangeMax()
{
	return range_max;
}
emu8 emuControl::getTTable()
{
	return ttable;
}
emu8 emuControl::getIndex()
{
	return index;
}
const emuControl & emuControl::operator=(const emuControl & e)
{
	if (initial_values)
		free(initial_values);
	if (name)
		free(name);
	copy(e);
	return *this;
}

void emuControl::copy(const emuControl & E)
{
	flags = E.flags;
	first_gpr = E.first_gpr;
	range_min = E.range_min;
	range_max = E.range_max;
	index = E.index;
	ngprs = E.ngprs;
	ttable = E.ttable;
	name = __strdup(E.name);
	if (ngprs && E.initial_values) {
		initial_values = (emu32 *) malloc(ngprs * sizeof(emu32));
		memcpy(initial_values, E.initial_values, ngprs * sizeof(emu32));
	}
	else
		initial_values = 0;
}

emuExport::emuExport(const char *sym_name, emu32 fl, emu16 addr, emu16 n)
{
	flags = fl;
	address = addr;
	count = n;
	symbol = __strdup(sym_name);
}

emuExport::emuExport(const emuExport & E)
{
	copy(E);
}

emuExport::~emuExport()
{
	free(symbol);
}

const char *emuExport::getSymbol()
{
	return symbol;
}
emu16 emuExport::getAddress()
{
	return address;
}
emu32 emuExport::getFlags()
{
	return flags;
}
emu16 emuExport::getCount()
{
	return count;
}

const emuExport & emuExport::operator=(const emuExport & e)
{
	if (symbol)
		free(symbol);
	copy(e);
	return *this;
}

void emuExport::copy(const emuExport & E)
{
	flags = E.flags;
	address = E.address;
	count = E.count;
	symbol = __strdup(E.symbol);
}

emuPatch::emuPatch()
{
}
emuPatch::~emuPatch()
{
}

#define CH_HDR 1
#define CH_INIT 2
#define CH_COMMENT 3
#define CH_CODE 4
#define CH_CONTROLS 5
#define CH_SYMBOLS 6
#define CH_TRAMS 7
#define CH_DSP 8
#define SUB_MAGIC 0x31306b31L
int emuPatch::Save(const char *filename)
{
	try {
		chunkArchive F(new chunkStreamFILE(filename, "wb"));
		F.newStream(SUB_MAGIC);
		// Header
		F.newChunk(CH_HDR);
		F.writeByte(1);
		F.writeByte(0);
		F.writeByte(0);	// Version MAJ.MIN.PATCH
		F.writeString("This should be the name of the patch");
		F.writeString(filename);
		F.closeChunk();

		F.newChunk(CH_DSP);
		F.writeLong(Image.DSP.getFirstGPR());
		F.writeLong(Image.DSP.getGPRCount());
		F.writeLong(Image.DSP.getFirstILine());
		F.writeLong(Image.DSP.getILineCount());
		F.writeLong(Image.DSP.getFirstLine());
		F.writeLong(Image.DSP.getLineCount());
		F.writeLong(Image.DSP.getLineAddressOffset());
		F.writeLong(Image.DSP.getCodeCount());
		F.writeLong(Image.DSP.getShiftOpcode());
		F.writeLong(Image.DSP.getShiftR());
		F.writeLong(Image.DSP.getShiftA());
		F.writeLong(Image.DSP.getShiftX());
		F.writeLong(Image.DSP.getShiftY());
		F.writeLong(Image.DSP.getMaskOpcode());
		F.writeLong(Image.DSP.getMaskR());
		F.writeLong(Image.DSP.getMaskA());
		F.writeLong(Image.DSP.getMaskX());
		F.writeLong(Image.DSP.getMaskY());
		F.closeChunk();

		// Comments
		F.newChunk(CH_COMMENT);
		F.writeString
			("Compiled for emufxtool by Nicola Orru' (nigu@itadinanta.it)");
		F.writeString
			("Development version. Not yet suitable for production use, format may change in the future");
		F.writeString("");
		F.closeChunk();

		// Gprs
		F.newChunk(CH_INIT);
		F.writeShort(Image.Registers.size());
		for (emuRegisterMap::iterator R = Image.Registers.begin();
		     R != Image.Registers.end(); R++) {
			F.writeShort(R->first);
			F.writeLong(R->second.value);
			F.writeLong(R->second.flags);
		}
		F.closeChunk();

		// Instructions
		F.newChunk(CH_CODE);
		F.writeShort(Image.Code.size());
		for (emuCodeList::iterator I = Image.Code.begin();
		     I != Image.Code.end(); I++) {
			F.writeLong(I->lo);
			F.writeLong(I->hi);
		}
		F.closeChunk();

		// Controls
		F.newChunk(CH_CONTROLS);
		F.writeShort(Image.Controls.size());
		for (emuControlList::iterator I = Image.Controls.begin();
		     I != Image.Controls.end(); I++) {
			F.writeString(I->getName());
			F.writeLong(I->getFlags());
			F.writeShort(I->getGPR());
			F.writeShort(I->getNValues());
			F.writeLong(I->getRangeMin());
			F.writeLong(I->getRangeMax());
			F.writeByte(I->getTTable());
			F.writeByte(I->getIndex());
		}
		F.closeChunk();

		// Trams (?)
		F.newChunk(CH_TRAMS);
		F.closeChunk();
		// Exports
		F.newChunk(CH_SYMBOLS);
		F.writeShort(Image.Exports.size());
		for (emuExportList::iterator I = Image.Exports.begin();
		     I != Image.Exports.end(); I++) {
			F.writeString(I->getSymbol());
			F.writeLong(I->getFlags());
			F.writeShort(I->getAddress());
			F.writeShort(I->getCount());
		}
		F.closeChunk();

		F.closeStream();

		return 1;
	}
	catch(...) {
		return 0;
	}
}

int emuPatch::Load(const char *filename)
{
	chunkArchive F(new chunkStreamFILE(filename, "rb"));
	unsigned short chunk_id;
	unsigned short i,
		n;
	char *Text;
	if (F.validateStream(SUB_MAGIC)) {
		while ((chunk_id = F.nextChunk()) != F.EOC) {
			switch (chunk_id) {
			case CH_HDR:
				F.beginChunk();
				F.endChunk();
				break;
			case CH_INIT:
				Image.Registers.clear();
				F.beginChunk();
				n = F.readShort();
				for (i = 0; i < n; i++) {
					unsigned short base_addr =
						F.readShort();
					unsigned long value = F.readLong();
					unsigned long flags = F.readLong();
					Image.Registers[base_addr] =
						emuRegisterImage(base_addr,
								 value, flags);
				}
				F.endChunk();
				break;
			case CH_CODE:
				F.beginChunk();
				Image.Code.clear();
				n = F.readShort();
				for (i = 0; i < n; i++) {
					unsigned long lo = F.readLong();
					unsigned long hi = F.readLong();
					Image.Code.
						push_back(emuCode(i, hi, lo));
				}
				F.endChunk();
				break;
			case CH_CONTROLS:
				F.beginChunk();
				Image.Controls.clear();
				n = F.readShort();
				for (i = 0; i < n; i++) {
					char *name = F.readString();
					unsigned long flags = F.readLong();
					unsigned short gpr = F.readShort();
					unsigned short n = F.readShort();
					unsigned long min = F.readLong();
					unsigned long max = F.readLong();
					unsigned char ttable = F.readByte();
					unsigned char index = F.readByte();
					Image.Controls.
						push_back(emuControl
							  (name, flags, gpr, n,
							   min, max, ttable,
							   index));
					F.freeBlock(name);
				}
				F.endChunk();
				break;
			case CH_SYMBOLS:
				F.beginChunk();
				Image.Exports.clear();
				n = F.readShort();
				for (i = 0; i < n; i++) {
					char *name = F.readString();
					unsigned long flags = F.readLong();
					unsigned short addr = F.readShort();
					unsigned short n = F.readShort();
					Image.Exports.
						push_back(emuExport
							  (name, flags, addr,
							   n));
					F.freeBlock(name);
				}
				F.endChunk();
				break;
			case CH_COMMENT:
				F.beginChunk();
				while (1) {
					Text = F.readString();
					if (!*Text) {
						F.freeBlock(Text);
						break;
					}
					printf("/* COMMENT: %s */\n", Text);
					F.freeBlock(Text);
				}
				F.endChunk();
				break;
			default:
				F.skipChunk();
			}
		}
		return 1;
	}
	return 0;
}

int emuPatch::unassemble(FILE * f, const char *symbtab_file)
{
	Image.unassemble(f, symbtab_file);
}

int emuImage::NewInstruction(emu8 op, emu16 r, emu16 a, emu16 x, emu16 y)
{
	Code.push_back(emuCode(Code.size(), op, r, a, x, y, &DSP));
	return Code.size() - 1;
}

int emuImage::NewControl(const char *ctrlname, emu32 Flags, emu16 gpr,
			 emu16 count, emu32 min, emu32 max, emu8 ttable)
{
	Controls.
		push_back(emuControl
			  (ctrlname, Flags, gpr, count, min, max, ttable));
	return Controls.size() - 1;
}

int emuImage::NewExport(const char *symbol_name, emu32 flags, emu16 addr,
			emu16 count)
{
	Exports.push_back(emuExport(symbol_name, flags, addr, count));
	return Exports.size() - 1;
}

void emuImage::Clear()
{
	Registers.clear();
	Constants.clear();
	Trams.clear();
	Code.clear();
	Controls.clear();
	Exports.clear();
}

extern const char *V_BIT_NAMES[];
patchString emuImage::unAsm(emuCode * C, emuReverseSymbolTable * ST,
			    emuReverseSymbolTable * LBL)
{
	static const char *opcodes[] = {
		"MACS",
		"MACNS",
		"MACW",
		"MACNW",
		"MACINTS",
		"MACINTW",
		"ACC3",
		"MACMV",
		"ANDXOR",
		"TSTNEG",
		"LIMITGE",
		"LIMITL",
		"LOG",
		"EXP",
		"INTERP",
		"SKIP",
		"UNKNOWN"
	};

	emu8 opcode;
	emu16 R,
		A,
		X,
		Y;
	C->split(&opcode, &R, &A, &X, &Y, &DSP);
	patchString strOpcode(opcodes[opcode & 0x0f]);
	patchString oR = ST->getString(R);
	patchString oA = ST->getString(A);
	patchString oX = ST->getString(X);
	patchString oY = ST->getString(Y);

	if (opcode == INSTR_SKIP) {
		emu16 addr = C->getPC() + getValue(Y) + 1;
		emuReverseSymbolTable::iterator L = LBL->find(addr);
		if (L != LBL->end())
			oY = emuString(":" + L->second);
	}
	return strOpcode + " " + oR + ", " + oA + ", " + oX + ", " + oY;
}

int emuImage::unassemble(FILE * F, const char *symtab)
{
	emu8 opcode;
	emu16 R,
		A,
		X,
		Y;
	emu32 high,
		low;
	emuRegisterMap::iterator rg;

	int i;
	int PC = 0;
	fprintf(F, ".include \"reverse.emu10k1\"\n");
	emuReverseSymbolTable S(&DSP);
	emuReverseSymbolTable L(&DSP);

	for (emuExportList::iterator I = Exports.begin(); I != Exports.end();
	     I++) {
		if (I->getCount() == 1)
			S.addSymbol(I->getAddress(), I->getSymbol());
		else
			for (i = 0; i < I->getCount(); i++)
				S.addSymbol(I->getAddress() + i,
					    I->getSymbol());
	}

	FILE *f;
	if (symtab && (f = fopen(symtab, "rt"))) {
		S.loadFromTextFile(f);
		fclose(f);
	}

	for (rg = Registers.begin(); rg != Registers.end(); rg++) {
		rg->second.flags &= ~(V_USER | V_WRITE);
	}
	// remove unused registers from the list
	// find constant registers
	int nlabels = 0;
	for (emuCodeList::iterator I = Code.begin(); I != Code.end(); I++) {
		emu8 opcode;
		emu16 R,
			A,
			X,
			Y;
		I->split(&opcode, &R, &A, &X, &Y, &DSP);
		if ((rg = Registers.find(R)) != Registers.end())
			rg->second.flags |= (V_USER | V_WRITE);
		if ((rg = Registers.find(A)) != Registers.end())
			rg->second.flags |= V_USER;
		if ((rg = Registers.find(X)) != Registers.end())
			rg->second.flags |= V_USER;
		if ((rg = Registers.find(Y)) != Registers.end())
			rg->second.flags |= V_USER;
		if (opcode == INSTR_SKIP) {
			char msg[128];
			emu16 addr = I->getPC() + getValue(Y) + 1;
			sprintf(msg, "_L_%03d", nlabels);
			if (L.find(addr) == L.end()) {
				L.addSymbol(addr, msg);
				nlabels++;
			}
		}
	}
	for (emuControlList::iterator I = Controls.begin(); I != Controls.end();
	     I++) {
		fprintf(F, "control");

		if ((rg = Registers.find(I->getGPR())) != Registers.end())
			rg->second.flags &= ~V_USER;
		if (I->getFlags() & V_ONOFF)
			fprintf(F, " onoff");
		else
			fprintf(F, " range");
		if (I->getFlags() & V_STEREO) {
			fprintf(F, " stereo");
			fprintf(F, " %s = {0x%08x, 0x%08x}",
				S.getString(I->getGPR()).data(),
				Registers[I->getGPR()].value,
				Registers[I->getGPR() + 1].value);
			if ((rg =
			     Registers.find(I->getGPR() + 1)) !=
			    Registers.end())
				rg->second.flags &= ~V_USER;
			S.addSymbol(I->getGPR() + 1,
				    S.getString(I->getGPR()) + ".right");
			S.addSymbol(I->getGPR(),
				    S.getString(I->getGPR()) + ".left");
		}
		else {
			fprintf(F, " mono");
			fprintf(F, " %s = 0x%08x",
				S.getString(I->getGPR()).data(),
				Registers[I->getGPR()].value);

			if ((rg =
			     Registers.find(I->getGPR() + 1)) !=
			    Registers.end())
				rg->second.flags &= ~V_USER;
			S.addSymbol(I->getGPR(), S.getString(I->getGPR()));
		}
		if (I->getFlags() & V_ONOFF)
			fprintf(F,
				" {name=\"%s\", index=%d, translation=\"%s\"};\n",
				I->getName(), I->getIndex(), I->getTTableStr());
		else
			fprintf(F,
				" {name=\"%s\", min=%d, max=%d, index=%d, translation=\"%s\"};\n",
				I->getName(), I->getRangeMin(),
				I->getRangeMax(), I->getIndex(),
				I->getTTableStr());
	}

	for (emuRegisterMap::iterator R = Registers.begin();
	     R != Registers.end(); R++) {
		if (R->second.flags & V_USER) {
			if ((R->second.flags & V_LINE)
			    && (R->second.flags & V_TRAMD))
				fprintf(F, "line %s = 0x%08x;\n",
					S.getString(R->first).data(),
					R->second.value);

			else if (R->second.flags & V_TRAMA) {
			}
			else if (R->second.flags & V_GPR) {
				if (R->second.flags & V_WRITE)
					fprintf(F, "var %s = 0x%08x;\n",
						S.getString(R->first).data(),
						R->second.value);

				else {
					char msg[128];
					sprintf(msg, "0x%x", R->second.value);
					S.addSymbol(R->first, msg);
				}
			}
		}
	}
	for (emuCodeList::iterator I = Code.begin(); I != Code.end(); I++) {
		emuReverseSymbolTable::iterator LBL = L.find(I->getPC());
		if (LBL != L.end())
			fprintf(F, "%s:\t", LBL->second.data());
		else
			fprintf(F, "\t");
		fprintf(F, "/* %04x %08x-%08x */", I->getPC(), I->hi, I->lo);
		fprintf(F, "\t%s;\n", unAsm(&(*I), &S, &L).data());
	}
	return 0;
}

int emuPatch::hwSet(const char *device)
{

	snd_hwdep_t *hw_fd;
	emu10k1_fx8010_code_t code;
	emu10k1_fx8010_control_gpr_t *ctl;
	int dsp_fd;
	unsigned long tram_size = 0;
	emu32 f,
		PC,
		i,
		j;

	memset(&code, 0, sizeof(code));
	code.gpr_add_control_count = Image.Controls.size();
	if (code.gpr_add_control_count) {
		code.gpr_add_controls = (emu10k1_fx8010_control_gpr_t *)
			malloc(sizeof(emu10k1_fx8010_control_gpr_t) *
			       code.gpr_add_control_count);
		memset(code.gpr_add_controls, 0,
		       sizeof(emu10k1_fx8010_control_gpr_t) *
		       code.gpr_add_control_count);
		i = 0;
		for (emuControlList::iterator L = Image.Controls.begin();
		     L != Image.Controls.end(); L++, i++) {
			ctl = &(code.gpr_add_controls[i]);
			ctl->id.index = L->getIndex();
			ctl->id.iface = SNDRV_CTL_ELEM_IFACE_MIXER;
			strcpy((char *) ctl->id.name, L->getName());
			// Blablabla
			ctl->vcount = ctl->count = L->getNValues();
			ctl->min = L->getRangeMin();
			ctl->max = L->getRangeMax();
			switch ((emuControl::emuTTable) L->getTTable()) {
			default:
				ctl->translation = EMU10K1_GPR_TRANSLATION_NONE;
				break;
				// FIXME: typo in *_TRANSLATION_ defs
			case emuControl::t100:
				ctl->translation =
					EMU10K1_GPR_TRANSLATION_TABLE100;
				break;
			case emuControl::bass:
				ctl->translation = EMU10K1_GRP_TRANSLATION_BASS;
				break;
			case emuControl::treble:
				ctl->translation =
					EMU10K1_GRP_TRANSLATION_TREBLE;
				break;
			case emuControl::onoff:
				ctl->translation =
					EMU10K1_GPR_TRANSLATION_ONOFF;
				break;
			}
			for (j = 0; j < L->getNValues(); j++) {
				ctl->gpr[j] = L->getGPR() + j;
				ctl->value[j] =
					Image.Registers[L->getGPR() + j].value;
			}
		}
	}
	for (emuRegisterMap::iterator M = Image.Registers.begin();
	     M != Image.Registers.end(); M++) {
		f = M->second.flags;
		if (f & V_LINEDATA) {
			i = M->first - Image.DSP.getFirstLine();
			emu_fx_set_bit(code.tram_valid, i);
			code.tram_data_map[i] = M->second.value;
		}
		else if (f & V_LINEADDR) {
			i = M->first - (Image.DSP.getFirstLineAddress());
			emu_fx_set_bit(code.tram_valid, i);
			code.tram_addr_map[i] = M->second.value;
		}
		else if (f & V_GPR) {
			i = M->first - Image.DSP.getFirstGPR();
			emu_fx_set_bit(code.gpr_valid, i);
			code.gpr_map[i] = M->second.value;
		}
	}
	PC = 0;
	for (emuCodeList::iterator C = Image.Code.begin();
	     C != Image.Code.end(); C++, PC++) {
		emu_fx_set_bit(code.code_valid, PC);
		code.code[PC][1] = C->hi;
		code.code[PC][0] = C->lo;
	}
	if (snd_hwdep_open
	    (&hw_fd, device ? device : "hw:0,0", SND_HWDEP_OPEN_WRITE) >= 0) {
		// snd_hwdep_ioctl(hw_fd,
		// SNDRV_EMU10K1_IOCTL_TRAM_SETUP,(void*)Image.getTramSize());
		snd_hwdep_ioctl(hw_fd, SNDRV_EMU10K1_IOCTL_CODE_POKE, &code);

		if (code.gpr_add_control_count)
			free(code.gpr_add_controls);
		snd_hwdep_close(hw_fd);
		return 1;
	}

	return 0;
}

int emuPatch::hwGet(const char *device)
{

	snd_hwdep_t *hw_fd;
	emu10k1_fx8010_code_t code;
	emu10k1_fx8010_info_t info;
	int ncontrols = 0;
	int dsp_fd;
	if (snd_hwdep_open
	    (&hw_fd, device ? device : "hw:0,0", SND_HWDEP_OPEN_READ) >= 0) {
		memset(&code, 0, sizeof(code));
		if (snd_hwdep_ioctl(hw_fd, SNDRV_EMU10K1_IOCTL_INFO, &info)) {
			memset(&info, 0, sizeof(info));
		}
		else {
			code.gpr_list_control_count = 64;
			code.gpr_list_controls =
				(emu10k1_fx8010_control_gpr_t *)
				malloc(sizeof(emu10k1_fx8010_control_gpr_t) *
				       64);
		}

		if (!snd_hwdep_ioctl
		    (hw_fd, SNDRV_EMU10K1_IOCTL_CODE_PEEK, &code)) {
			emu8 opcode;
			emu16 R,
				A,
				X,
				Y;
			emu32 high,
				low;
			int i;

			for (i = 0; i < NINSTR; i++)
				if (emu_fx_test_bit(code.code_valid, i)) {
					high = code.code[i][1];
					low = code.code[i][0];
					Image.DSP.splitInstr(high, low, &opcode,
							     &R, &A, &X, &Y);
					Image.Code.
						push_back(emuCode
							  (i, opcode, R, A, X,
							   Y, &Image.DSP));
				}
			for (i = 0; i < Image.DSP.getGPRCount(); i++)
				if (emu_fx_test_bit(code.gpr_valid, i))
					Image.Registers[i +
							Image.DSP.
							getFirstGPR()] =
						(emuRegisterImage
						 (i + Image.DSP.getFirstGPR(),
						  code.gpr_map[i],
						  V_GPR | V_READ | V_WRITE |
						  V_ADDRESS));

			for (i = 0; i < Image.DSP.getLineCount(); i++)
				if (emu_fx_test_bit(code.tram_valid, i)) {
					Image.Registers[i +
							Image.DSP.
							getFirstLineAddress()] =
						emuRegisterImage(i +
								 Image.DSP.
								 getFirstLineAddress
								 (),
								 code.
								 tram_addr_map
								 [i],
								 V_GPR | V_LINE
								 | V_TRAMA |
								 V_READ |
								 V_WRITE |
								 V_ADDRESS);
					Image.Registers[i +
							Image.DSP.
							getFirstLine()] =
						emuRegisterImage(i +
								 Image.DSP.
								 getFirstLine(),
								 code.
								 tram_data_map
								 [i],
								 V_GPR | V_LINE
								 | V_TRAMD |
								 V_READ |
								 V_WRITE |
								 V_ADDRESS);
				}

			for (i = 0; i < code.gpr_list_control_total; i++) {
				emu10k1_fx8010_control_gpr_t *ctl =
					code.gpr_list_controls + i;
				emu32 flags = V_GPR;
				if (ctl->count > 1)
					flags |= V_STEREO;
				if (ctl->translation ==
				    EMU10K1_GPR_TRANSLATION_ONOFF)
					flags |= V_ONOFF;
				emuControl::emuTTable tt;
				switch (ctl->translation) {
				default:
					tt = emuControl::none;
					break;
					// FIXME: typo in *_TRANSLATION_ defs
				case EMU10K1_GPR_TRANSLATION_TABLE100:
					tt = emuControl::t100;
					break;
				case EMU10K1_GRP_TRANSLATION_BASS:
					tt = emuControl::bass;
					break;
				case EMU10K1_GRP_TRANSLATION_TREBLE:
					tt = emuControl::treble;
					break;
				case EMU10K1_GPR_TRANSLATION_ONOFF:
					tt = emuControl::onoff;
					break;
				}

				Image.Controls.
					push_back(emuControl
						  ((const char *) ctl->id.name,
						   flags,
						   ctl->gpr[0] +
						   Image.DSP.getFirstGPR(),
						   ctl->count, ctl->min,
						   ctl->max, tt,
						   ctl->id.index));
				char escaped[sizeof(ctl->id.name) + 10];
				char escaped_i[sizeof(ctl->id.name) + 10 + 12];
				const char *c = (const char *) ctl->id.name;
				char *e = escaped;
				while (*c) {
					if (*c >= 'a' && *c <= 'z')
						*e = toupper(*c);
					else if ((*c >= 'A' && *c <= 'Z')
						 || (*c >= '0' && *c <= '9'))
						*e = *c;
					else
						*e = '_';
					*c++;
					*e++;
				}
				*e = 0;
				if (ctl->id.index)
					sprintf(escaped_i, "%s_%d", escaped,
						ctl->id.index);
				else
					strcpy(escaped_i, escaped);
				if (ctl->count == 1) {
					unsigned short addr =
						(unsigned short) (ctl->gpr[0] +
								  Image.DSP.
								  getFirstGPR
								  ());
					Image.Exports.
						push_back(emuExport
							  (patchString
							   (escaped_i).data(),
							   V_GPR | V_CONTROL,
							   addr, 1));
				}
				else if (ctl->count == 2) {
					unsigned short addr =
						(unsigned short) (ctl->gpr[0] +
								  Image.DSP.
								  getFirstGPR
								  ());
					Image.Exports.
						push_back(emuExport
							  ((patchString
							    (escaped_i)).data(),
							   (unsigned
							    long) (V_GPR |
								   V_CONTROL |
								   V_STEREO),
							   addr, 2));
				}
			}
			if (code.gpr_list_controls)
				free(code.gpr_list_controls);
		}
		snd_hwdep_close(hw_fd);
		return 1;
	}

	return 0;
}
