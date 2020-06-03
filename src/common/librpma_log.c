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

#include <stddef.h>
#include <stdarg.h>
#include <syslog.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#ifndef assert
#include <assert.h>
#endif

#include "librpma_log_internal.h"

#ifdef RPMA_LOG_BACKTRACE_LVL
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#endif

static const char *const rpma_level_names[] = {
	[RPMA_LOG_ERROR]	= "ERROR",
	[RPMA_LOG_WARN]		= "WARNING",
	[RPMA_LOG_NOTICE]	= "NOTICE",
	[RPMA_LOG_INFO]		= "INFO",
	[RPMA_LOG_DEBUG]	= "DEBUG",
};

#define MAX_TMPBUF 1024

static logfunc *g_log = NULL;

enum rpma_log_level g_rpma_log_level = RPMA_LOG_NOTICE;
enum rpma_log_level g_rpma_log_print_level = RPMA_LOG_NOTICE;
enum rpma_log_level g_rpma_log_backtrace_level = RPMA_LOG_DISABLED;

void
rpma_log_open(logfunc *logf)
{
	if (logf) {
		g_log = logf;
	} else {
		openlog("rpma", LOG_PID, LOG_LOCAL7);
	}
}

void
rpma_log_close(void)
{
	if (!g_log) {
		closelog();
	} else {
		g_log = NULL;
	}
}

#ifdef RPMA_LOG_BACKTRACE_LVL
static void
log_unwind_stack(FILE *fp, enum rpma_log_level level)
{
	unw_error_t err;
	unw_cursor_t cursor;
	unw_context_t uc;
	unw_word_t ip;
	unw_word_t offp;
	char f_name[64];
	int frame;

	if (level > g_rpma_log_backtrace_level) {
		return;
	}

	unw_getcontext(&uc);
	unw_init_local(&cursor, &uc);
	fprintf(fp, "*%s*: === BACKTRACE START ===\n", rpma_level_names[level]);

	unw_step(&cursor);
	for (frame = 1; unw_step(&cursor) > 0; frame++) {
		unw_get_reg(&cursor, UNW_REG_IP, &ip);
		err = unw_get_proc_name(&cursor, f_name, sizeof(f_name), &offp);
		if (err || strcmp(f_name, "main") == 0) {
			break;
		}

		fprintf(fp, "*%s*: %3d: %*s%s() at %#lx\n", \
				rpma_level_names[level], frame, frame - 1, "", \
				f_name, (unsigned long)ip);
	}
	fprintf(fp, "*%s*: === BACKTRACE END ===\n", rpma_level_names[level]);
}

#else
#define log_unwind_stack(fp, lvl)
#endif

static void
get_timestamp_prefix(char *buf, size_t buf_size)
{
	struct tm *info;
	char date[24];
	struct timespec ts;
	long usec;

	clock_gettime(CLOCK_REALTIME, &ts);
	info = localtime(&ts.tv_sec);
	usec = ts.tv_nsec / 1000;

	strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", info);
	snprintf(buf, buf_size, "[%s.%06ld] ", date, usec);
}

void
rpma_log(enum rpma_log_level level, const char *file, const int line, \
		const char *func, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	rpma_vlog(level, file, line, func, format, ap);
	va_end(ap);
}

void
rpma_vlog(enum rpma_log_level level, const char *file, const int line, \
		const char *func, const char *format, va_list ap)
{
	int severity = LOG_INFO;
	char buf[MAX_TMPBUF];
	char timestamp[45];

	if (g_log) {
		g_log(level, file, line, func, format, ap);
		return;
	}

	if (level > g_rpma_log_print_level && level > g_rpma_log_level) {
		return;
	}

	switch (level) {
	case RPMA_LOG_ERROR:
		severity = LOG_ERR;
		break;
	case RPMA_LOG_WARN:
		severity = LOG_WARNING;
		break;
	case RPMA_LOG_NOTICE:
		severity = LOG_NOTICE;
		break;
	case RPMA_LOG_INFO:
	case RPMA_LOG_DEBUG:
		severity = LOG_INFO;
		break;
	case RPMA_LOG_DISABLED:
		return;
	}

	vsnprintf(buf, sizeof(buf), format, ap);

	if (level <= g_rpma_log_print_level) {
		get_timestamp_prefix(timestamp, sizeof(timestamp));
		if (file) {
			fprintf(stderr, "%s%s:%4d:%s: *%s*: %s", timestamp,\
					file, line, func,\
					rpma_level_names[level], buf);
			log_unwind_stack(stderr, level);
		} else {
			fprintf(stderr, "%s%s", timestamp, buf);
		}
	}

	if (level <= g_rpma_log_level) {
		if (file) {
			syslog(severity, "%s:%4d:%s: *%s*: %s", file, line, \
					func, rpma_level_names[level], buf);
		} else {
			syslog(severity, "%s", buf);
		}
	}
}

static void
fdump(FILE *fp, const char *label, const unsigned char *buf, size_t len)
{
	char tmpbuf[MAX_TMPBUF];
	unsigned char buf16[16 + 1];
	size_t total;
	unsigned idx;

	fprintf(fp, "%s\n", label);

	memset(buf16, 0, sizeof(buf16));
	total = 0;
	for (idx = 0; idx < len; idx++) {
		if (idx != 0 && idx % 16 == 0) {
			snprintf(tmpbuf + total, sizeof(tmpbuf) - total,\
					" %s", buf16);
			memset(buf16, 0, sizeof(buf16));
			fprintf(fp, "%s\n", tmpbuf);
			total = 0;
		}
		if (idx % 16 == 0) {
			total += (size_t)snprintf(tmpbuf + total,\
					sizeof(tmpbuf) - total,\
					"%08x ", idx);
		}
		if (idx % 8 == 0) {
			total += (size_t)snprintf(tmpbuf + total,\
					sizeof(tmpbuf) - total,\
					"%s", " ");
		}
		total += (size_t)snprintf(tmpbuf + total, \
				sizeof(tmpbuf) - total, \
				"%2.2x ", buf[idx] & 0xff);
		buf16[idx % 16] = isprint(buf[idx]) ? buf[idx] : '.';
	}
	for (; idx % 16 != 0; idx++) {
		if (idx == 8) {
			total += (size_t)snprintf(tmpbuf + total, \
					sizeof(tmpbuf) - total,\
					" ");
		}

		total += (size_t)snprintf(tmpbuf + total, \
				sizeof(tmpbuf) - total, "   ");
	}
	snprintf(tmpbuf + total, sizeof(tmpbuf) - total, "  %s", buf16);
	fprintf(fp, "%s\n", tmpbuf);
	fflush(fp);
}

void
rpma_log_dump(FILE *fp, const char *label, const void *buf, size_t len)
{
	fdump(fp, label, buf, len);
}

void
rpma_log_set_level(enum rpma_log_level level)
{
	assert(level >= RPMA_LOG_DISABLED);
	assert(level <= RPMA_LOG_DEBUG);
	g_rpma_log_level = level;
}

enum rpma_log_level
rpma_log_get_level(void)
{
	return g_rpma_log_level;
}

void
rpma_log_set_print_level(enum rpma_log_level level)
{
	assert(level >= RPMA_LOG_DISABLED);
	assert(level <= RPMA_LOG_DEBUG);
	g_rpma_log_print_level = level;
}

enum rpma_log_level
rpma_log_get_print_level(void)
{
	return g_rpma_log_print_level;
}

void
rpma_log_set_backtrace_level(enum rpma_log_level level)
{
	assert(level >= RPMA_LOG_DISABLED);
	assert(level <= RPMA_LOG_DEBUG);
	g_rpma_log_backtrace_level = level;
}

enum rpma_log_level
rpma_log_get_backtrace_level(void)
{
	return g_rpma_log_backtrace_level;
}
