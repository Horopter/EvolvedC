#include "../../src/ec_deque.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

EC_DEQUE_DEF(unsigned long, ulong)

static void
test_init_free_pos(void)
{
	ec_deque_ulong_t d;
	assert(ec_deque_ulong_init(&d, NULL) == EC_OK);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_push_front_back_pos(void)
{
	ec_deque_ulong_t d;
	assert(ec_deque_ulong_init(&d, NULL) == EC_OK);
	unsigned long x = 100UL, y = 200UL, z = 300UL;
	assert(d.push_back(&d, &x) == EC_OK);
	assert(d.push_front(&d, &y) == EC_OK);
	assert(d.push_back(&d, &z) == EC_OK);
	assert(d.size_fn(&d) == 3);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_peek_front_back_pos(void)
{
	ec_deque_ulong_t d;
	assert(ec_deque_ulong_init(&d, NULL) == EC_OK);
	unsigned long x = 100UL, y = 200UL;
	assert(d.push_front(&d, &x) == EC_OK);
	assert(d.push_back(&d, &y) == EC_OK);
	void* pv = NULL;
	unsigned long* up;
	assert(d.peek_front(&d, &pv) == EC_OK);
	up = (unsigned long*)pv;
	assert(*up == 100UL);
	free(pv);
	assert(d.peek_back(&d, &pv) == EC_OK);
	up = (unsigned long*)pv;
	assert(*up == 200UL);
	free(pv);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_pop_front_back_pos(void)
{
	ec_deque_ulong_t d;
	assert(ec_deque_ulong_init(&d, NULL) == EC_OK);
	unsigned long x = 100UL, y = 200UL, z = 300UL;
	void* pv = NULL;
	unsigned long* up;
	assert(d.push_back(&d, &x) == EC_OK);
	assert(d.push_back(&d, &y) == EC_OK);
	assert(d.push_back(&d, &z) == EC_OK);
	assert(d.pop_front(&d, &pv) == EC_OK);
	up = (unsigned long*)pv;
	assert(*up == 100UL);
	free(pv);
	assert(d.pop_back(&d, &pv) == EC_OK);
	up = (unsigned long*)pv;
	assert(*up == 300UL);
	free(pv);
	assert(d.pop_front(&d, &pv) == EC_OK);
	up = (unsigned long*)pv;
	assert(*up == 200UL);
	free(pv);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_size_pos(void)
{
	ec_deque_ulong_t d;
	assert(ec_deque_ulong_init(&d, NULL) == EC_OK);
	unsigned long x = 100UL, y = 200UL, z = 300UL;
	assert(d.size_fn(&d) == 0);
	assert(d.push_back(&d, &x) == EC_OK);
	assert(d.size_fn(&d) == 1);
	assert(d.push_front(&d, &y) == EC_OK);
	assert(d.size_fn(&d) == 2);
	assert(d.push_back(&d, &z) == EC_OK);
	assert(d.size_fn(&d) == 3);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_empty_neg(void)
{
	ec_deque_ulong_t d;
	assert(ec_deque_ulong_init(&d, NULL) == EC_OK);
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
	ec_deque_ulong_t d;
	assert(ec_deque_ulong_init(&d, NULL) == EC_OK);
	void* pv = NULL;
	assert(ec_deque_ulong_init(NULL, NULL) == EC_ERR_INVALID_ARG);
	assert(d.pop_front(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.pop_back(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.peek_front(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.peek_back(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.free_fn(&d) == EC_OK);
}

int
main(void)
{
	printf("Starting ec_deque_ulong tests...\n");
	test_init_free_pos();
	test_push_front_back_pos();
	test_peek_front_back_pos();
	test_pop_front_back_pos();
	test_size_pos();
	test_empty_neg();
	test_invalid_args_neg();
	printf("All ec_deque_ulong tests passed.\n");
	return 0;
}
