# # Process this file with automake to produce Makefile.in.
AUTOMAKE_OPTIONS = 1.3 foreign

bin_PROGRAMS = emufxtool

emufxtool_LIBS = asound m g++

emufxtool_SOURCES = \
	excep.h asmdefs.h dsp_defs.h \
	config.h fx8010asm.h fx8010ioctl.h \
	emufxtool.C \
	scanner.C scanner.h \
	instruction.C instruction.h \
	parser.C parser.h \
	patch.C patch.h \
	dsp.C dsp.h \
	strdsp.C strdsp.h __strdup.h \
	chunkfile.C chunkfile.h

EXTRA_DIST = depcomp COPYING \
	README.tex README.txt \
	reverse.sym \
	constants.as10k1 stddef.as10k1 default.as10k1 fxbus.as10k1 \
	inout.as10k1 special.as10k1 types.as10k1 test.as10k1 reverse.emu10k1


README.dvi: README.tex
	latex README.tex

README.pdf: README.dvi
	dvipdf README.dvi

README.ps: README.dvi
	dvips README.dvi

doc: README.ps README.pdf

alsa-dist: distdir
	@rm -rf ../distdir/emufxtool
	@mkdir -p ../distdir/emufxtool
	@cp -RLpv $(distdir)/* ../distdir/emufxtool
	@rm -rf $(distdir)

