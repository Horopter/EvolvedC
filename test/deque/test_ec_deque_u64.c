#include "../../src/ec_deque.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void
test_empty_neg(void)
{
	ec_deque_u64_t d;
	assert(ec_deque_u64_init(&d, NULL) == EC_OK);
	void* pv = NULL;
	assert(d.pop_front(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.pop_back(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.peek_front(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.peek_back(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_peek_pop_pos(void)
{
	ec_deque_u64_t d;
	assert(ec_deque_u64_init(&d, NULL) == EC_OK);
	void* pv = NULL;
	uint64_t x = 9, y = 99;
	uint64_t* up;
	assert(d.push_back(&d, &x) == EC_OK);
	assert(d.push_front(&d, &y) == EC_OK);
	assert(d.peek_front(&d, &pv) == EC_OK);
	up = (uint64_t*)pv;
	assert(*up == 99);
	free(pv);
	assert(d.peek_back(&d, &pv) == EC_OK);
	up = (uint64_t*)pv;
	assert(*up == 9);
	free(pv);
	assert(d.pop_back(&d, &pv) == EC_OK);
	up = (uint64_t*)pv;
	assert(*up == 9);
	free(pv);
	assert(d.pop_front(&d, &pv) == EC_OK);
	up = (uint64_t*)pv;
	assert(*up == 99);
	free(pv);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_size_pos(void)
{
	ec_deque_u64_t d;
	assert(ec_deque_u64_init(&d, NULL) == EC_OK);
	uint64_t x = 9, y = 99;
	assert(d.size_fn(&d) == 0);
	assert(d.push_back(&d, &x) == EC_OK);
	assert(d.size_fn(&d) == 1);
	assert(d.push_front(&d, &y) == EC_OK);
	assert(d.size_fn(&d) == 2);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_invalid_args_neg(void)
{
	ec_deque_u64_t d;
	assert(ec_deque_u64_init(&d, NULL) == EC_OK);
	void* pv = NULL;
	assert(ec_deque_u64_init(NULL, NULL) == EC_ERR_INVALID_ARG);
	assert(d.pop_front(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.pop_back(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.peek_front(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.peek_back(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.free_fn(&d) == EC_OK);
}

int
main(void)
{
	printf("Starting ec_deque_u64 tests...\n");
	test_empty_neg();
	test_peek_pop_pos();
	test_size_pos();
	test_invalid_args_neg();
	printf("All ec_deque_u64 tests passed.\n");
	return 0;
}
