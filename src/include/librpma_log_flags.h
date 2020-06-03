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
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
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

#ifndef LIBRPMA_LOG_FLAGS_H
#define LIBRPMA_LOG_FLAGS_H
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Check whether the log flag exists and is enabled.
 *
 * \return 1 if enabled, or 0 otherwise.
 */
int rpma_log_get_flag(const char *flag);

/*
 * Enable the log flag.
 *
 * \param flag Log flag to be enabled.
 *
 * \return 0 on success, -1 on failure.
 */
int rpma_log_set_flag(const char *flag);

/*
 * Clear a log flag.
 *
 * \param flag Log flag to clear.
 *
 * \return 0 on success, -1 on failure.
 */
int rpma_log_clear_flag(const char *flag);

/*
 * Show all the log flags and their usage.
 *
 * \param f File to hold all the flags' information.
 * \param log_arg Command line option to set/enable the log flag.
 */
void rpma_log_usage(FILE *f, const char *log_arg);

#ifdef __cplusplus
}
#endif

#endif /* LIBRPMA_LOG_FLAGS_H */
