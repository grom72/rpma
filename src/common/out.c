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
#include "util.h"
#include "log_internal.h"
// TG static const char *Log_prefix;
// TG static int Log_level;
// TG static FILE *Out_fp;
// TG static unsigned Log_alignment;

// TG #define MAXPRINT 256	/* maximum expected log line for libpmem */

#if 0
struct errormsg
{
	char msg[MAXPRINT];
};
#endif

#if 0
/*
 * out_init -- initialize the log
 *
 * This is called from the library initialization code.
 */
void
out_init(const char *log_prefix, const char *log_level_var,
		const char *log_file_var, int major_version,
		int minor_version)
{
// TG
	static int once;

	/* only need to initialize the out module once */
	if (once)
		return;
	once++;

	Log_prefix = log_prefix;

#ifdef DEBUG
	char *log_level;
	char *log_file;

	if ((log_level = getenv(log_level_var)) != NULL) {
		Log_level = atoi(log_level);
		if (Log_level < 0) {
			Log_level = 0;
		}
	}

	if ((log_file = getenv(log_file_var)) != NULL &&
				log_file[0] != '\0') {

		/* reserve more than enough space for a PID + '\0' */
		char log_file_pid[PATH_MAX];
		size_t len = strlen(log_file);
		if (len > 0 && log_file[len - 1] == '-') {
			if (util_snprintf(log_file_pid, PATH_MAX, "%s%d",
					log_file, getpid()) < 0) {
				ERR("snprintf: %d", errno);
				abort();
			}
			log_file = log_file_pid;
		}

		if ((Out_fp = fopen(log_file, "w")) == NULL) {
			char buff[UTIL_MAX_ERR_MSG];
			util_strerror(errno, buff, UTIL_MAX_ERR_MSG);
			fprintf(stderr, "Error (%s): %s=%s: %s\n",
				log_prefix, log_file_var,
				log_file, buff);
			abort();
		}
	}
#endif	/* DEBUG */

	char *log_alignment = getenv("RPMA_LOG_ALIGN");
	if (log_alignment) {
		int align = atoi(log_alignment);
		if (align > 0)
			Log_alignment = (unsigned)align;
	}

	if (Out_fp == NULL)
		Out_fp = stderr;
	else
		setlinebuf(Out_fp);

#ifdef DEBUG
// TG	static char namepath[PATH_MAX];
// TG	LOG(1, "pid %d: program: %s", getpid(),
// TG		util_getexecname(namepath, PATH_MAX));
#endif
// TG	LOG(1, "%s version %d.%d", log_prefix, major_version, minor_version);

// TG	static __attribute__((used)) const char *version_msg =
// TG			"src version: " SRCVERSION;
// TG	LOG(1, "%s", version_msg);
#if VG_PMEMCHECK_ENABLED
	/*
	 * Attribute "used" to prevent compiler from optimizing out the variable
	 * when LOG expands to no code (!DEBUG)
	 */
// TG	static __attribute__((used)) const char *pmemcheck_msg =
// TG			"compiled with support for Valgrind pmemcheck";
// TG	LOG(1, "%s", pmemcheck_msg);
#endif /* VG_PMEMCHECK_ENABLED */
#if VG_HELGRIND_ENABLED
	static __attribute__((used)) const char *helgrind_msg =
			"compiled with support for Valgrind helgrind";
// TG	LOG(1, "%s", helgrind_msg);
#endif /* VG_HELGRIND_ENABLED */
#if VG_MEMCHECK_ENABLED
	static __attribute__((used)) const char *memcheck_msg =
			"compiled with support for Valgrind memcheck";
// TG	LOG(1, "%s", memcheck_msg);
#endif /* VG_MEMCHECK_ENABLED */
#if VG_DRD_ENABLED
	static __attribute__((used)) const char *drd_msg =
			"compiled with support for Valgrind drd";
// TG	LOG(1, "%s", drd_msg);
#endif /* VG_DRD_ENABLED */
#if SDS_ENABLED
	static __attribute__((used)) const char *shutdown_state_msg =
			"compiled with support for shutdown state";
// TG	LOG(1, "%s", shutdown_state_msg);
#endif
#if NDCTL_ENABLED
	static __attribute__((used)) const char *ndctl_ge_63_msg =
		"compiled with libndctl 63+";
// TG	LOG(1, "%s", ndctl_ge_63_msg);
#endif
}
#endif
#if 0
/*
 * out_print_func -- default print_func, goes to stderr or Out_fp
 */
static void
out_print_func(const char *s)
{
	/* to suppress drd false-positive */
	/* XXX: confirm real nature of this issue: pmem/issues#863 */
#ifdef SUPPRESS_FPUTS_DRD_ERROR
	VALGRIND_ANNOTATE_IGNORE_READS_BEGIN();
	VALGRIND_ANNOTATE_IGNORE_WRITES_BEGIN();
#endif
	fputs(s, Out_fp);
#ifdef SUPPRESS_FPUTS_DRD_ERROR
	VALGRIND_ANNOTATE_IGNORE_READS_END();
	VALGRIND_ANNOTATE_IGNORE_WRITES_END();
#endif
}
#endif
/*
 * calling Print(s) calls the current print_func...
 */
// TG typedef void (*Print_func)(const char *s);
// TG typedef int (*Vsnprintf_func)(char *str, size_t size, const char *format,
// TG		va_list ap);
// TG static Print_func Print = out_print_func;
// TG static Vsnprintf_func Vsnprintf = vsnprintf;
#if 0
/*
 * out_snprintf -- (internal) custom snprintf implementation
 */
FORMAT_PRINTF(3, 4)
static int
out_snprintf(char *str, size_t size, const char *format, ...)
{
	int ret;
	va_list ap;

	va_start(ap, format);
	ret = Vsnprintf(str, size, format, ap);
	va_end(ap);

	return (ret);
}
#endif
#if 0
/*
 * out_common -- common output code, all output goes through here
 */
static void
out_common(const char *file, int line, const char *func, int level,
		const char *suffix, const char *fmt, va_list ap)
{
	int oerrno = errno;
	char buf[MAXPRINT];
	unsigned cc = 0;
	int ret;
	const char *sep = "";
	char errstr[UTIL_MAX_ERR_MSG] = "";

	if (file) {
		char *f = strrchr(file, '/');
		if (f)
			file = f + 1;
		ret = out_snprintf(&buf[cc], MAXPRINT - cc,
				"<%s>: <%d> [%s:%d %s] ",
				Log_prefix, level, file, line, func);
		if (ret < 0) {
			Print("out_snprintf failed");
			goto end;
		}
		cc += (unsigned)ret;
		if (cc < Log_alignment) {
			memset(buf + cc, ' ', Log_alignment - cc);
			cc = Log_alignment;
		}
	}

	if (fmt) {
		if (*fmt == '!') {
			sep = ": ";
			fmt++;
			if (*fmt != '!') {
				util_strerror(oerrno, errstr, UTIL_MAX_ERR_MSG);
			}

		}
		ret = Vsnprintf(&buf[cc], MAXPRINT - cc, fmt, ap);
		if (ret < 0) {
			Print("Vsnprintf failed");
			goto end;
		}
		cc += (unsigned)ret;
	}

	out_snprintf(&buf[cc], MAXPRINT - cc, "%s%s%s", sep, errstr, suffix);

	Print(buf);

end:

	errno = oerrno;
}
#endif

#if 0
/*
 * out_error -- common error output code, all error messages go through here
 */
static void
out_error(const char *file, int line, const char *func,
		const char *suffix, const char *fmt, va_list ap)
{
	int oerrno = errno;
	unsigned cc = 0;
	int ret;
	const char *sep = "";
	char errstr[UTIL_MAX_ERR_MSG] = "";

	char *errormsg = (char *)out_get_errormsg();
	if (fmt) {
		if (*fmt == '!') {
			sep = ": ";
			fmt++;
			if (*fmt != '!') {
				util_strerror(oerrno, errstr, UTIL_MAX_ERR_MSG);
			}
		}

		ret = Vsnprintf(&errormsg[cc], MAXPRINT, fmt, ap);
		if (ret < 0) {
			strcpy(errormsg, "Vsnprintf failed");
			goto end;
		}
		cc += (unsigned)ret;
		out_snprintf(&errormsg[cc], MAXPRINT - cc, "%s%s",
				sep, errstr);
	}
#ifdef DEBUG
	if (Log_level >= 1) {
		char buf[MAXPRINT];
		cc = 0;

		if (file) {
			char *f = strrchr(file, '/');
			if (f)
				file = f + 1;
			ret = out_snprintf(&buf[cc], MAXPRINT,
					"<%s>: <1> [%s:%d %s] ",
					Log_prefix, file, line, func);
			if (ret < 0) {
				Print("out_snprintf failed");
				goto end;
			}
			cc += (unsigned)ret;
		}

		out_snprintf(&buf[cc], MAXPRINT - cc, "%s%s", errormsg,
				suffix);

		Print(buf);
	}
#endif

end:
	errno = oerrno;
}
#endif

#if 0
/*
 * out -- output a line, newline added automatically
 */
void
out(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	out_common(NULL, 0, NULL, 0, "\n", fmt, ap);

	va_end(ap);
}
#endif
#if 0
/*
 * out_nonl -- output a line, no newline added automatically
 */
void
out_nonl(int level, const char *fmt, ...)
{
	va_list ap;

	if (Log_level < level)
			return;

	va_start(ap, fmt);
	out_common(NULL, 0, NULL, level, "", fmt, ap);

	va_end(ap);
}
#endif
#if 0
/*
 * out_log -- output a log line if Log_level >= level
 */
void
out_log(const char *file, int line, const char *func, int level,
		const char *fmt, ...)
{
	va_list ap;

	if (Log_level < level)
		return;

	va_start(ap, fmt);
	out_common(file, line, func, level, "\n", fmt, ap);

	va_end(ap);
}
#endif
/*
 * out_fatal -- output a fatal error & die (i.e. assertion failure)
 */
void
out_fatal(const char *file, int line, const char *func,
		const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	rpma_vlog(RPMA_LOG_ERROR, file, line, func, fmt, ap);
	// TG out_common(file, line, func, 1, "\n", fmt, ap);

	va_end(ap);

	abort();
}

/*
 * out_err -- output an error message
 */
void
out_err(const char *file, int line, const char *func,
		const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	rpma_vlog(RPMA_LOG_ERROR, file, line, func, fmt, ap);
	//TG out_error(file, line, func, "\n", fmt, ap);

	va_end(ap);
}

