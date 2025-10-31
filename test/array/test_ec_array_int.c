#include "../../src/ec_array.h"
#include <assert.h>
#include <stdio.h>

static void
test_init_pos(void)
{
	ec_array_int_t a;
	assert(ec_array_int_init(&a, 3, NULL) == EC_OK);
	a.free(&a);
}

static void
test_push_pos(void)
{
	ec_array_int_t a;
	assert(ec_array_int_init(&a, 3, NULL) == EC_OK);
	assert(a.push(&a, 10) == EC_OK);
	assert(a.push(&a, 20) == EC_OK);
	assert(a.push(&a, 30) == EC_OK);
	a.free(&a);
}

static void
test_push_neg_over_capacity(void)
{
	ec_array_int_t a;
	assert(ec_array_int_init(&a, 3, NULL) == EC_OK);
	assert(a.push(&a, 10) == EC_OK);
	assert(a.push(&a, 20) == EC_OK);
	assert(a.push(&a, 30) == EC_OK);
	assert(a.push(&a, 40) == EC_ERR_MAX_CAPACITY);
	a.free(&a);
}

static void
test_get_pos(void)
{
	ec_array_int_t a;
	assert(ec_array_int_init(&a, 3, NULL) == EC_OK);
	assert(a.push(&a, 10) == EC_OK);
	assert(a.push(&a, 20) == EC_OK);
	int v;
	assert(a.get(&a, 0, &v) == EC_OK && v == 10);
	assert(a.get(&a, 1, &v) == EC_OK && v == 20);
	a.free(&a);
}

static void
test_get_neg_oob(void)
{
	ec_array_int_t a;
	assert(ec_array_int_init(&a, 2, NULL) == EC_OK);
	assert(a.push(&a, 10) == EC_OK);
	int v;
	assert(a.get(&a, 2, &v) == EC_ERR_OUT_OF_BOUNDS);
	a.free(&a);
}

static void
test_get_neg_null_out(void)
{
	ec_array_int_t a;
	assert(ec_array_int_init(&a, 2, NULL) == EC_OK);
	assert(a.push(&a, 10) == EC_OK);
	assert(a.get(&a, 0, NULL) == EC_ERR_OUT_OF_BOUNDS);
	a.free(&a);
}

static void
test_set_pos(void)
{
	ec_array_int_t a;
	assert(ec_array_int_init(&a, 3, NULL) == EC_OK);
	assert(a.push(&a, 10) == EC_OK);
	assert(a.push(&a, 20) == EC_OK);
	int v;
	assert(a.set(&a, 1, 99) == EC_OK);
	assert(a.get(&a, 1, &v) == EC_OK && v == 99);
	a.free(&a);
}

static void
test_set_neg_oob(void)
{
	ec_array_int_t a;
	assert(ec_array_int_init(&a, 2, NULL) == EC_OK);
	assert(a.push(&a, 10) == EC_OK);
	assert(a.set(&a, 2, 5) == EC_ERR_OUT_OF_BOUNDS);
	a.free(&a);
}

static void
test_init_neg_null(void)
{
	assert(ec_array_int_init(NULL, 3, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_init_neg_zero_capacity(void)
{
	ec_array_int_t a;
	assert(ec_array_int_init(&a, 0, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_push_neg_null(void)
{
	assert(ec_array_int_push(NULL, 10) == EC_ERR_INVALID_ARG);
}

static void
test_get_neg_null_self(void)
{
	int v;
	assert(ec_array_int_get(NULL, 0, &v) == EC_ERR_OUT_OF_BOUNDS);
}

static void
test_set_neg_null(void)
{
	assert(ec_array_int_set(NULL, 0, 10) == EC_ERR_OUT_OF_BOUNDS);
}

static void
test_free_neg_null(void)
{
	ec_array_int_free(NULL); /* Should handle NULL gracefully */
}

int
main(void)
{
	printf("Starting ec_array_int tests...\n");
	test_init_pos();
	test_init_neg_null();
	test_init_neg_zero_capacity();
	test_push_pos();
	test_push_neg_over_capacity();
	test_push_neg_null();
	test_get_pos();
	test_get_neg_oob();
	test_get_neg_null_out();
	test_get_neg_null_self();
	test_set_pos();
	test_set_neg_oob();
	test_set_neg_null();
	test_free_neg_null();
	printf("All ec_array_int tests passed.\n");
	return 0;
}
