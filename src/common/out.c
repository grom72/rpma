// SPDX-License-Identifier: BSD-3-Clause
/* Copyright 2014-2020, Intel Corporation */

/*
 * out.c -- support for logging, tracing, and assertion output
 *
 * Macros like LOG(), OUT, ASSERT(), etc. end up here.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

#include "out.h"
#include "../valgrind/valgrind_internal.h"
#include "log_internal.h"

/*
 * out_fatal -- output a fatal error & die (i.e. assertion failure)
 */
void
out_fatal_and_abort(const char *file, int line, const char *func,
		const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	rpma_vlog(RPMA_LOG_ERROR, file, line, func, fmt, ap);
	va_end(ap);
	abort();
}
