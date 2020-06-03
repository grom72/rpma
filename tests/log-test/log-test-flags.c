/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright 2020, Intel Corporation
 */

/*
 * log-test.c -- unit tests of the log module
 */

#include <stdlib.h>
#include <syslog.h>
#include "cmocka_headers.h"
#include "librpma_log.h"
#include "librpma_log_internal.h"

/*
 * openlog -- openlog() mock
 */
void
openlog(const char *__ident, int __option, int __facility)
{
	check_expected(__ident);
	check_expected(__option);
	check_expected(__facility);
}

/*
 * closelog -- closelog() mock
 */
void
closelog(void)
{
	function_called();
}

static void user_logfunc(int level, const char *file, const int line,
		const char *func, const char *format, va_list args) {

}

int
log_setup_no_logfunction(void **p_logfunction)
{
	*p_logfunction = NULL;
	expect_string(openlog, __ident, "rpma");
	expect_value(openlog, __option, LOG_PID);
	expect_value(openlog, __facility, LOG_LOCAL7);
	rpma_log_open(NULL);
	return 0;
}

int
log_setup_logfunction(void **p_logfunction)
{
	*p_logfunction = user_logfunc;
	rpma_log_open(user_logfunc);
	return 0;
}

int
log_teardown(void **p_logfunction)
{
	logfunc *logf = *p_logfunction;
	if (NULL == logf) {
		expect_function_call(closelog);
	}
	rpma_log_close();
	return 0;
}

static void
test_open_close_no_logfunction(void **unused)
{

}

static void
test_open_close_logfunction(void **unused)
{

}

static void
test_set_level(void **unused)
{
	enum rpma_log_level level;
	for (level = RPMA_LOG_DISABLED; level <= RPMA_LOG_DEBUG; level ++) {
		rpma_log_set_level(level);
		assert_int_equal(level, rpma_log_get_level());
	}
}

static void
test_set_level_assert(void **unused)
{
	enum rpma_log_level level = RPMA_LOG_DEBUG;
	level ++;
	expect_assert_failure(rpma_log_set_level(level));
	level = RPMA_LOG_DISABLED;
	level --;
	expect_assert_failure(rpma_log_set_level(level));
}

static void
test_set_print_level(void **unused)
{
	enum rpma_log_level level;
	for (level = RPMA_LOG_DISABLED; level <= RPMA_LOG_DEBUG; level ++) {
		rpma_log_set_print_level(level);
		assert_int_equal(level, rpma_log_get_print_level());
	}
}

static void
test_set_print_level_assert(void **unused)
{
	enum rpma_log_level level = RPMA_LOG_DEBUG;
	level ++;
	expect_assert_failure(rpma_log_set_print_level(level));
	level = RPMA_LOG_DISABLED;
	level --;
	expect_assert_failure(rpma_log_set_print_level(level));
}


RPMA_LOG_REGISTER_COMPONENT("grom72", RPMA_LOG_GROM72);

static void
test_flag(void **unused)
{
	assert_int_equal(0, rpma_log_get_flag("grom72"));
	assert_int_equal(0, rpma_log_set_flag("grom72"));
	assert_int_equal(1, rpma_log_get_flag("grom72"));
	assert_int_equal(0, rpma_log_clear_flag("grom72"));
	assert_int_equal(0, rpma_log_get_flag("grom72"));
}

static void
test_flag_not_exist(void **unused)
{
	assert_int_equal(0, rpma_log_get_flag("grom"));
	assert_int_equal(-1, rpma_log_set_flag("grom"));
	assert_int_equal(0, rpma_log_get_flag("grom"));
	assert_int_equal(-1, rpma_log_clear_flag("grom"));
	assert_int_equal(0, rpma_log_get_flag("grom"));
}



int
main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup_teardown(
			test_open_close_no_logfunction,
			log_setup_no_logfunction, log_teardown),
		cmocka_unit_test_setup_teardown(
			test_open_close_logfunction,
			log_setup_logfunction, log_teardown),
		cmocka_unit_test_setup_teardown(
			test_set_level,
			log_setup_no_logfunction, log_teardown),
		cmocka_unit_test_setup_teardown(
			test_set_level,
			log_setup_logfunction, log_teardown),
		cmocka_unit_test_setup_teardown(
			test_set_level_assert,
			log_setup_no_logfunction, log_teardown),
		cmocka_unit_test_setup_teardown(
			test_set_level_assert,
			log_setup_logfunction, log_teardown),
		cmocka_unit_test_setup_teardown(
			test_set_print_level,
			log_setup_no_logfunction, log_teardown),
		cmocka_unit_test_setup_teardown(
			test_set_print_level,
			log_setup_logfunction, log_teardown),
		cmocka_unit_test_setup_teardown(
			test_set_print_level_assert,
			log_setup_no_logfunction, log_teardown),
		cmocka_unit_test_setup_teardown(
			test_set_print_level_assert,
			log_setup_logfunction, log_teardown),


/* flags tests */
		cmocka_unit_test(test_flag),
		cmocka_unit_test(test_flag_not_exist),

	};

	int retVal = cmocka_run_group_tests(tests, NULL, NULL);
	if (rpma_log_set_flag("grom72") != 0) {
		fprintf(stderr, "Problem\n");
	}
	rpma_log_usage(stderr, "test");
	return retVal;
}

#if 0
enum rpma_log_level {
	/* All messages will be suppressed. */
	RPMA_LOG_DISABLED = -1,
	RPMA_LOG_ERROR,
	RPMA_LOG_WARN,
	RPMA_LOG_NOTICE,
	RPMA_LOG_INFO,
	RPMA_LOG_DEBUG,
};
#endif
