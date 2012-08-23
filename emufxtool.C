#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <parser.h>
#include <patch.h>
#include <alsa/hwdep.h>
#include <__strdup.h>
#include "fx8010asm.h"
typedef enum { OP_ASM =
		1, OP_INFO, OP_VERSION, OP_HELP, OP_TEST, OP_LOAD, OP_SUCK,
	OP_UNASSEMBLE
} tOP;
#define OPT_TOKENS emuParser::optTokenList
#define OPT_PREPROCESS emuParser::optPreprocess
#define OPT_EXPANDMACRO emuParser::optExpandMacro
#define OPT_VERBOSE 0x1000

#define DEFAULT_EXT "emufx"
#define SYMBOL_EXT "sym"

#define PROGRAM "emufxtool - emu10k1 assembler/unassembler/loader"
#define AUTHOR "Nicola Orrù"
#define VERSION_MAJOR "0"
#define VERSION_MINOR "2"
#define INFOMSG "%s\n\ntype %s -h for help\n"

unsigned short parse_subopt(const char *optarg)
{
	const char *oopt = optarg;
	unsigned short opt = 0;
	while (*oopt) {
		switch (*oopt++) {
		case 'a':
			opt = ~0;
			break;
		case 'v':
			opt |= OPT_VERBOSE;
			break;
		case 'V':
			opt &= ~OPT_VERBOSE;
			break;
		case 't':
			opt |= OPT_TOKENS;
			break;
		case 'T':
			opt &= ~OPT_TOKENS;
			break;
		case 'p':
			opt |= OPT_PREPROCESS;
			break;
		case 'P':
			opt &= ~OPT_PREPROCESS;
			break;
		case 'm':
			opt |= OPT_EXPANDMACRO;
			break;
		case 'M':
			opt &= ~OPT_EXPANDMACRO;
			break;
		}
	}
	return opt;
}

static const char *helpmsg[] = {
	"emufxtool <action> [-o dump_options] [-d device, defaults to hw:0,0] [-y symbol_table] [dsp patch files...]",
	"action: '-l' load patches into emu hardware",
	"        '-t' test patches sanity, useful with -oa, see later",
	"        '-a' assemble patches",
	"        '-h' help",
	"        '-v' version",
	"        '-u' unassemble patch found in the emu hardware",
	"        '-s' save hardware contents in a patch file",
	" ",
	"dump_options (multiple options allowed, latest override earliest):",
	"               'a' all options on (default all off)",
	"               'v' enables 'V' disables patch dump to stdout",
	"               't' enables 'T' disables token analysis dump to stdout",
	"               'p' enables 'P' disables parsing only (no files generated during compilation)",
	"               'm' enables 'M' disables macro expansion to stdout",
	""
};

int ld(const char *device, int nfiles, char *files[256], unsigned short opts,
       const char *sym, bool fake = 0)
{
	int i;
	char *last_dot,
	 *name,
	 *base_name,
	 *last_slash;
	int failed = 0;
	for (i = 0; i < nfiles; i++) {
		emuPatch P;
		name = __strdup(files[i]);
		if (P.Load(name)) {
			if (opts & OPT_VERBOSE)
				P.unassemble(stdout, sym);
			if (!fake)
				P.hwSet(device);
			free(name);
		}
		else {
			fprintf(stderr,
				"Loading of the patch named '%s' failed\n",
				files[i]);
			failed++;
		}
	}
	return failed;
}

void unasm(const char *device, unsigned short opts, const char *sym)
{
	emuPatch P;
	P.hwGet(device);
	if (opts & OPT_VERBOSE)
		P.unassemble(stdout, sym);
}

void suck(const char *device, const char *name, unsigned short opts,
	  const char *sym)
{
	emuPatch P;
	P.hwGet(device);
	if (opts & OPT_VERBOSE)
		P.unassemble(stdout, sym);
	P.Save(name);
}

int as(int nfiles, char *files[256], unsigned short opts, const char *sym)
{
	int i;
	char *last_dot,
	 *name,
	 *base_name,
	 *last_slash;
	int failed = 0;
	for (i = 0; i < nfiles; i++) {
		FileParserStream *F = new FileParserStream(files[i]);
		emuParser E;
		emuPatch P;
		E.SetOptions(opts);
		E.BeginParsing(F);
		E.ParsePatch();
		if (!E.ErrorState()) {
			if (E.compile(&P)) {
				if (opts & OPT_VERBOSE)
					P.unassemble(stdout, sym);

				base_name = __strdup(files[i]);
				last_slash = strrchr(base_name, '/');
				last_dot =
					strrchr(last_slash ? last_slash :
						base_name, '.');
				if (last_dot)
					*last_dot = '\0';

				name = (char *) malloc(strlen(base_name) +
						       strlen(DEFAULT_EXT) +
						       10);
				sprintf(name, "%s.%s", base_name, DEFAULT_EXT);
				P.Save(name);

				sprintf(name, "%s.%s", base_name, SYMBOL_EXT);
				E.SaveSymbols(name);
				free(name);
				free(base_name);
			}
		}
		else {
			fprintf(stderr,
				"Program has errors, compilation of '%s' failed\n",
				files[i]);
			failed++;
		}
	}
	return failed;
};

int main(int argc, char **argv)
{
	int c;
	const char **msg;
	int nfiles = 0;
	char *files[256];
	char *opt_device = 0;
	char *symbol_table = 0;
	struct stat statstruc;

	tOP action = OP_INFO;
	unsigned short as_opts = 0;

	memset(files, 0, sizeof(files));

	while ((c = getopt(argc, argv, "-atlvs:uo:d:hy:")) >= 0) {
		switch (c) {
		case 1:
			if (action) {
				if (stat(optarg, &statstruc)) {
					printf("not found: %s\n", optarg);
				}
				else {
					files[nfiles++] = __strdup(optarg);
				}
			}
			else {
				printf("you must specify -a or -l\n", optarg);
			}
			break;
		case 'y':
			if (symbol_table)
				printf("a single symbol table is allowed\n");
			else
				symbol_table = __strdup(optarg);
			break;
		case 'l':
			action = OP_LOAD;
			break;
		case 't':
			action = OP_TEST;
			break;
		case 'v':
			action = OP_VERSION;
			break;
		case 'h':
			action = OP_HELP;
			break;
		case 'd':
			if (!opt_device)
				opt_device = __strdup(optarg);
			break;
		case 'a':
			action = OP_ASM;
			break;
		case 's':
			files[nfiles++] = __strdup(optarg);
			action = OP_SUCK;
			break;
		case 'o':
			as_opts = parse_subopt(optarg);
			break;
		case 'u':
			action = OP_UNASSEMBLE;
			break;
		}
	}

	switch (action) {
	case OP_INFO:
		printf(INFOMSG, PROGRAM, argv[0]);
		break;
	case OP_VERSION:
		printf("%s %s %s %s\n", PROGRAM, AUTHOR, VERSION_MAJOR,
		       VERSION_MINOR);
		break;
	case OP_HELP:
		printf("%s\n\n", PROGRAM);
		for (msg = helpmsg; **msg; msg++)
			printf("%s\n", *msg);
		break;
	case OP_TEST:
	case OP_LOAD:
		if (nfiles)
			ld(opt_device, nfiles, files, as_opts, symbol_table,
			   (action == OP_TEST));
		else
			fprintf(stderr, "You must specify one file at least\n");
		break;
	case OP_ASM:
		if (nfiles) {
			as(nfiles, files, as_opts, symbol_table);
		}
		else
			fprintf(stderr, "You must supply one file at least\n");
		break;
	case OP_SUCK:
		if (nfiles != 1)
			fprintf(stderr, "You must supply a single file\n");
		else
			suck(opt_device, files[0], as_opts, symbol_table);
		break;
	case OP_UNASSEMBLE:
		unasm(opt_device, as_opts, symbol_table);
		break;
	}
	return 0;
}
