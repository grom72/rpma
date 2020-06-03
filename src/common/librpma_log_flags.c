// SPDX-License-Identifier: BSD-3-Clause
/* Copyright 2019-2020, Intel Corporation */
/*
 *   BSD LICENSE
 *
 *   Copyright (c) Intel Corporation.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *	* Redistributions of source code must retain the above copyright
 *		notice, this list of conditions and the following disclaimer.
 *	* Redistributions in binary form must reproduce the above copyright
 *		notice, this list of conditions and the following disclaimer in
 *		the documentation and/or other materials provided with the
 *		distribution.
 *	* Neither the name of Intel Corporation nor the names of its
 *		contributors may be used to endorse or promote products derived
 *		from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "librpma_log_flags_internal.h"
#include <stddef.h>
#include <stdarg.h>
#include <syslog.h>
#include <time.h>
#include <string.h>
#ifndef assert
#include <assert.h>
#endif

static TAILQ_HEAD(, rpma_log_flag) g_log_flags = \
	TAILQ_HEAD_INITIALIZER(g_log_flags);


RPMA_LOG_REGISTER_COMPONENT("log", RPMA_LOG_LOG)


static struct rpma_log_flag *
get_log_flag(const char *name)
{
	struct rpma_log_flag *flag;

	TAILQ_FOREACH(flag, &g_log_flags, tailq) {
		if (strcasecmp(name, flag->name) == 0) {
			return flag;
		}
	}

	return NULL;
}

void
rpma_log_register_flag(const char *name, struct rpma_log_flag *flag)
{
	struct rpma_log_flag *iter;

	if (name == NULL || flag == NULL) {
		RPMA_ERRLOG("missing rpma_log_flag parameters\n");
		assert(false);
		return;
	}

	if (get_log_flag(name)) {
		RPMA_ERRLOG("duplicate rpma_log_flag '%s'\n", name);
		assert(false);
		return;
	}

	TAILQ_FOREACH(iter, &g_log_flags, tailq) {
		if (strcasecmp(iter->name, flag->name) > 0) {
			TAILQ_INSERT_BEFORE(iter, flag, tailq);
			return;
		}
	}

	TAILQ_INSERT_TAIL(&g_log_flags, flag, tailq);
}

int
rpma_log_get_flag(const char *name)
{
	struct rpma_log_flag *flag = get_log_flag(name);

	if (flag && flag->enabled) {
		return 1;
	}

	return 0;
}

static int
log_set_flag(const char *name, int value)
{
	struct rpma_log_flag *flag;

	if (strcasecmp(name, "all") == 0) {
		TAILQ_FOREACH(flag, &g_log_flags, tailq) {
			flag->enabled = value;
		}
		return 0;
	}

	flag = get_log_flag(name);
	if (flag == NULL) {
		return -1;
	}

	flag->enabled = value;

	return 0;
}

int
rpma_log_set_flag(const char *name)
{
	return log_set_flag(name, 1);
}

int
rpma_log_clear_flag(const char *name)
{
	return log_set_flag(name, false);
}

struct rpma_log_flag *
rpma_log_get_first_flag(void)
{
	return TAILQ_FIRST(&g_log_flags);
}

struct rpma_log_flag *
rpma_log_get_next_flag(struct rpma_log_flag *flag)
{
	return TAILQ_NEXT(flag, tailq);
}

void
rpma_log_usage(FILE *f, const char *log_arg)
{
#ifdef DEBUG
	struct rpma_log_flag *flag;
	fprintf(f, " %s, --logflag <flag>    enable debug log flag (all",\
			log_arg);

	TAILQ_FOREACH(flag, &g_log_flags, tailq) {
		fprintf(f, ", %s", flag->name);
	}

	fprintf(f, ")\n");
#else
	fprintf(f, " %s, --logflag <flag>    enable debug log flag " \
			"(not supported" \
			" - must reconfigure with --enable-debug)\n", log_arg);
#endif
}
