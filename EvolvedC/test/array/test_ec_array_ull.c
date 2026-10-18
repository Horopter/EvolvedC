#include "../../src/ec_array.h"
#include <assert.h>
#include <stdio.h>

static void
test_init_pos(void)
{
	ec_array_ull_t a;
	assert(ec_array_ull_init(&a, 1, NULL) == EC_OK);
	a.free(&a);
}

static void
test_push_capacity_neg(void)
{
	ec_array_ull_t a;
	assert(ec_array_ull_init(&a, 1, NULL) == EC_OK);
	assert(a.push(&a, 7ULL) == EC_OK);
	assert(a.push(&a, 9ULL) == EC_ERR_MAX_CAPACITY);
	a.free(&a);
}

static void
test_get_pos(void)
{
	ec_array_ull_t a;
	assert(ec_array_ull_init(&a, 1, NULL) == EC_OK);
	assert(a.push(&a, 7ULL) == EC_OK);
	unsigned long long x;
	assert(a.get(&a, 0, &x) == EC_OK && x == 7ULL);
	a.free(&a);
}

static void
test_get_neg_oob(void)
{
	ec_array_ull_t a;
	assert(ec_array_ull_init(&a, 1, NULL) == EC_OK);
	unsigned long long x;
	assert(a.get(&a, 1, &x) == EC_ERR_OUT_OF_BOUNDS);
	a.free(&a);
}

static void
test_init_neg_null(void)
{
	assert(ec_array_ull_init(NULL, 1, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_init_neg_zero_capacity(void)
{
	ec_array_ull_t a;
	assert(ec_array_ull_init(&a, 0, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_push_pos(void)
{
	ec_array_ull_t a;
	assert(ec_array_ull_init(&a, 2, NULL) == EC_OK);
	assert(a.push(&a, 7ULL) == EC_OK);
	assert(a.push(&a, 9ULL) == EC_OK);
	a.free(&a);
}

static void
test_push_neg_null(void)
{
	assert(ec_array_ull_push(NULL, 7ULL) == EC_ERR_INVALID_ARG);
}

static void
test_get_neg_null_self(void)
{
	unsigned long long x;
	assert(ec_array_ull_get(NULL, 0, &x) == EC_ERR_OUT_OF_BOUNDS);
}

static void
test_get_neg_null_out(void)
{
	ec_array_ull_t a;
	assert(ec_array_ull_init(&a, 1, NULL) == EC_OK);
	assert(a.push(&a, 7ULL) == EC_OK);
	assert(a.get(&a, 0, NULL) == EC_ERR_OUT_OF_BOUNDS);
	a.free(&a);
}

static void
test_set_pos(void)
{
	ec_array_ull_t a;
	assert(ec_array_ull_init(&a, 2, NULL) == EC_OK);
	assert(a.push(&a, 7ULL) == EC_OK);
	assert(a.push(&a, 9ULL) == EC_OK);
	unsigned long long x;
	assert(a.set(&a, 1, 11ULL) == EC_OK);
	assert(a.get(&a, 1, &x) == EC_OK && x == 11ULL);
	a.free(&a);
}

static void
test_set_neg_oob(void)
{
	ec_array_ull_t a;
	assert(ec_array_ull_init(&a, 2, NULL) == EC_OK);
	assert(a.push(&a, 7ULL) == EC_OK);
	assert(a.set(&a, 2, 13ULL) == EC_ERR_OUT_OF_BOUNDS);
	a.free(&a);
}

static void
test_set_neg_null(void)
{
	assert(ec_array_ull_set(NULL, 0, 7ULL) == EC_ERR_OUT_OF_BOUNDS);
}

static void
test_free_neg_null(void)
{
	ec_array_ull_free(NULL); /* Should handle NULL gracefully */
}

int
main(void)
{
	printf("Starting ec_array_ull tests...\n");
	test_init_pos();
	test_init_neg_null();
	test_init_neg_zero_capacity();
	test_push_pos();
	test_push_capacity_neg();
	test_push_neg_null();
	test_get_pos();
	test_get_neg_oob();
	test_get_neg_null_self();
	test_get_neg_null_out();
	test_set_pos();
	test_set_neg_oob();
	test_set_neg_null();
	test_free_neg_null();
	printf("All ec_array_ull tests passed.\n");
	return 0;
}
