// SPDX-License-Identifier: BSD-3-Clause
/* Copyright 2014-2020, Intel Corporation */

/*
 * out.h -- definitions for "out" module
 */

#ifndef RPMA_OUT_H
#define RPMA_OUT_H 1

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

#include "log_internal.h"

/*
 * from "util.h"
 *  TG - do we ned this?
 */
#if defined(__CHECKER__)
#define COMPILE_ERROR_ON(cond)
#define ASSERT_COMPILE_ERROR_ON(cond)
#else
#define COMPILE_ERROR_ON(cond) ((void)sizeof(char[(cond) ? -1 : 1]))
#define ASSERT_COMPILE_ERROR_ON(cond) COMPILE_ERROR_ON(cond)
#endif
/*
 * Suppress errors which are after appropriate ASSERT* macro for nondebug
 * builds.
 */
#if !defined(DEBUG) && (defined(__clang_analyzer__) || defined(__COVERITY__) ||\
		defined(__KLOCWORK__))
#define OUT_FATAL_DISCARD_NORETURN __attribute__((noreturn))
#else
#define OUT_FATAL_DISCARD_NORETURN
#endif

#ifndef EVALUATE_DBG_EXPRESSIONS
#if defined(DEBUG) || defined(__clang_analyzer__) || defined(__COVERITY__) ||\
	defined(__KLOCWORK__)
#define EVALUATE_DBG_EXPRESSIONS 1
#else
#define EVALUATE_DBG_EXPRESSIONS 0
#endif
#endif

#ifdef DEBUG
#define OUT_FATAL_AND_ABORT out_fatal_and_abort
#else

static
__attribute__((always_inline))
__attribute__((noreturn))
inline void
out_fatal_and_abort_discard(const char *file, int line, const char *func,
		const char *fmt, ...)
{
	(void) file;
	(void) line;
	(void) func;
	(void) fmt;

	abort();
}

#define OUT_FATAL_AND_ABORT out_fatal_and_abort_discard

#endif

#if defined(__KLOCWORK__)
#define TEST_ALWAYS_TRUE_EXPR(cnd)
#define TEST_ALWAYS_EQ_EXPR(cnd)
#define TEST_ALWAYS_NE_EXPR(cnd)
#else
#define TEST_ALWAYS_TRUE_EXPR(cnd)\
	if (__builtin_constant_p(cnd))\
		ASSERT_COMPILE_ERROR_ON(cnd);
#define TEST_ALWAYS_EQ_EXPR(lhs, rhs)\
	if (__builtin_constant_p(lhs) && __builtin_constant_p(rhs))\
		ASSERT_COMPILE_ERROR_ON((lhs) == (rhs));
#define TEST_ALWAYS_NE_EXPR(lhs, rhs)\
	if (__builtin_constant_p(lhs) && __builtin_constant_p(rhs))\
		ASSERT_COMPILE_ERROR_ON((lhs) != (rhs));
#endif

/* assert a condition is true at runtime */
#define ASSERT_rt(cnd) do { \
	if (!EVALUATE_DBG_EXPRESSIONS || (cnd)) break; \
	OUT_FATAL_AND_ABORT(__FILE__, __LINE__, __func__, \
	"assertion failure: %s", #cnd);\
} while (0)

/* assertion with extra info printed if assertion fails at runtime */
#define ASSERTinfo_rt(cnd, info) do { \
	if (!EVALUATE_DBG_EXPRESSIONS || (cnd)) break; \
	OUT_FATAL_AND_ABORT(__FILE__, __LINE__, __func__, \
		"assertion failure: %s (%s = %s)", #cnd, #info, info);\
} while (0)

/* assert two integer values are equal at runtime */
#define ASSERTeq_rt(lhs, rhs) do { \
	if (!EVALUATE_DBG_EXPRESSIONS || ((lhs) == (rhs))) break; \
	OUT_FATAL_AND_ABORT(__FILE__, __LINE__, __func__, \
	"assertion failure: %s (0x%llx) == %s (0x%llx)", #lhs,\
	(unsigned long long)(lhs), #rhs, (unsigned long long)(rhs)); \
} while (0)

/* assert two integer values are not equal at runtime */
#define ASSERTne_rt(lhs, rhs) do { \
	if (!EVALUATE_DBG_EXPRESSIONS || ((lhs) != (rhs))) break; \
	OUT_FATAL_AND_ABORT(__FILE__, __LINE__, __func__, \
	"assertion failure: %s (0x%llx) != %s (0x%llx)", #lhs, \
	(unsigned long long)(lhs), #rhs, (unsigned long long)(rhs)); \
} while (0)

/* assert a condition is true */
#define ASSERT(cnd)\
	do {\
		/*\
		 * Detect useless asserts on always true expression. Please use\
		 * COMPILE_ERROR_ON(!cnd) or ASSERT_rt(cnd) in such cases.\
		 */\
		TEST_ALWAYS_TRUE_EXPR(cnd);\
		ASSERT_rt(cnd);\
	} while (0)

/* assertion with extra info printed if assertion fails */
#define ASSERTinfo(cnd, info)\
	do {\
		/* See comment in ASSERT. */\
		TEST_ALWAYS_TRUE_EXPR(cnd);\
		ASSERTinfo_rt(cnd, info);\
	} while (0)

/* assert two integer values are equal */
#define ASSERTeq(lhs, rhs)\
	do {\
		/* See comment in ASSERT. */\
		TEST_ALWAYS_EQ_EXPR(lhs, rhs);\
		ASSERTeq_rt(lhs, rhs);\
	} while (0)

/* assert two integer values are not equal */
#define ASSERTne(lhs, rhs)\
	do {\
		/* See comment in ASSERT. */\
		TEST_ALWAYS_NE_EXPR(lhs, rhs);\
		ASSERTne_rt(lhs, rhs);\
	} while (0)

void
__attribute__((noreturn))
__attribute__((__format__(__printf__, 4, 5)))
out_fatal_and_abort(const char *file, int line, const char *func,
		const char *fmt, ...);
#endif
