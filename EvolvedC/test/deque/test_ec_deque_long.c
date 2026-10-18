#include "../../src/ec_deque.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void
test_push_front_back_pos(void)
{
	ec_deque_long_t d;
	assert(ec_deque_long_init(&d, NULL) == EC_OK);
	long x = -1, y = 0, z = 5;
	assert(d.push_front(&d, &x) == EC_OK);
	assert(d.push_back(&d, &y) == EC_OK);
	assert(d.push_back(&d, &z) == EC_OK);
	assert(d.size_fn(&d) == 3);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_peek_front_pos(void)
{
	ec_deque_long_t d;
	assert(ec_deque_long_init(&d, NULL) == EC_OK);
	long x = -1, z = 5;
	assert(d.push_front(&d, &x) == EC_OK);
	assert(d.push_back(&d, &z) == EC_OK);
	void* pv = NULL;
	long* lp;
	assert(d.peek_front(&d, &pv) == EC_OK);
	lp = (long*)pv;
	assert(*lp == -1);
	free(pv);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_peek_back_pos(void)
{
	ec_deque_long_t d;
	assert(ec_deque_long_init(&d, NULL) == EC_OK);
	long z = 5;
	assert(d.push_back(&d, &z) == EC_OK);
	void* pv = NULL;
	long* lp;
	assert(d.peek_back(&d, &pv) == EC_OK);
	lp = (long*)pv;
	assert(*lp == 5);
	free(pv);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_pop_order_pos(void)
{
	ec_deque_long_t d;
	assert(ec_deque_long_init(&d, NULL) == EC_OK);
	long x = -1, y = 0, z = 5;
	void* pv = NULL;
	long* lp;
	assert(d.push_front(&d, &x) == EC_OK);
	assert(d.push_back(&d, &y) == EC_OK);
	assert(d.push_back(&d, &z) == EC_OK);
	assert(d.pop_back(&d, &pv) == EC_OK);
	lp = (long*)pv;
	assert(*lp == 5);
	free(pv);
	assert(d.pop_front(&d, &pv) == EC_OK);
	lp = (long*)pv;
	assert(*lp == -1);
	free(pv);
	assert(d.pop_back(&d, &pv) == EC_OK);
	lp = (long*)pv;
	assert(*lp == 0);
	free(pv);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_empty_neg(void)
{
	ec_deque_long_t d;
	assert(ec_deque_long_init(&d, NULL) == EC_OK);
	void* pv = NULL;
	assert(d.pop_front(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.pop_back(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.peek_front(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.peek_back(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_invalid_args_neg(void)
{
	ec_deque_long_t d;
	assert(ec_deque_long_init(&d, NULL) == EC_OK);
	void* pv = NULL;
	assert(ec_deque_long_init(NULL, NULL) == EC_ERR_INVALID_ARG);
	assert(d.pop_front(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.pop_back(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.peek_front(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.peek_back(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.free_fn(&d) == EC_OK);
}

int
main(void)
{
	printf("Starting ec_deque_long tests...\n");
	test_push_front_back_pos();
	test_peek_front_pos();
	test_peek_back_pos();
	test_pop_order_pos();
	test_empty_neg();
	test_invalid_args_neg();
	printf("All ec_deque_long tests passed.\n");
	return 0;
}
