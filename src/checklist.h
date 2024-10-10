#pragma once

#include <stdio.h>

/*
 * Use the template for asking the user the check status and the note.
 * All the argument must be sanitized before calling this function.
 * return -1 in case of error, 0 otherwise
 */
int checklist(FILE *fout, char *templ, const char *user);
