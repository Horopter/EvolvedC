#include "../../src/ec_deque.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void
test_init_free_pos(void)
{
	ec_deque_ull_t d;
	assert(ec_deque_ull_init(&d, NULL) == EC_OK);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_peek_pos(void)
{
	ec_deque_ull_t d;
	assert(ec_deque_ull_init(&d, NULL) == EC_OK);
	unsigned long long a = 1ull, b = 2ull;
	void* pv = NULL;
	unsigned long long* up;
	assert(d.push_front(&d, &a) == EC_OK);
	assert(d.push_back(&d, &b) == EC_OK);
	assert(d.peek_front(&d, &pv) == EC_OK);
	up = (unsigned long long*)pv;
	assert(*up == 1ull);
	free(pv);
	assert(d.peek_back(&d, &pv) == EC_OK);
	up = (unsigned long long*)pv;
	assert(*up == 2ull);
	free(pv);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_pop_pos(void)
{
	ec_deque_ull_t d;
	assert(ec_deque_ull_init(&d, NULL) == EC_OK);
	unsigned long long a = 1ull, b = 2ull;
	void* pv = NULL;
	unsigned long long* up;
	assert(d.push_front(&d, &a) == EC_OK);
	assert(d.push_back(&d, &b) == EC_OK);
	assert(d.pop_front(&d, &pv) == EC_OK);
	up = (unsigned long long*)pv;
	assert(*up == 1ull);
	free(pv);
	assert(d.pop_back(&d, &pv) == EC_OK);
	up = (unsigned long long*)pv;
	assert(*up == 2ull);
	free(pv);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_size_pos(void)
{
	ec_deque_ull_t d;
	assert(ec_deque_ull_init(&d, NULL) == EC_OK);
	unsigned long long a = 1ull, b = 2ull;
	assert(d.size_fn(&d) == 0);
	assert(d.push_back(&d, &a) == EC_OK);
	assert(d.size_fn(&d) == 1);
	assert(d.push_front(&d, &b) == EC_OK);
	assert(d.size_fn(&d) == 2);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_empty_neg(void)
{
	ec_deque_ull_t d;
	assert(ec_deque_ull_init(&d, NULL) == EC_OK);
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
	ec_deque_ull_t d;
	assert(ec_deque_ull_init(&d, NULL) == EC_OK);
	void* pv = NULL;
	assert(ec_deque_ull_init(NULL, NULL) == EC_ERR_INVALID_ARG);
	assert(d.pop_front(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.pop_back(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.peek_front(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.peek_back(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.free_fn(&d) == EC_OK);
}

int
main(void)
{
	printf("Starting ec_deque_ull tests...\n");
	test_init_free_pos();
	test_peek_pos();
	test_pop_pos();
	test_size_pos();
	test_empty_neg();
	test_invalid_args_neg();
	printf("All ec_deque_ull tests passed.\n");
	return 0;
}
