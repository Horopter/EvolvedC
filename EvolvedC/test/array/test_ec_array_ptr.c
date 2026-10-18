#include "../../src/ec_array.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* ec_array.h already instantiates void* as ptr */

static void
test_init_pos(void)
{
	ec_array_ptr_t a;
	assert(ec_array_ptr_init(&a, 2, NULL) == EC_OK);
	a.free(&a);
}

static void
test_push_capacity_neg(void)
{
	ec_array_ptr_t a;
	assert(ec_array_ptr_init(&a, 2, NULL) == EC_OK);
	int x = 1, y = 2, z = 3;
	void* p1 = &x;
	void* p2 = &y;
	void* p3 = &z;
	assert(a.push(&a, p1) == EC_OK);
	assert(a.push(&a, p2) == EC_OK);
	assert(a.push(&a, p3) == EC_ERR_MAX_CAPACITY);
	a.free(&a);
}

static void
test_get_pos(void)
{
	ec_array_ptr_t a;
	assert(ec_array_ptr_init(&a, 2, NULL) == EC_OK);
	int x = 1, y = 2;
	void* p1 = &x;
	void* p2 = &y;
	assert(a.push(&a, p1) == EC_OK);
	assert(a.push(&a, p2) == EC_OK);
	void* out;
	assert(a.get(&a, 0, &out) == EC_OK && out == p1);
	a.free(&a);
}

static void
test_get_neg_oob(void)
{
	ec_array_ptr_t a;
	assert(ec_array_ptr_init(&a, 2, NULL) == EC_OK);
	int x = 1;
	void* p1 = &x;
	assert(a.push(&a, p1) == EC_OK);
	void* out;
	assert(a.get(&a, 2, &out) == EC_ERR_OUT_OF_BOUNDS);
	a.free(&a);
}

static void
test_get_neg_null_out(void)
{
	ec_array_ptr_t a;
	assert(ec_array_ptr_init(&a, 2, NULL) == EC_OK);
	int x = 1;
	void* p1 = &x;
	assert(a.push(&a, p1) == EC_OK);
	assert(a.get(&a, 0, NULL) == EC_ERR_OUT_OF_BOUNDS);
	a.free(&a);
}

static void
test_set_pos(void)
{
	ec_array_ptr_t a;
	assert(ec_array_ptr_init(&a, 2, NULL) == EC_OK);
	int x = 1, y = 2, z = 3;
	void* p1 = &x;
	void* p2 = &y;
	void* p3 = &z;
	assert(a.push(&a, p1) == EC_OK);
	assert(a.push(&a, p2) == EC_OK);
	void* out;
	assert(a.set(&a, 1, p3) == EC_OK);
	assert(a.get(&a, 1, &out) == EC_OK && out == p3);
	a.free(&a);
}

static void
test_set_neg_oob(void)
{
	ec_array_ptr_t a;
	assert(ec_array_ptr_init(&a, 2, NULL) == EC_OK);
	int x = 1;
	void* p1 = &x;
	assert(a.push(&a, p1) == EC_OK);
	assert(a.set(&a, 2, p1) == EC_ERR_OUT_OF_BOUNDS);
	a.free(&a);
}

static void
test_init_neg_null(void)
{
	assert(ec_array_ptr_init(NULL, 2, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_init_neg_zero_capacity(void)
{
	ec_array_ptr_t a;
	assert(ec_array_ptr_init(&a, 0, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_push_pos(void)
{
	ec_array_ptr_t a;
	assert(ec_array_ptr_init(&a, 3, NULL) == EC_OK);
	int x = 1, y = 2, z = 3;
	void* p1 = &x;
	void* p2 = &y;
	void* p3 = &z;
	assert(a.push(&a, p1) == EC_OK);
	assert(a.push(&a, p2) == EC_OK);
	assert(a.push(&a, p3) == EC_OK);
	a.free(&a);
}

static void
test_push_neg_null(void)
{
	int x = 1;
	void* p1 = &x;
	assert(ec_array_ptr_push(NULL, p1) == EC_ERR_INVALID_ARG);
}

static void
test_get_neg_null_self(void)
{
	void* out;
	assert(ec_array_ptr_get(NULL, 0, &out) == EC_ERR_OUT_OF_BOUNDS);
}

static void
test_set_neg_null(void)
{
	int x = 1;
	void* p1 = &x;
	assert(ec_array_ptr_set(NULL, 0, p1) == EC_ERR_OUT_OF_BOUNDS);
}

static void
test_free_neg_null(void)
{
	ec_array_ptr_free(NULL); /* Should handle NULL gracefully */
}

int
main(void)
{
	printf("Starting ec_array_ptr tests...\n");
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
	printf("All ec_array_ptr tests passed.\n");
	return 0;
}
