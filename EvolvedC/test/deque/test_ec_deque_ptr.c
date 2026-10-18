#include "../../src/ec_deque.h"
#include <assert.h>
#include <stdio.h>

static void
test_push_peek_pos(void)
{
	ec_deque_ptr_t d;
	assert(ec_deque_ptr_init(&d, NULL) == EC_OK);
	int x = 1, y = 2, z = 3;
	void *p1 = &x, *p2 = &y, *p3 = &z;
	void** out;
	assert(d.push_front(&d, &p1) == EC_OK);
	assert(d.push_back(&d, &p2) == EC_OK);
	assert(d.push_back(&d, &p3) == EC_OK);
	assert(d.peek_front(&d, (void**)&out) == EC_OK && *out == p1);
	assert(d.peek_back(&d, (void**)&out) == EC_OK && *out == p3);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_pop_order_pos(void)
{
	ec_deque_ptr_t d;
	assert(ec_deque_ptr_init(&d, NULL) == EC_OK);
	int x = 1, y = 2, z = 3;
	void *p1 = &x, *p2 = &y, *p3 = &z;
	void** out;
	assert(d.push_front(&d, &p1) == EC_OK);
	assert(d.push_back(&d, &p2) == EC_OK);
	assert(d.push_back(&d, &p3) == EC_OK);
	assert(d.pop_front(&d, (void**)&out) == EC_OK && *out == p1);
	assert(d.pop_back(&d, (void**)&out) == EC_OK && *out == p3);
	assert(d.pop_back(&d, (void**)&out) == EC_OK && *out == p2);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_size_pos(void)
{
	ec_deque_ptr_t d;
	assert(ec_deque_ptr_init(&d, NULL) == EC_OK);
	int x = 1, y = 2, z = 3;
	void *p1 = &x, *p2 = &y, *p3 = &z;
	assert(d.size_fn(&d) == 0);
	assert(d.push_front(&d, &p1) == EC_OK);
	assert(d.size_fn(&d) == 1);
	assert(d.push_back(&d, &p2) == EC_OK);
	assert(d.size_fn(&d) == 2);
	assert(d.push_back(&d, &p3) == EC_OK);
	assert(d.size_fn(&d) == 3);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_empty_neg(void)
{
	ec_deque_ptr_t d;
	assert(ec_deque_ptr_init(&d, NULL) == EC_OK);
	void** out;
	assert(d.pop_front(&d, (void**)&out) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.pop_back(&d, (void**)&out) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.peek_front(&d, (void**)&out) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.peek_back(&d, (void**)&out) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_invalid_args_neg(void)
{
	void** out;
	assert(ec_deque_ptr_init(NULL, NULL) == EC_ERR_INVALID_ARG);
	assert(ec_deque_ptr_pop_front(NULL, (void**)&out) == EC_ERR_INVALID_ARG);
	assert(ec_deque_ptr_pop_back(NULL, (void**)&out) == EC_ERR_INVALID_ARG);
	assert(ec_deque_ptr_peek_front(NULL, (void**)&out) == EC_ERR_INVALID_ARG);
	assert(ec_deque_ptr_peek_back(NULL, (void**)&out) == EC_ERR_INVALID_ARG);
}

int
main(void)
{
	printf("Starting ec_deque_ptr tests...\n");
	test_push_peek_pos();
	test_pop_order_pos();
	test_size_pos();
	test_empty_neg();
	test_invalid_args_neg();
	printf("All ec_deque_ptr tests passed.\n");
	return 0;
}
