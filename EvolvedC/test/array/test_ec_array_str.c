#include "../../src/ec_array.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* ec_array.h already instantiates const char* as str */

static void
test_init_pos(void)
{
	ec_array_str_t a;
	assert(ec_array_str_init(&a, 2, NULL) == EC_OK);
	a.free(&a);
}

static void
test_push_capacity_neg(void)
{
	ec_array_str_t a;
	assert(ec_array_str_init(&a, 2, NULL) == EC_OK);
	const char* s1 = "aa";
	const char* s2 = "bb";
	const char* s3 = "cc";
	assert(a.push(&a, s1) == EC_OK);
	assert(a.push(&a, s2) == EC_OK);
	assert(a.push(&a, s3) == EC_ERR_MAX_CAPACITY);
	a.free(&a);
}

static void
test_get_pos(void)
{
	ec_array_str_t a;
	assert(ec_array_str_init(&a, 2, NULL) == EC_OK);
	const char* s1 = "aa";
	const char* s2 = "bb";
	assert(a.push(&a, s1) == EC_OK);
	assert(a.push(&a, s2) == EC_OK);
	const char* out;
	assert(a.get(&a, 1, &out) == EC_OK && strcmp(out, "bb") == 0);
	a.free(&a);
}

static void
test_get_neg_oob(void)
{
	ec_array_str_t a;
	assert(ec_array_str_init(&a, 2, NULL) == EC_OK);
	const char* s1 = "aa";
	assert(a.push(&a, s1) == EC_OK);
	const char* out;
	assert(a.get(&a, 2, &out) == EC_ERR_OUT_OF_BOUNDS);
	a.free(&a);
}

static void
test_get_neg_null_out(void)
{
	ec_array_str_t a;
	assert(ec_array_str_init(&a, 2, NULL) == EC_OK);
	const char* s1 = "aa";
	assert(a.push(&a, s1) == EC_OK);
	assert(a.get(&a, 0, NULL) == EC_ERR_OUT_OF_BOUNDS);
	a.free(&a);
}

static void
test_set_pos(void)
{
	ec_array_str_t a;
	assert(ec_array_str_init(&a, 2, NULL) == EC_OK);
	const char* s1 = "aa";
	const char* s2 = "bb";
	const char* s3 = "cc";
	assert(a.push(&a, s1) == EC_OK);
	assert(a.push(&a, s2) == EC_OK);
	const char* out;
	assert(a.set(&a, 1, s3) == EC_OK);
	assert(a.get(&a, 1, &out) == EC_OK && strcmp(out, "cc") == 0);
	a.free(&a);
}

static void
test_set_neg_oob(void)
{
	ec_array_str_t a;
	assert(ec_array_str_init(&a, 2, NULL) == EC_OK);
	const char* s1 = "aa";
	assert(a.push(&a, s1) == EC_OK);
	assert(a.set(&a, 2, s1) == EC_ERR_OUT_OF_BOUNDS);
	a.free(&a);
}

static void
test_init_neg_null(void)
{
	assert(ec_array_str_init(NULL, 2, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_init_neg_zero_capacity(void)
{
	ec_array_str_t a;
	assert(ec_array_str_init(&a, 0, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_push_pos(void)
{
	ec_array_str_t a;
	assert(ec_array_str_init(&a, 3, NULL) == EC_OK);
	const char* s1 = "aa";
	const char* s2 = "bb";
	const char* s3 = "cc";
	assert(a.push(&a, s1) == EC_OK);
	assert(a.push(&a, s2) == EC_OK);
	assert(a.push(&a, s3) == EC_OK);
	a.free(&a);
}

static void
test_push_neg_null(void)
{
	const char* s1 = "aa";
	assert(ec_array_str_push(NULL, s1) == EC_ERR_INVALID_ARG);
}

static void
test_get_neg_null_self(void)
{
	const char* out;
	assert(ec_array_str_get(NULL, 0, &out) == EC_ERR_OUT_OF_BOUNDS);
}

static void
test_set_neg_null(void)
{
	const char* s1 = "aa";
	assert(ec_array_str_set(NULL, 0, s1) == EC_ERR_OUT_OF_BOUNDS);
}

static void
test_free_neg_null(void)
{
	ec_array_str_free(NULL); /* Should handle NULL gracefully */
}

int
main(void)
{
	printf("Starting ec_array_str tests...\n");
	test_init_pos();
	test_init_neg_null();
	test_init_neg_zero_capacity();
	test_push_pos();
	test_push_capacity_neg();
	test_push_neg_null();
	test_get_pos();
	test_get_neg_oob();
	test_get_neg_null_out();
	test_get_neg_null_self();
	test_set_pos();
	test_set_neg_oob();
	test_set_neg_null();
	test_free_neg_null();
	printf("All ec_array_str tests passed.\n");
	return 0;
}
