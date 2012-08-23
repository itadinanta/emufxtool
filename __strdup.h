#ifndef ___STRDUP_H
#define ___STRDUP_H
#include "config.h"
static char *__strdup(const char *s)
{
	char *dest = 0;
	return s ? strcpy((char *) malloc(strlen(s) + 1), s) : 0;
}
#endif
