#ifndef DIRECTIVE_H
#define DIRECTIVE_H

#include "macro.h"
#include <lacc/token.h>

/*
 * Preprocess a line starting with a '#' directive. Borrows ownership of
 * input. Assume input is END terminated.
 */
void preprocess_directive(TokenArray *line);

/* Non-zero iff currently not inside a false #if directive. */
int in_active_block(void);

#endif
