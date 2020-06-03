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

/*
 * \file
 * Logging interfaces
 */

#ifndef LIBRPMA_LOG_FLAGS_INTERNAL_H
#define LIBRPMA_LOG_FLAGS_INTERNAL_H

#include <sys/queue.h>
#include "librpma_log_internal.h"
#include "librpma_log_flags.h"

#define false 0
#define true 1

struct rpma_log_flag {
	TAILQ_ENTRY(rpma_log_flag) tailq;
	const char *name;
	int enabled;
};

void rpma_log_register_flag(const char *name, struct rpma_log_flag *flag);

struct rpma_log_flag *rpma_log_get_first_flag(void);
struct rpma_log_flag *rpma_log_get_next_flag(struct rpma_log_flag *flag);

#define RPMA_LOG_REGISTER_COMPONENT(str, flag) \
static struct rpma_log_flag flag = { \
	.enabled = false, \
	.name = str, \
}; \
__attribute__((constructor)) static void register_flag_##flag(void) \
{ \
	rpma_log_register_flag(str, &flag); \
}

#define RPMA_INFOLOG(FLAG, ...)	\
do { \
	extern struct rpma_log_flag FLAG; \
	if (FLAG.enabled) {	\
		rpma_log(RPMA_LOG_INFO, __FILE__, \
				__LINE__, __func__, __VA_ARGS__); \
	} \
} while (0)

#ifdef DEBUG

#define RPMA_DEBUGLOG(FLAG, ...) \
do { \
	extern struct rpma_log_flag FLAG; \
	if (FLAG.enabled) {	\
	rpma_log(RPMA_LOG_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__); \
	} \
} while (0)

#define RPMA_LOGDUMP(FLAG, LABEL, BUF, LEN)	\
do {\
	extern struct rpma_log_flag FLAG; \
	if ((FLAG.enabled) && (LEN)) { \
		rpma_log_dump(stderr, (LABEL), (BUF), (LEN)); \
	} \
} while (0)

#else
#define RPMA_DEBUGLOG(...) do { } while (0)
#define RPMA_LOGDUMP(...) do { } while (0)
#endif

#endif /* LIBRPMA_LOG_FLAGS_INTERNAL_H */
